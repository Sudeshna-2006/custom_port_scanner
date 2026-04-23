#include <stdio.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define SERVER_IP ""//SERVER 1P

int total_ports = 5;

void *scan_port(void *arg)
{
    int port = *(int *)arg;

    int sock;
    struct sockaddr_in server;
    char buffer[1024] = {0};

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    // 🔁 RETRY LOGIC
    int retries = 3;
    int connected = 0;

    for (int i = 0; i < retries; i++)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);

        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0)
        {
            connected = 1;
            break;
        }

        close(sock);
        sleep(1); // wait before retry
    }

    if (!connected)
    {
        return NULL; // ❌ failed after retries
    }

    // ===== BANNER GRABBING =====

    if (port == 80)
    {
        // HTTP
        char request[] = "GET / HTTP/1.1\r\n\r\n";
        send(sock, request, strlen(request), 0);

        int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';
            printf("[OPEN] Port %d | Banner: %s\n", port, buffer);
        }
        else
        {
            printf("[OPEN] Port %d | Banner: Unknown\n", port);
        }
    }
    else if (port == 8080)
    {
        // SSL
        SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);

        if (SSL_connect(ssl) > 0)
        {
            int bytes = SSL_read(ssl, buffer, sizeof(buffer)-1);

            if (bytes > 0)
            {
                buffer[bytes] = '\0';
                printf("[OPEN] Port %d | SSL Banner: %s\n", port, buffer);
            }
            else
            {
                printf("[OPEN] Port %d | SSL Banner: Unknown\n", port);
            }
        }
        else
        {
            printf("[OPEN] Port %d | SSL handshake failed\n", port);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
    }
    else
    {
        // Normal TCP
        int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';
            printf("[OPEN] Port %d | Banner: %s\n", port, buffer);
        }
        else
        {
            printf("[OPEN] Port %d | Banner: Unknown\n", port);
        }
    }

    close(sock);
    return NULL;
}

int main()
{
    int start_port, range;

    printf("Enter start port: ");
    scanf("%d", &start_port);

    printf("Enter number of ports to scan: ");
    scanf("%d", &range);

    total_ports = range;

    pthread_t threads[range];
    int ports[range];

    for (int i = 0; i < range; i++)
    {
        ports[i] = start_port + i;
    }

    clock_t start = clock();

    for (int i = 0; i < range; i++)
        pthread_create(&threads[i], NULL, scan_port, &ports[i]);

    // ✅ FIXED THREAD JOIN
    for (int i = 0; i < range; i++)
        pthread_join(threads[i], NULL);

    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    double rate = total_ports / time_taken;

    printf("\nTime: %.2f sec\n", time_taken);
    printf("Rate: %.2f ports/sec\n", rate);

    // ===== SSL CLIENT PART =====

    int sock;
    struct sockaddr_in server;
    SSL_CTX *ctx;
    SSL *ssl;
    char buffer[1024];

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_client_method());

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    printf("\nConnecting to SSL server...\n");

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("Connection failed");
        return 1;
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0)
    {
        printf("SSL connection failed\n");
        ERR_print_errors_fp(stderr);
        return 1;
    }

    printf("SSL Connected!\n");

    char msg[] = "Hello server";
    SSL_write(ssl, msg, strlen(msg));

    memset(buffer, 0, sizeof(buffer));
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));

    if (bytes > 0)
        printf("Server says: %s\n", buffer);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    return 0;
}