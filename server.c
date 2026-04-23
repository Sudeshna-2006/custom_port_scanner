#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string.h>

#define PORT 8080

int main()
{
    int server_fd;
    struct sockaddr_in address;
    SSL_CTX *ctx;

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_server_method());

    SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        int client_fd = accept(server_fd, NULL, NULL);

        if (fork() == 0)
        {
            printf("Client connected\n");

            // ✅ CREATE SSL FIRST
            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, client_fd);

            // ✅ HANDSHAKE
            if (SSL_accept(ssl) > 0)
            {
                printf("SSL connection established\n");

                // ✅ SEND BANNER AFTER SSL
                char banner[] = "SSL Server Ready\n";
                SSL_write(ssl, banner, strlen(banner));

                char buffer[1024] = {0};
                int bytes = SSL_read(ssl, buffer, sizeof(buffer));

                if (bytes > 0)
                {
                    printf("Received: %s\n", buffer);

                    char reply[] = "Hello from server!";
                    SSL_write(ssl, reply, strlen(reply));
                }
                else
                {
                    printf("No data received\n");
                }
            }
            else
            {
                printf("SSL accept failed\n");
                ERR_print_errors_fp(stderr);
            }

            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(client_fd);
            exit(0);
        }
    }

    close(server_fd);
    SSL_CTX_free(ctx);
    return 0;
}