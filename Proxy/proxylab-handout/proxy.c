#include <stdio.h>
#include <getopt.h>
#include "cache.h"
#include "csapp.h"

#define DEBUG 0 // 设置为1以启用调试信息，设置为0以禁用

// Global cache variable
Cache cache;

struct Uri
{
    char host[100];
    char port[6];
    char path[100];
};

void parseArgs(int argc, char *argv[]);
void *handleRequest(void *vargp);
void parse_uri(char *uri, struct Uri *uri_data);
void build_header(char *http_header, struct Uri *uri_data, rio_t *client_rio);

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    parseArgs(argc, argv);

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

void parse_uri(char *uri, struct Uri *uri_data)
{
    // Initialize default values
    strcpy(uri_data->port, "80");
    uri_data->path[0] = '\0';

    // Find the position of "//" to identify the host
    char *host_start = strstr(uri, "//");
    if (host_start == NULL)
    {
        // If no host specified, the entire URI is a path
        strcpy(uri_data->path, uri);
        return;
    }

    // Move past the "//"
    host_start += 2;

    // Find the next "/" to isolate the host and path
    char *path_start = strchr(host_start, '/');
    if (path_start != NULL)
    {
        strcpy(uri_data->path, path_start);
        *path_start = '\0'; // Terminate the host string
    }

    // Find the port, if specified
    char *port_start = strchr(host_start, ':');
    if (port_start != NULL)
    {
        sscanf(port_start + 1, "%5s", uri_data->port); // Read port (max 5 digits)
        *port_start = '\0';                            // Terminate the host string
    }

    // Copy the host into the uri_data
    strcpy(uri_data->host, host_start);
}

void build_header(char *http_header, struct Uri *uri_data, rio_t *client_rio)
{
    // Standard headers
    char *User_Agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
    char *conn_hdr = "Connection: close\r\n";
    char *prox_hdr = "Proxy-Connection: close\r\n";
    char *host_hdr_format = "Host: %s\r\n";
    char *requestlint_hdr_format = "GET %s HTTP/1.0\r\n";
    char *endof_hdr = "\r\n";

    char buf[MAXLINE], request_hdr[MAXLINE], other_hdr[MAXLINE] = "", host_hdr[MAXLINE] = "";
    sprintf(request_hdr, requestlint_hdr_format, uri_data->path);

    while (Rio_readlineb(client_rio, buf, MAXLINE) > 0)
    {
        if (DEBUG)
            printf("Header line: %s", buf);

        if (strcmp(buf, endof_hdr) == 0)
        {
            break; // EOF
        }

        if (!strncasecmp(buf, "Host", strlen("Host")))
        {
            strcpy(host_hdr, buf);
            continue;
        }

        if (!strncasecmp(buf, "Connection", strlen("Connection")) &&
            !strncasecmp(buf, "Proxy-Connection", strlen("Proxy-Connection")) &&
            !strncasecmp(buf, "User-Agent", strlen("User-Agent")))
        {
            strcat(other_hdr, buf);
        }
    }

    if (strlen(host_hdr) == 0)
    {
        sprintf(host_hdr, host_hdr_format, uri_data->host);
    }

    sprintf(http_header, "%s%s%s%s%s%s%s",
            request_hdr,
            host_hdr,
            conn_hdr,
            prox_hdr,
            User_Agent,
            other_hdr,
            endof_hdr);

    if (DEBUG)
        printf("Constructed HTTP Header:\n%s", http_header);
}

void *handleRequest(void *vargp)
{
    int connfd = *((int *)vargp);
    free(vargp); // Assuming the caller allocated memory for connfd

    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char server[MAXLINE];
    rio_t rio, server_rio;

    // Initialize rio for the client connection
    Rio_readinitb(&rio, connfd);

    // Read the request line
    if (!Rio_readlineb(&rio, buf, MAXLINE))
    {
        fprintf(stderr, "Error reading request line\n");
        return NULL;
    }

    // Parse the request line
    if (sscanf(buf, "%s %s %s", method, uri, version) != 3)
    {
        fprintf(stderr, "Invalid request line\n");
        return NULL;
    }

    // Handle only GET requests
    if (strcasecmp(method, "GET") != 0)
    {
        fprintf(stderr, "Proxy does not implement the method\n");
        return NULL;
    }

    // Parse the URI to get the host, port, and path
    struct Uri *uri_data = malloc(sizeof(struct Uri));
    if (!uri_data)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    parse_uri(uri, uri_data);

    // Generate the cache key as a combination of host, port, and path
    char cache_key[MAXLINE];
    snprintf(cache_key, sizeof(cache_key), "%s:%s%s", uri_data->host, uri_data->port, uri_data->path);

    // Check if the generated cache key is in cache
    CacheItem *item = findCacheItem(&cache, cache_key);

    if (DEBUG)
        printCache(&cache);

    if (item)
    {
        Rio_writen(connfd, item->content, item->size);
    }
    else
    {
        build_header(server, uri_data, &rio);

        // Connect to the server
        int serverfd = Open_clientfd(uri_data->host, uri_data->port);
        if (serverfd < 0)
        {
            fprintf(stderr, "Connection to server failed\n");
            free(uri_data);
            return NULL;
        }

        // Forward the request to the server
        Rio_readinitb(&server_rio, serverfd);
        Rio_writen(serverfd, server, strlen(server));

        // Forward the server's response back to the client and cache it
        char *response = malloc(MAXLINE); // Allocate memory for caching
        size_t total_size = 0;
        ssize_t n;
        while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0)
        {
            Rio_writen(connfd, buf, n);
            // Append to response buffer for caching
            if (response)
            {
                response = realloc(response, total_size + n);
                memcpy(response + total_size, buf, n);
                total_size += n;
            }
        }

        // Add the response to cache
        if (response)
        {
            addCacheItem(&cache, cache_key, response, total_size);
            free(response); // Free the temporary response buffer
        }

        // Clean up
        Close(serverfd);
        free(uri_data);
    }

    Close(connfd);
    return NULL;
}