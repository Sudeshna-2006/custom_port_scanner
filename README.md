# 🔍 Custom Port Scanner with Service Detection

## 👤 Details
**Name:**  
**SRN:**  
**Section:**  

---

## 📌 1. Abstract

A port scanner is a network security tool used to identify open ports on a target system and determine the services running on them.

This project implements a **custom port scanner with service detection capability**, which not only checks whether a port is open but also identifies the underlying service using protocol-level communication.

The system demonstrates **real-time connection handling**, including:
- SSL-based communication  
- Banner exchange  
- Service response detection  

This helps in understanding how attackers or security tools map network services and identify potential vulnerabilities.

The project enhances knowledge of:
- TCP/IP communication  
- Socket programming  
- SSL handshaking  
- Service discovery techniques  

---

## 🔗 2. GitHub Repository Link




---

## 📸 3. Screenshots


### 🖥️ a. Server Listening on Port 8080

[1](server.png)



### 🔍 Observation

- Server is actively listening on port 8080  
- Confirms that the port is open and accepting connections  

---

### 🔗 b. Client Connection & Service Detection
[2](client.png)

### 🔍 Observation

- Client successfully connects to server  
- SSL connection is established  
- Message exchange observed  
- Service detection is successful  

---

## 🔄 4. Flow of Execution

1. Start server on port 8080  
2. Client initiates connection  
3. Port scanner detects open ports  
4. SSL handshake occurs  
5. Service responds (banner/message)  
6. Service is identified successfully  

---

## ✅ 5. Conclusion

The custom port scanner successfully detects open ports and identifies running services using protocol-level communication.

The project demonstrates key networking concepts such as:
- Socket programming  
- SSL communication  
- Service fingerprinting  

These techniques are widely used in cybersecurity tools.

---

## 🚀 Features

- 🔐 SSL-secured communication  
- ⚡ Concurrent scanning (multithreading)  
- 🔁 Retry logic  
- ⏱️ Timeout handling  
- 📡 Service detection (banner grabbing)  
- 📊 Scan efficiency calculation  

---

## ⚙️ How to Run

### Compile


### Run


---

## 🌐 Network Setup

- Use same WiFi / hotspot  
- Update client with server IP:

```c
inet_pton(AF_INET, "SERVER_IP", &server.sin_addr);