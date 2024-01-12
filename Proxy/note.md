# Example code:

## Cache
为了给这个 Web 服务器增加一个 LRU（最近最少使用）缓存来存储 Web 对象，你可以设计一个简单的缓存系统。这个缓存系统需要以下几个关键部分：

1. **缓存结构体**：定义一个结构体来存储缓存项，包括 Web 对象的内容、大小、键（例如 URL 或文件名）、以及其他必要的元数据。

2. **LRU 机制**：实现一个机制来跟踪和管理最近最少使用的项。这通常需要一个队列或链表来记录访问顺序。

3. **缓存操作函数**：包括添加缓存项、查找缓存项、更新缓存项、以及在缓存满时移除最老的项等。

下面是一个简单的示例实现：

### 定义缓存结构体

```c
#define MAX_CACHE_SIZE 1024
#define MAX_OBJECT_SIZE 102400

typedef struct {
    char *url; // 键，如 URL 或文件名
    char *content; // 存储的 Web 对象内容
    size_t size; // Web 对象的大小
    // 可以添加其他元数据，如时间戳等
} CacheItem;

typedef struct {
    CacheItem items[MAX_CACHE_SIZE]; // 缓存数组
    int count; // 当前缓存数量
    // 可以添加其他控制信息，如总缓存大小等
} Cache;
```

### 实现 LRU 缓存机制

在这里，我将简化 LRU 机制的实现。为了保持简单，我们将使用一个数组来存储缓存项，并按访问顺序排列。

```c
void accessCacheItem(Cache *cache, int index) {
    // 将访问的缓存项移至数组末尾
    CacheItem item = cache->items[index];
    for (int i = index; i < cache->count - 1; i++) {
        cache->items[i] = cache->items[i + 1];
    }
    cache->items[cache->count - 1] = item;
}

CacheItem* findCacheItem(Cache *cache, const char *url) {
    for (int i = 0; i < cache->count; i++) {
        if (strcmp(cache->items[i].url, url) == 0) {
            accessCacheItem(cache, i);
            return &cache->items[i];
        }
    }
    return NULL;
}
```

### 缓存操作

```c
void addCacheItem(Cache *cache, const char *url, const char *content, size_t size) {
    // 如果缓存已满，移除最老的项（数组第一个元素）
    if (cache->count == MAX_CACHE_SIZE) {
        free(cache->items[0].url);
        free(cache->items[0].content);
        for (int i = 1; i < cache->count; i++) {
            cache->items[i - 1] = cache->items[i];
        }
        cache->count--;
    }

    // 添加新的缓存项到数组末尾
    cache->items[cache->count].url = strdup(url);
    cache->items[cache->count].content = strdup(content);
    cache->items[cache->count].size = size;
    cache->count++;
}
```

## Concurrency


~~~c
#include "csapp.h"

void echo(int connfd);
void *thread(void *vargp);

int main(int argc, char **argv)
{
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(argv[1]);
    
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Pthread_create(&tid, NULL, thread, connfdp);
    }
}

/* Thread routine */
void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    echo(connfd);
    Close(connfd);
    return NULL;
}
~~~

“分离线程”（Detached Thread）是多线程编程中的一个概念。
在多线程应用程序中，线程可以以两种状态之一运行：分离状态（detached state）或者非分离状态（joinable state）。
理解这两种状态的差异对于管理线程生命周期和资源很重要。

### 非分离状态（Joinable State）

- **Joinable**：默认情况下，线程是可汇合的（joinable）。这意味着线程完成其执行后，其他线程可以通过调用 `pthread_join()`（在 POSIX 线程中）来等待这个线程结束，并且回收其资源。这种等待过程类似于进程中的 `wait()` 调用。
- **资源回收**：在一个线程被 join 之前，它所占用的一些资源（如线程描述符和堆栈）不会被操作系统自动释放。
- **同步**：`pthread_join()` 可以用于线程间的同步，即等待特定线程完成其任务。

### 分离状态（Detached State）

- **Detached**：一个分离状态的线程在终止时会自动清理自己占用的资源。一旦线程被设置为分离状态，它不能被 join。
- **自动资源释放**：分离线程在完成执行时会自动释放其资源，包括线程ID和堆栈。因此，它不需要其他线程来显式地回收资源。
- **无法同步**：由于分离线程在结束时立即被清理，因此无法对其调用 `pthread_join()`，这意味着无法直接等待分离线程完成。

### 设置线程为分离状态

在 POSIX 线程库中，可以使用 `pthread_detach()` 函数将线程设置为分离状态：

```c
pthread_t thread;
pthread_create(&thread, NULL, &thread_function, NULL);
pthread_detach(thread);
```

或者在创建线程时设置其属性为分离：

```c
pthread_attr_t attr;
pthread_t thread;

pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
pthread_create(&thread, &attr, &thread_function, NULL);
pthread_attr_destroy(&attr);
```

### 使用场景

- 当你需要一个“火并忘记”（fire-and-forget）类型的线程，即不需要与其同步或等待其结束，那么分离线程是一个好选择。
- 如果你需要等待线程完成，以获取结果或进行资源清理，则应使用 joinable 线程。

在使用分离线程时，需要特别注意资源管理和同步问题，以避免诸如内存泄漏或竞态条件等问题。