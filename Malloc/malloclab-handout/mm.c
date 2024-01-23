/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "SEU",
    /* First member's full name */
    "Chen Zongyao",
    /* First member's email address */
    "solar1s@seu.edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* Basic constants and macros */
#define WSIZE 4             /* Word and header/footer size (bytes) */
#define DSIZE 8             /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Extend heap by this amont (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack and size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at adderss p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

/* Give free block ptr bp, get the val of `pred` and `succ` of the free block */
#define GET_PRED(p) (*(void **)(p))
#define GET_SUCC(p) (*(void **)((p) + WSIZE))

/* Give free block ptr bp, set the val of `pred` and `succ` of the free block */
#define SET_PRED(p, pred) (*(void **)(p) = (pred))
#define SET_SUCC(p, succ) (*(void **)((p) + WSIZE) = (succ))

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Debug option */
#define VERBOSE 0

/* Turn off heap checker */
// #define check(verbose, lineo) mm_check(verbose, lineo)
#define check(verbose, lineo)

/* Global variables */
static char *heap_listp = 0;   /* Pointer to first block */
static char *dummy_blockp = 0; /* Pointer to first dummy free block */

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
void mm_check(int verbose, int lineno);

// free list manipulate
static void add_to_free_list(char *bp);
static int is_in_free_list(char *bp);
static void remove_from_free_list(char *bp);

static void *coalesce(char *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

void printMemoryContent(void *ptr);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Initialize the heap list pointer to create an empty heap.
       Allocate memory space for the additional blocks, checking for failure. */
    if ((heap_listp = mem_sbrk((2 + 4 + 2) * WSIZE)) == (void *)-1) // Allocate space for dummy block and its pointers
        return -1;

    /* Set up the alignment padding at the beginning of the heap. */
    PUT(heap_listp, 0);

    /* Initialize the Prologue */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));

    /* Initialize the dummy block. */
    dummy_blockp = heap_listp + (4 * WSIZE); // set global dummy pointer

    PUT(heap_listp + (3 * WSIZE), PACK(2 * DSIZE, 1)); // Dummy block header
    PUT(heap_listp + (4 * WSIZE), dummy_blockp);       // Dummy block predecessor pointer -> self
    PUT(heap_listp + (5 * WSIZE), dummy_blockp);       // Dummy block successor pointer   -> self
    PUT(heap_listp + (6 * WSIZE), PACK(2 * DSIZE, 1)); // Dummy block footer

    /* Set up the epilogue header, which is now two blocks further. */
    PUT(heap_listp + (7 * WSIZE), PACK(0, 1));

    /* Move heap list pointer to the prologue */
    heap_listp += (2 * WSIZE);

    /* Extend the heap with a free block of size CHUNKSIZE. */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    // printf("After mm_init");
    check(VERBOSE, __LINE__);

    /* If everything is successful, return 0 indicating successful initialization. */
    return 0;
}

/*
 * extend_heap - extend heap CHUNKSIZE bytes, Initialize free block and the epilogue header
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;

    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    // printMemoryContent(heap_listp - (2 * WSIZE));

    // printf("After extend_heap");
    // check(VERBOSE, __LINE__);

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;      // Adjusted block size
    size_t extendsize; // Amount to extend heap if no fit
    char *bp;          // Pointer to the block

    // Initialize the memory allocator if it hasn't been initialized yet
    if (heap_listp == 0)
    {
        mm_init();
    }

    // Ignore spurious requests
    if (size == 0)
    {
        return NULL;
    }

    // Adjust block size to include overhead and alignment requirements
    if (size <= DSIZE)
    {
        asize = 2 * DSIZE; // Minimum block size
    }
    else
    {
        // Adjust size to be aligned and include overhead
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    }

    // Search the free list for a fit
    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize); // Place the block if a fit is found
        return bp;
    }

    // No fit found. Get more memory and place the block
    extendsize = MAX(asize, CHUNKSIZE); // Calculate the size to extend the heap
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    {
        return NULL; // Return NULL if unable to extend heap
    }
    place(bp, asize); // Place the block in the new heap space

    // printf("After mm_malloc");
    check(VERBOSE, __LINE__);

    // printf("[🐅] malloc ok\n\n");

    return bp;
}

/*
 * find_fit - First fit search of the free list to find a block that fits
 */
