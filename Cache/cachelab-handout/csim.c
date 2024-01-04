#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "cachelab.h"

// Define global variables for set bits (s), associativity (E), and block bits (b)
int s = 0, E = 0, b = 0;
// Pointer to hold the trace file name
char *tracefile = NULL;
// Flags for help and verbose options
int hflag = 0, verbose = 0;
// Define global variables for count (hits misses evictions)
int hits = 0, misses = 0, evictions = 0;

// Structure for a cache line
typedef struct CacheLine
{
    int valid;
    unsigned long tag;
    struct CacheLine *prev, *next;
    // Additional fields can be added here if needed
} CacheLine;

// Structure for a cache set
typedef struct CacheSet
{
    CacheLine *dummy; // Dummy node to simplify list operations
    int numLines;     // Number of lines in the set
} CacheSet;

// Parses command-line arguments to set cache parameters and flags.
void parseArgs(int argc, char *argv[]);
// Processes a single line of the cache trace, updating cache state and handling verbose output.
void parseLine(char *line, CacheSet *cache);
// Prints a summary of cache hits, misses, and evictions.
void printSummary();

// Checks if a tag is present in a given cache set and updates the set based on LRU policy.
// Returns 1 if hit, 0 if miss.
int checkAndUpdateSet(CacheSet *set, unsigned long tag);
// Adds a new line to a cache set or replaces the least recently used line based on LRU policy.
void addOrReplaceLine(CacheSet *set, unsigned long tag);
// Accesses the cache with the given address, updates cache state, and returns the result string.
void accessCache(CacheSet *cache, unsigned long address, char *result);
// Prints detailed information about a cache access (used when verbose flag is set).
void printAccessDetails(char op, unsigned long address, int size, char *result);

// Creates and initializes a single cache set with the specified number of lines (E).
CacheSet createCacheSet(int E);
// Creates the entire cache structure based on the current configuration.
CacheSet *createCache();
// Frees all memory allocated for the cache.
void freeCache(CacheSet *cache);

// Main function of the program.
int main(int argc, char *argv[])
{
    // Parse the command line arguments to set cache parameters and options.
    parseArgs(argc, argv);

    // Construct the cache data structure based on the provided parameters.
    CacheSet *cache = createCache();

    // Open the trace file for reading cache access patterns.
    FILE *file = fopen(tracefile, "r");
    if (file == NULL)
    {
        // Print an error message and exit if the trace file cannot be opened.
        perror("Error opening trace file");
        exit(EXIT_FAILURE);
    }

    char line[256]; // Buffer to hold each line read from the trace file.

    // Read the trace file line by line and process each cache access.
    while (fgets(line, sizeof(line), file))
    {
        parseLine(line, cache);
    }

    // Print a summary of cache hits, misses, and evictions after processing the trace file.
    printSummary(hits, misses, evictions);

    // Close the trace file.
    fclose(file);
    // Free all memory allocated for the cache to avoid memory leaks.
    freeCache(cache);

    return 0; // Return 0 to indicate successful execution.
}

