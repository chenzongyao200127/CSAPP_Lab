#include <stdio.h>
#include <getopt.h>
#include "cache.h"
#include "csapp.h"

#define DEBUG 1 // 设置为1以启用调试信息，设置为0以禁用

Cache cache;

/* Function to parse command line arguments */
void parseArgs(int argc, char *argv[]);
void *handleRequest(void *vargp);
void parseRequestLine(int connfd, char *method, char *uri, char *version);
void parseURI(const char *uri, char *hostname, char *path, int *port);
void forwardResponseToClient(int connfd, const char *content, size_t size);
int forwardRequestToServer(const char *hostname, int port, const char *headers, const char *path, const char *method, const char *version);
CacheItem *checkCache(Cache *cache, const char *uri);
void storeResponseInCache(Cache *cache, const char *uri, const char *content, size_t size);
void buildRequestHeaders(const char *hostname, int port, char *headers, int connfd);
int receiveResponseFromServer(int serverfd, char *response, int maxSize);

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    parseArgs(argc, argv);

    // 初始化全局变量 cache
    initCache(&cache);

    listenfd = Open_listenfd(argv[1]);

    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);

        // 将 connfd 的地址传递给 handleRequest
        int *connfdp = malloc(sizeof(int));
        *connfdp = connfd;
        Pthread_create(&tid, NULL, handleRequest, connfdp);
    }

    freeCache(&cache);
    Close(listenfd);

    return 0;
}

// Function to parse command line arguments (`linux> ./proxy 15213`)
void parseArgs(int argc, char *argv[])
{
    // Check if the command line argument count is correct
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Convert the port argument to an integer
    long val = strtol(argv[1], NULL, 10);

    // Check if the port number is within the valid range
    if (val < 1024 || val > 65535)
    {
        fprintf(stderr, "Invalid port number. Choose a port between 1024 and 65535.\n");
        exit(1);
    }
}

void parseRequestLine(int connfd, char *method, char *uri, char *version)
{
    char buf[MAXLINE];
    rio_t rio;

    // 初始化 Robust I/O 结构体
    Rio_readinitb(&rio, connfd);

    // 读取一行数据（请求行）
    if (Rio_readlineb(&rio, buf, MAXLINE) <= 0)
    {
        fprintf(stderr, "Error reading request line from client.\n");
        return;
    }

    // 解析方法、URI和版本
    sscanf(buf, "%s %s %s", method, uri, version);
}

void forwardResponseToClient(int connfd, const char *content, size_t size)
{
    Rio_writen(connfd, content, size);
}

void parseURI(const char *uri, char *hostname, char *path, int *port)
{
    // 解析URI，假设URI的格式为 http://hostname[:port]/path
    char *pos = strstr(uri, "://");
    if (pos == NULL)
    {
        pos = uri;
    }
    else
    {
        pos += 3;
    }

    char *pos2 = strstr(pos, ":");
    if (pos2 != NULL)
    {
        *pos2 = '\0';
        sscanf(pos, "%s", hostname);
        sscanf(pos2 + 1, "%d%s", port, path);
    }
    else
    {
        pos2 = strstr(pos, "/");
        if (pos2 != NULL)
        {
            *pos2 = '\0';
            sscanf(pos, "%s", hostname);
            *pos2 = '/';
            strcpy(path, pos2);
        }
        else
        {
            sscanf(pos, "%s", hostname);
            strcpy(path, "/");
        }
    }
}

void buildRequestHeaders(const char *hostname, int port, char *headers, int connfd)
{
    char buf[MAXLINE];
    rio_t rio;

    // 构建请求头
    // 如果端口不是80，则在Host头部中包含端口号
    if (port == 80)
    {
        sprintf(headers, "Host: %s\r\n", hostname);
    }
    else
    {
        sprintf(headers, "Host: %s:%d\r\n", hostname, port);
    }
    strcat(headers, "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n");
    strcat(headers, "Connection: close\r\n");
    strcat(headers, "Proxy-Connection: close\r\n");

    // 转发客户端的其他头部，但要避免头部重复
    while (Rio_readlineb(&rio, buf, MAXLINE) > 0)
    {
        if (strcmp(buf, "\r\n") == 0)
        {
            break; // 头部结束
        }
        if (strncasecmp(buf, "Host:", 5) == 0 ||
            strncasecmp(buf, "User-Agent:", 11) == 0 ||
            strncasecmp(buf, "Connection:", 11) == 0 ||
            strncasecmp(buf, "Proxy-Connection:", 17) == 0)
        {
            continue; // 跳过这些头部
        }
        strcat(headers, buf);

        // Debug: 打印每行内容
        if (DEBUG)
        {
            printf("Debug: Read header line: %s", buf);
        }
    }
}