static void *find_fit(size_t asize)
{
    void *bp;

    // Start the search from the block following the dummy block in the free list
    for (bp = GET_SUCC(dummy_blockp); bp != dummy_blockp; bp = GET_SUCC(bp))
    {
        if (asize <= GET_SIZE(HDRP(bp)))
        {
            return bp; // Found a block that is large enough
        }
    }

    return NULL; // No suitable block found
}

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp)); // Get the current block size from the header

    if ((csize - asize) >= (2 * DSIZE))
    {                                  // Check if the remaining space after allocation is enough for a new block
        PUT(HDRP(bp), PACK(asize, 1)); // Set the header for the new block of size asize
        PUT(FTRP(bp), PACK(asize, 1)); // Set the footer for the new block of size asize
        remove_from_free_list(bp);

        bp = NEXT_BLKP(bp);                    // Move the block pointer to the next block
        PUT(HDRP(bp), PACK(csize - asize, 0)); // Set the header for the remainder block
        PUT(FTRP(bp), PACK(csize - asize, 0)); // Set the footer for the remainder block
        coalesce(bp);
    }

    else
    {
        PUT(HDRP(bp), PACK(csize, 1)); // If splitting is not possible, update the current block as allocated
        PUT(FTRP(bp), PACK(csize, 1)); // Set both header and footer to mark the entire block as allocated
        remove_from_free_list(bp);
    }

    // printf("After place");
    // check(VERBOSE, __LINE__);
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    // If the block pointer is NULL, return immediately as there's nothing to free.
    if (bp == 0)
        return;

    // Get the size of the block from its header.
    size_t size = GET_SIZE(HDRP(bp));

    // If the heap list pointer is NULL, initialize the memory manager.
    if (heap_listp == 0)
    {
        mm_init();
    }

    // Set the header and footer of the block to indicate that it's free.
    // PACK combines the size and the free/allocated bit.
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    // Coalesce, if possible, with adjacent free blocks.
    coalesce(bp);

    // printf("After mm_free");
    check(VERBOSE, __LINE__);

    // printf("[🐖] free OK\n\n");
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(char *bp)
{
    if (bp == NULL)
    {
        return NULL;
    }

    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    /* Case 1: No coalescing */
    if (prev_alloc && next_alloc)
    {
        add_to_free_list(bp);
        return bp;
    }

    /* Case 2: Coalesce with next block */
    if (prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_from_free_list(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    /* Case 3: Coalesce with previous block */
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        bp = PREV_BLKP(bp);
        remove_from_free_list(bp);
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    /* Case 4: Coalesce with both previous and next blocks */
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_from_free_list(PREV_BLKP(bp));
        remove_from_free_list(NEXT_BLKP(bp));
        bp = PREV_BLKP(bp);
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    add_to_free_list(bp);

    return bp;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0)
    {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
    {
        return mm_malloc(size);
    }

    newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if (!newptr)
        return 0;

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(ptr));
    if (size < oldsize)
        oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    return newptr;
}

/*
 * printblock - Prints the details of a block pointed to by bp.
 */
static void printblock(void *bp, int lineno)
{
    size_t hsize, halloc; // Declare variables for size and allocation status.

    // Retrieve size and allocation status from the block's header.
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));

    // If the block size is zero, it's the end of the list. Print EOL and return.
    if (hsize == 0 && halloc == 1)
    {
        // print the epilogue block
        printf("%p: EOL\n", bp);
        return;
    }

    // Print the block pointer and details about the block's header.
    printf("%p: header: [%ld:%c]\n", bp, hsize, (halloc ? 'a' : 'f'));
}

/*
 * checkblock - Checks a block for consistency.
 */
static void checkblock(void *bp, int lineno)
{
    // Check if the block pointer is doubleword aligned (8-byte aligned).
    if ((size_t)bp % 8)
        printf("Line %d Error: %p is not doubleword aligned\n", lineno, bp);

    // Check if the header and footer of the block match.
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
        printf("Line %d Error: header does not match footer, curr bp: %p \n", lineno, bp);
}

/*
 * check_freelist() - Checks heap Free list
 */
static int check_freelist(int lineno)
{
    char *bp;

    // Loop through all blocks in the free list
    for (bp = GET_SUCC(dummy_blockp); bp != dummy_blockp; bp = GET_SUCC(bp))
    {
        // Check if the current block is allocated, which should not be the case for blocks in the free list
        if (GET_ALLOC(HDRP(bp)))
        {
            printf("Line %d: Allocated block found in free list at %p\n", lineno, bp);
            return 0;
        }

        // Check if there are contiguous free blocks that should have been coalesced
        if (!GET_ALLOC(HDRP(bp)) && !GET_ALLOC(HDRP(PREV_BLKP(bp))))
        {
            printf("Line %d: Contiguous free blocks that escaped coalescing found.\n", lineno);
            return 0;
        }
    }

    // Check if every free block is actually in the free list
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (!GET_ALLOC(HDRP(bp)) && !is_in_free_list(bp))
        {
            printf("Line %d: Free block not in free list found at %p\n", lineno, bp);
            return 0;
        }
    }

    return 1; // Return 1 to indicate successful check if no errors were found
}