// Function to parse command line arguments
void parseArgs(int argc, char *argv[])
{
    int opt;

    // Loop through and parse command line arguments
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h': // If option is 'h', set help flag
            hflag = 1;
            break;
        case 'v': // If option is 'v', set verbose flag
            verbose = 1;
            break;
        case 's': // If option is 's', parse and set number of set index bits
            s = atoi(optarg);
            break;
        case 'E': // If option is 'E', parse and set associativity
            E = atoi(optarg);
            break;
        case 'b': // If option is 'b', parse and set number of block bits
            b = atoi(optarg);
            break;
        case 't': // If option is 't', set tracefile name
            tracefile = optarg;
            break;
        default: /* '?' */
            // Print error message if argument is unrecognized
            fprintf(stderr, "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (hflag)
    {
        printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
        printf("Options:\n");
        printf("  -h\t\tPrint this help message.\n");
        printf("  -v\t\tVerbose mode.\n");
        printf("  -s <s>\tNumber of set index bits.\n");
        printf("  -E <E>\tAssociativity.\n");
        printf("  -b <b>\tNumber of block bits.\n");
        printf("  -t <tracefile>\tValgrind trace file.\n");
        exit(EXIT_SUCCESS);
    }

    if (s == 0 || E == 0 || b == 0 || tracefile == NULL)
    {
        fprintf(stderr, "Missing required arguments\n");
        fprintf(stderr, "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

void printAccessDetails(char op, unsigned long address, int size, char *result)
{
    printf("%c %lx,%d %s\n", op, address, size, result);
}

// This function parses a single line of trace data.
void parseLine(char *line, CacheSet *cache)
{
    char op;
    unsigned long address;
    int size;
    char result[100] = "";

    sscanf(line, " %c %lx,%d", &op, &address, &size);

    if (op != 'I')
    {
        char tempResult[50] = "";
        accessCache(cache, address, tempResult);
        strcat(result, tempResult);

        if (op == 'M')
        {
            accessCache(cache, address, tempResult);
            strcat(result, tempResult);
        }

        if (verbose)
        {
            printf("%c %lx,%d %s\n", op, address, size, result);
        }
    }
}

// Function to extract set index and tag from an address
void getSetIndexAndTag(unsigned long address, unsigned long *setIndex, unsigned long *tag)
{
    *tag = address >> (s + b);
    *setIndex = (address >> b) & ((1 << s) - 1);
}

// Function to check if a tag is in a set and update the LRU order
// Returns 1 if it's a hit, 0 otherwise. Also updates the set on a hit.
int checkAndUpdateSet(CacheSet *set, unsigned long tag)
{
    CacheLine *current = set->dummy->next;
    while (current != set->dummy)
    {
        if (current->valid && current->tag == tag)
        {
            // It's a hit. Move this line to the front of the list (most recently used)
            // Remove the current line from its position
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // Insert it at the front
            current->next = set->dummy->next;
            current->prev = set->dummy;
            set->dummy->next->prev = current;
            set->dummy->next = current;

            return 1; // Hit
        }
        current = current->next;
    }
    return 0; // Miss
}

// Function to add a new line to the set or replace the least recently used one
void addOrReplaceLine(CacheSet *set, unsigned long tag)
{
    CacheLine *last = set->dummy->prev;

    // Check if there's room to add a new line (i.e., the last line is not valid)
    if (!last->valid)
    {
        last->valid = 1;
        last->tag = tag;
    }
    else
    {
        // No room, replace the least recently used line (last line)
        last->tag = tag;
    }

    // Move the added or replaced line to the front (most recently used)
    last->prev->next = last->next;
    last->next->prev = last->prev;
    last->next = set->dummy->next;
    last->prev = set->dummy;
    set->dummy->next->prev = last;
    set->dummy->next = last;
}

// This function handles a single cache access, updating the cache state and returning the result.
void accessCache(CacheSet *cache, unsigned long address, char *result)
{
    unsigned long setIndex, tag;
    int isMiss, isEviction;

    // Extract the set index and tag from the address.
    getSetIndexAndTag(address, &setIndex, &tag);

    // Get the cache set corresponding to the set index.
    CacheSet *set = &cache[setIndex];

    // Check if the tag is in the set and update the LRU order.
    // isMiss is 1 if the tag is not found (miss), 0 if found (hit).
    isMiss = !checkAndUpdateSet(set, tag);
    // Initialize isEviction to 0. It will be set to 1 if an eviction occurs.
    isEviction = 0;

    if (isMiss) // If it's a cache miss
    {
        misses++; // Increment the global miss counter
        // Get the last line of the set to check if it's valid (used).
        CacheLine *last = set->dummy->prev;
        if (last->valid) // If the last line is valid, an eviction occurs.
        {
            isEviction = 1;
            evictions++; // Increment the global eviction counter
        }
        // Add or replace a line in the cache set.
        addOrReplaceLine(set, tag);
        strcpy(result, "miss "); // Set the result string to "miss".
    }
    else // If it's a cache hit
    {
        hits++;                 // Increment the global hit counter
        strcpy(result, "hit "); // Set the result string to "hit".
    }

    if (isEviction) // If an eviction occurred
    {
        strcat(result, "eviction "); // Append "eviction" to the result string.
    }
}

// Function to create and initialize a cache set
CacheSet createCacheSet(int E)
{
    CacheSet set;
    set.numLines = E;

    // Create and initialize the dummy node
    set.dummy = (CacheLine *)malloc(sizeof(CacheLine));
    set.dummy->prev = set.dummy;
    set.dummy->next = set.dummy;

    // Dynamically allocate and initialize the actual cache lines
    for (int i = 0; i < E; i++)
    {
        CacheLine *newLine = (CacheLine *)malloc(sizeof(CacheLine));
        newLine->valid = 0;
        newLine->tag = 0;

        // Insert the new line after the dummy node
        newLine->next = set.dummy->next;
        newLine->prev = set.dummy;
        set.dummy->next->prev = newLine;
        set.dummy->next = newLine;
    }

    return set;
}

// Function to create the entire cache
CacheSet *createCache()
{
    int numSets = 1 << s; // Calculate the number of sets (2^s)
    CacheSet *cache = (CacheSet *)malloc(numSets * sizeof(CacheSet));

    for (int i = 0; i < numSets; i++)
    {
        cache[i] = createCacheSet(E);
    }

    return cache;
}

// Example function to free the memory of the cache
void freeCache(CacheSet *cache)
{
    int numSets = 1 << s;
    for (int i = 0; i < numSets; i++)
    {
        CacheLine *current = cache[i].dummy->next;
        while (current != cache[i].dummy)
        {
            CacheLine *temp = current;
            current = current->next;
            free(temp);
        }
        free(cache[i].dummy);
    }
    free(cache);
}