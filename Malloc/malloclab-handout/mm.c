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
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* Basic constants and macros */
#define WSIZE 4     /* Word and header/footer size (bytes) */
#define DSIZE 8     /* Double word size (bytes) */
#define CHUNKSIZE (1<<12)   /* Extend heap by this amont (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack and size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at adderss p */
#define GET(p)       (*(unsigned int *)(p))            
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & (~0x7))
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Give free block ptr bp, get the val of `pred` and `succ` of the free block */
#define GET_PRED(p)  (*(void **)(p))
#define GET_SUCC(p)  (*(void **)((p) + WSIZE))

/* Give free block ptr bp, set the val of `pred` and `succ` of the free block */
#define SET_PRED(p, pred)  (*(void **)(p) = (pred))
#define SET_SUCC(p, succ)  (*(void **)((p) + WSIZE) = (succ))

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Global variables */
static char *heap_listp = 0;        /* Pointer to first block */  
static char *dummy_blockp = 0;      /* Pointer to first dummy free block */

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
int mm_check(void);
static void add_to_free_list(char *bp);
static int is_in_free_list(char *bp);
static void remove_from_free_list(char* bp);
static void *coalesce(char *bp);


/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Initialize the heap list pointer to create an empty heap. 
       Allocate memory space for the additional blocks, checking for failure. */
    if ((heap_listp = mem_sbrk((4 + 2) * WSIZE)) == (void *) - 1) // Allocate space for dummy block and its pointers
        return -1;

    /* Set up the alignment padding at the beginning of the heap. */
    PUT(heap_listp, 0);                           

    /* Initialize the Prologue */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); 
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); 

    /* Initialize the dummy block. */
    PUT(heap_listp + (3 * WSIZE), PACK(DSIZE, 1)); // Dummy block header
    PUT(heap_listp + (4 * WSIZE), 0); // Dummy block predecessor pointer (null)
    PUT(heap_listp + (5 * WSIZE), 0); // Dummy block successor pointer (null)
    PUT(heap_listp + (6 * WSIZE), PACK(DSIZE, 1)); // Dummy block footer

    dummy_blockp = heap_listp + (4 * WSIZE); // set global dummy pointer

    /* Set up the epilogue header, which is now two blocks further. */
    PUT(heap_listp + (7 * WSIZE), PACK(0, 1));     

    /* Move heap list pointer past the prologue and dummy block for future allocations. */
    heap_listp += (6 * WSIZE);

    /* Extend the heap with a free block of size CHUNKSIZE. */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    /* If everything is successful, return 0 indicating successful initialization. */
    return 0;
}

/*
* extend_heap - extend heap CHUNKSIZE bytes, Initialize free block and the epilogue header
*/
static void *extend_heap(size_t words) {
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

    /* Add the new free block to the free list */
    add_to_free_list(bp);

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/*
* mm_check - debug tool
*/
int mm_check(void) {
    char *bp = heap_listp;

    /* Check all blocks in the heap */
    while (GET_SIZE(HDRP(bp)) != 0) {
        if (!is_valid_heap_address(bp)) {
            printf("Line %d: Invalid heap address found at %p\n", __LINE__, bp);
            return 0;
        }

        if (GET_ALLOC(HDRP(bp)) && GET_ALLOC(NEXT_BLKP(bp))) {
            if (FTRP(bp) > HDRP(NEXT_BLKP(bp))) {
                printf("Line %d: Overlapping allocated blocks found.\n", __LINE__);
                return 0;
            }
        }

        bp = NEXT_BLKP(bp);
    }

    /* Check all blocks in the free list */
    for (bp = GET_SUCC(dummy_blockp); bp != NULL; bp = GET_SUCC(bp)) {
        if (GET_ALLOC(bp)) {
            printf("Line %d: Allocated block found in free list at %p\n", __LINE__, bp);
            return 0;
        }

        if (!is_valid_heap_address(bp) || GET_ALLOC(bp)) {
            printf("Line %d: Invalid or allocated block in free list at %p\n", __LINE__, bp);
            return 0;
        }

        if (!GET_ALLOC(bp) && !GET_ALLOC(PREV_BLKP(bp))) {
            printf("Line %d: Contiguous free blocks that escaped coalescing found.\n", __LINE__);
            return 0;
        }
    }

    /* Check if every free block is actually in the free list */
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(bp) && !is_in_free_list(bp)) {
            printf("Line %d: Free block not in free list found at %p\n", __LINE__, bp);
            return 0;
        }
    }

    return 1;
}



/* 
 * add_to_free_list - give a free block pointer bp, add this block to the LIFO queue
 */
static void add_to_free_list(char *bp) {
    /* Set the successor of the new block to be the current successor of the dummy block */
    SET_SUCC(bp, GET_SUCC(dummy_blockp));

    /* Set the predecessor of the new block to be the dummy block */
    SET_PRED(bp, dummy_blockp);

    /* If the dummy block's successor is not NULL, update its predecessor to the new block */
    if (GET_SUCC(dummy_blockp) != NULL) {
        SET_PRED(GET_SUCC(dummy_blockp), bp);
    }

    /* Update the dummy block's successor to the new block */
    SET_SUCC(dummy_blockp, bp);
}

/* 
 * is_in_free_list - give a free block pointer bp, check whether it is in the free list
 */
static int is_in_free_list(char *bp) {
    char *current_bp = GET_SUCC(dummy_blockp); // Start from the first block in the free list

    // Traverse the free list
    while (current_bp != NULL) {
        if (current_bp == bp) {
            return 1; // Found the block in the free list
        }
        current_bp = GET_SUCC(current_bp); // Move to the next block in the free list
    }

    return 0; // Block not found in the free list
}

/*
 * remove_from_free_list - given a free block pointer bp, remove it from the free block list
 */
static void remove_from_free_list(char* bp) {
    // get predecessor block pointer
    char* pred_bp = GET_PRED(bp);

    // get successor block pointer
    char* succ_bp = GET_SUCC(bp);

    // update the successor of the predecessor block
    SET_SUCC(pred_bp, succ_bp);

    // update the predecessor of the successor block
    SET_PRED(succ_bp, pred_bp);
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    if (bp == 0) 
        return;

    size_t size = GET_SIZE(HDRP(bp));

    if (heap_listp == 0){
        mm_init();
    }

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(char *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    /* Case 1: No coalescing */
    if (prev_alloc && next_alloc) {
        add_to_free_list(bp);
        return bp;
    }

    /* Case 2: Coalesce with next block */
    if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_from_free_list(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    /* Case 3: Coalesce with previous block */
    else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        bp = PREV_BLKP(bp);
        remove_from_free_list(bp);
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(bp), PACK(size, 0));
    }

    /* Case 4: Coalesce with both previous and next blocks */
    else {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
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
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
