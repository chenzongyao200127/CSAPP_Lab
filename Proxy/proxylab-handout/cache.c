#include "cache.h"
#include <string.h>
#include <stdio.h>

// Initialize the cache, including the dummy head node
void initCache(Cache *cache)
{
    // Allocate memory for the dummy head node
    cache->head = malloc(sizeof(CacheItem));
    if (cache->head == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for cache head.\n");
        return;
    }

    // Initialize the dummy head node
    cache->head->url = NULL;
    cache->head->content = NULL;
    cache->head->size = 0;
    cache->head->prev = NULL;
    cache->head->next = NULL;
    cache->tail = cache->head;
    cache->totalSize = 0;

    // Initialize the read-write lock
    pthread_rwlock_init(&cache->lock, NULL);
}

// Free all the cache items and the cache itself
void freeCache(Cache *cache)
{
    // Acquire write lock before modifying the cache
    pthread_rwlock_wrlock(&cache->lock);

    CacheItem *current = cache->head->next; // Start with the first real item
    while (current != NULL)
    {
        CacheItem *next = current->next;
        free(current->url);
        free(current->content);
        free(current);
        current = next;
    }
    free(cache->head); // Free the dummy head node
    cache->head = cache->tail = NULL;
    cache->totalSize = 0;

    // Release the lock
    pthread_rwlock_unlock(&cache->lock);

    // Destroy the read-write lock
    pthread_rwlock_destroy(&cache->lock);
}

// Move a cache item to the front of the list (most recently used)
void moveToFront(Cache *cache, CacheItem *item)
{
    if (item == cache->head->next)
    {
        // Item is already at the front
        return;
    }

    // Detach item from its current position
    if (item->prev)
        item->prev->next = item->next;
    if (item->next)
        item->next->prev = item->prev;

    // If item was the tail, update the tail pointer
    if (item == cache->tail)
    {
        cache->tail = item->prev;
    }

    // Attach item at the front
    item->next = cache->head->next;
    item->prev = cache->head;
    if (cache->head->next)
    {
        cache->head->next->prev = item;
    }
    cache->head->next = item;

    // Update tail if the list was empty
    if (cache->tail == cache->head)
    {
        cache->tail = item;
    }
}

// Find a cache item by URL. If found, move it to the front (as it's now most recently used)
CacheItem *findCacheItem(Cache *cache, const char *url)
{
    // Acquire read lock for searching
    pthread_rwlock_rdlock(&cache->lock);

    CacheItem *current = cache->head->next;
    while (current != NULL)
    {
        if (strcmp(current->url, url) == 0)
        {
            // Move the found item to the front of the cache
            moveToFront(cache, current);
            return current;
        }
        current = current->next;
    }
    // Release the read lock
    pthread_rwlock_unlock(&cache->lock);

    return NULL; // Not found
}

// Remove the oldest cache item (least recently used)
void removeOldestCacheItem(Cache *cache)
{
    if (cache->tail == cache->head)
    {
        // Cache is empty
        return;
    }

    CacheItem *oldest = cache->tail;
    cache->tail = oldest->prev;
    cache->tail->next = NULL;

    cache->totalSize -= oldest->size;
    free(oldest->url);
    free(oldest->content);
    free(oldest);
}

// Add a new cache item. If the cache is full, remove the oldest items first.
void addCacheItem(Cache *cache, const char *url, const char *content, size_t size)
{
    // Acquire write lock before modifying the cache
    pthread_rwlock_wrlock(&cache->lock);

    // Check if the object size exceeds the maximum object size
    if (size > MAX_OBJECT_SIZE)
    {
        fprintf(stderr, "Object size exceeds the maximum limit.\n");
        return;
    }

    // Remove the oldest cache items until there is enough space
    while (cache->totalSize + size > MAX_CACHE_SIZE)
    {
        removeOldestCacheItem(cache);
    }

    // Create a new cache item
    CacheItem *newItem = malloc(sizeof(CacheItem));
    if (newItem == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for new cache item.\n");
        return;
    }
    newItem->url = strdup(url);
    newItem->content = strdup(content);
    newItem->size = size;

    // Insert the new item at the front of the cache
    newItem->prev = cache->head;
    newItem->next = cache->head->next;
    if (cache->head->next)
    {
        cache->head->next->prev = newItem;
    }
    else
    {
        cache->tail = newItem; // If cache was empty, update tail
    }
    cache->head->next = newItem;
    cache->totalSize += size;

    // Release the read lock
    pthread_rwlock_unlock(&cache->lock);
}

void printCache(Cache *cache)
{
    // Acquire the read lock to ensure thread safety
    pthread_rwlock_rdlock(&cache->lock);

    printf("Cache Contents (Total Size: %lu):\n", cache->totalSize);
    printf("-----------------------------------\n");

    // Traverse the list from head to tail
    CacheItem *current = cache->head;
    while (current != NULL)
    {
        printf("URL: %s\n", current->url);
        // printf("Content: %s\n", current->content);
        printf("Size: %lu\n", current->size);
        printf("-----------------------------------\n");

        // Move to the next item
        current = current->next;
    }

    // Release the read lock
    pthread_rwlock_unlock(&cache->lock);
}