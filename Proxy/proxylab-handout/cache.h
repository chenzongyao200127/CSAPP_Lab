#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>
#include <pthread.h>

// Define the maximum cache size and maximum object size
#define MAX_CACHE_SIZE 1048576    // 1 MiB
#define MAX_OBJECT_SIZE 102400    // 100 KiB
#define MAX_ACTIVE_CONNECTIONS 10 // Max number of active connections

// Structure for a cache item
typedef struct CacheItem
{
    char *url;                     // Key, such as URL or file name
    char *content;                 // Stored web object content
    size_t size;                   // Size of the web object
    struct CacheItem *prev, *next; // Pointers for the doubly-linked list
} CacheItem;

// Structure for the cache
typedef struct
{
    CacheItem *head, *tail; // Pointers to the head and tail of the list
    size_t totalSize;       // Total size of all objects in the cache
    pthread_rwlock_t lock;  // Read-Write lock for the cache
} Cache;

// Function prototypes for cache operations
void initCache(Cache *cache);
void freeCache(Cache *cache);
CacheItem *findCacheItem(Cache *cache, const char *url);
void addCacheItem(Cache *cache, const char *url, const char *content, size_t size);
void removeOldestCacheItem(Cache *cache);
void printCache(Cache *cache);

#endif // CACHE_H
