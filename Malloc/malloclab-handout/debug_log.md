~~~shell
────────────────────────────────────────────────────────────────────────────────────────────────────────────────── source:mm.c+163 ────
    158          SET_SUCC(dummy_blockp, old_dummy_succ);
    159      }
    160  
    161      /* Initialize free block header/footer and the epilogue header */
    162      PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
 →  163      PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    164      PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */
    165  
    166      /* Coalesce if the previous block was free */
    167      return coalesce(bp);
    168  }
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "mdriver", stopped 0x56557cdf in extend_heap (), reason: BREAKPOINT
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x56557cdf → extend_heap(words=0x400)
[#1] 0x56557df2 → mm_init()
[#2] 0x56556881 → eval_mm_valid(ranges=0xffffd050, tracenum=0x0, trace=0x5655e600)
[#3] 0x56556881 → main(argc=<optimized out>, argv=<optimized out>)
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  p *(int *)0xf69c4024
$2 = 0x1000
gef➤  
~~~
dummy node 后继节点地址 在extend_heap后被修改