int forwardRequestToServer(const char *hostname, int port, const char *headers, const char *path, const char *method, const char *version)
{
    int serverfd;
    char portStr[10];

    // 将端口号转换为字符串
    sprintf(portStr, "%d", port);

    if (DEBUG)
    {
        printf("Debug: Connecting to server %s on port %s\n", hostname, portStr);
    }

    // 连接到服务器
    serverfd = Open_clientfd(hostname, portStr);
    if (serverfd < 0)
    {
        fprintf(stderr, "Unable to connect to server.\n");
        if (DEBUG)
        {
            fprintf(stderr, "Debug: Failed to connect to server %s on port %s\n", hostname, portStr);
        }
        return -1;
    }

    if (DEBUG)
    {
        printf("Debug: Connected to server, sending request...\n");
    }

    // 构建并发送请求
    char request[MAXLINE];
    sprintf(request, "%s %s %s\r\n", method, path, version);
    Rio_writen(serverfd, request, strlen(request));
    Rio_writen(serverfd, headers, strlen(headers));

    if (DEBUG)
    {
        printf("Debug: Request sent to server:\n%s%s\n", request, headers);
    }

    return serverfd;
}

int receiveResponseFromServer(int serverfd, char *response, int maxSize)
{
    int n;
    int totalSize = 0;
    char buf[MAXLINE];
    rio_t rio;

    // 初始化 Robust I/O 结构体
    Rio_readinitb(&rio, serverfd);

    // 读取服务器的响应
    while ((n = Rio_readnb(&rio, buf, MAXLINE)) > 0)
    {
        if (totalSize + n <= maxSize)
        {
            memcpy(response + totalSize, buf, n);
            totalSize += n;
        }
        else
        {
            if (DEBUG)
            {
                printf("Debug: Response is too large to store completely. Truncating.\n");
            }
            break; // 响应太大，不完全存储
        }

        if (DEBUG)
        {
            printf("Debug: Received %d bytes from server, total received: %d bytes\n", n, totalSize);
        }
    }

    if (n < 0)
    {
        if (DEBUG)
        {
            printf("Debug: Error occurred while reading from server.\n");
        }
        return -1; // 读取错误
    }

    return totalSize; // 返回响应的大小
}

void *handleRequest(void *vargp)
{
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);

    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], path[MAXLINE];
    int serverfd, port = 8080; // 默认 Server HTTP 端口为 8080
    char headers[MAXLINE];

    if (DEBUG)
        printf("Debug: handleRequest started for fd %d\n", connfd);

    // 解析请求行
    parseRequestLine(connfd, method, uri, version);
    if (DEBUG)
        printf("Debug: Request Line: %s %s %s\n", method, uri, version);

    // 检查缓存
    CacheItem *item = findCacheItem(&cache, uri);
    if (item != NULL)
    {
        if (DEBUG)
            printf("Debug: Cache hit for %s\n", uri);
        // 缓存命中，直接发送缓存内容
        forwardResponseToClient(connfd, item->content, item->size);
    }
    else
    {
        if (DEBUG)
            printf("Debug: Cache miss for %s\n", uri);

        // 缓存未命中，解析 URI 并构建请求头
        parseURI(uri, hostname, path, &port);

        if (DEBUG)
        {
            printf("Debug: Parsed URI:\n");
            printf("  Hostname: %s\n", hostname);
            printf("  Path: %s\n", path);
            printf("  Port: %d\n", port);
        }

        buildRequestHeaders(hostname, port, headers, connfd);

        if (DEBUG)
        {
            printf("Debug: Constructed request headers:\n%s\n", headers);
        }

        // 转发请求到服务器
        serverfd = forwardRequestToServer(hostname, port, headers, path, method, version);
        if (serverfd < 0)
        {
            if (DEBUG)
                printf("Debug: Failed to connect to server %s\n", hostname);
            close(connfd);
            return NULL;
        }

        // 接收服务器响应
        char serverResponse[MAX_OBJECT_SIZE];
        int responseSize = receiveResponseFromServer(serverfd, serverResponse, MAX_OBJECT_SIZE);
        if (DEBUG)
            printf("Debug: Received response of size %d from server\n", responseSize);

        // 存储响应到缓存
        addCacheItem(&cache, uri, serverResponse, responseSize);

        // 转发响应到客户端
        forwardResponseToClient(connfd, serverResponse, responseSize);

        // 关闭服务器套接字
        close(serverfd);
    }

    // 关闭客户端连接
    close(connfd);
    if (DEBUG)
        printf("Debug: Connection closed for fd %d\n", connfd);
    return NULL;
}