/*
 * checkheap - Performs a minimal check of the heap for consistency.
 */
void checkheap(int verbose, int lineno)
{
    char *bp = heap_listp; // Set bp (block pointer) to start of the heap.

    // If verbose mode is enabled, print the heap starting address.
    if (verbose)
        printf("[⭐] Start of The Heap (%p):\n", heap_listp);

    // Check if the size of the prologue block is correct and if it is allocated.
    // The prologue block should have a size of DSIZE and be allocated.
    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");
    checkblock(heap_listp, lineno); // Check the prologue block for consistency.

    // Iterate through the blocks in the heap.
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        // printf("line: %d\n", lineno);
        // printf("cur dp: %p\n", bp);
        // printf("GET_SIZE(HDRP(bp)): %d\n", GET_SIZE(HDRP(bp)));
        // printf("next dp: %p\n", NEXT_BLKP(bp));
        // If verbose mode is enabled, print details of the current block.
        if (verbose)
            printblock(bp, lineno);
        checkblock(bp, lineno); // Check the current block for consistency.

        if (GET_ALLOC(HDRP(bp)) && GET_ALLOC(NEXT_BLKP(bp)))
        {
            if (FTRP(bp) > HDRP(NEXT_BLKP(bp)))
            {
                printf("Line %d: Overlapping allocated blocks found.\n", lineno);
                return 0;
            }
        }
    }

    // If verbose mode is enabled, print the epilogue block.
    if (verbose)
    {
        printblock(bp, lineno);
    }

    // Check if the size of the epilogue block is zero and if it is allocated.
    // The epilogue block should have a size of 0 and be allocated.
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
    {
        printf("current bp: %p\n", bp);
        printMemoryContent(bp - WSIZE);
        printf("Line %d: Bad epilogue header.\n", lineno);
    }

    // check free list
    // assert(check_freelist(lineno));
}

/*
 * mm_check - debug tool
 */
void mm_check(int verbose, int lineno)
{
    checkheap(verbose, lineno);

    printf("[*] MM CHECK: Pass :) \n");
    return 1;
}

/*
 * is_in_free_list - give a free block pointer bp, check whether it is in the free list
 */
static int is_in_free_list(char *bp)
{
    char *current_bp = GET_SUCC(dummy_blockp); // Start from the first block in the free list

    // Traverse the free list until we loop back to the dummy block
    while (current_bp != dummy_blockp)
    {
        if (current_bp == bp)
        {
            return 1; // Found the block in the free list
        }
        current_bp = GET_SUCC(current_bp); // Move to the next block in the free list
    }

    return 0; // Block not found in the free list
}

/*
 * add_to_free_list - give a free block pointer bp, add this block to the LIFO queue
 */
static void add_to_free_list(char *bp)
{
    char *next_bp = GET_SUCC(dummy_blockp);

    // Set the successor of the new block to be the current successor of the dummy block
    SET_SUCC(bp, next_bp);

    // Set the predecessor of the successor of the dummy block to be the new block
    SET_PRED(next_bp, bp);

    // Set the predecessor of the new block to be the dummy block
    SET_PRED(bp, dummy_blockp);

    // Update the dummy block's successor to the new block
    SET_SUCC(dummy_blockp, bp);
}

/*
 * remove_from_free_list - given a free block pointer bp, remove it from the free block list
 */
static void remove_from_free_list(char *bp)
{
    char *prev_bp = GET_PRED(bp);
    char *next_bp = GET_SUCC(bp);

    SET_SUCC(prev_bp, next_bp);
    SET_PRED(next_bp, prev_bp);
}

// Define a function to print the memory contents at a given pointer.
void printMemoryContent(void *ptr)
{
    // Convert the void pointer to an unsigned char pointer for byte-wise access.
    unsigned char *bytePtr = (unsigned char *)ptr;

    // Print the address of the memory location being accessed.
    printf("Memory content at [%p]:\n", ptr);

    // Iterate over a fixed number of bytes (WSIZE * 10).
    for (int i = 0; i < WSIZE * 10; i++)
    {
        // Add a space after every WSIZE bytes for readability.
        if (i > 0 && i % WSIZE == 0)
        {
            printf(" ");
        }

        // Calculate the index for big-endian byte order.
        // This reorders the byte sequence for each WSIZE block.
        int bigEndianIndex = (i / WSIZE) * WSIZE + WSIZE - 1 - (i % WSIZE);

        // Print the byte at the calculated index in hexadecimal format.
        printf("%02x", bytePtr[bigEndianIndex]);
    }

    // Print a newline at the end for better output formatting.
    printf("\n");
}