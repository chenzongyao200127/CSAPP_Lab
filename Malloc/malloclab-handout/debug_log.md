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




~~~shell
    391      // Check if the header and footer of the block match.
 →  392      if (GET(HDRP(bp)) != GET(FTRP(bp)))
    393          printf("Line %d Error: header does not match footer\n", __LINE__);
    394  }
    395  
    396  
    397  /*
────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "mdriver", stopped 0x56557d2f in checkblock (), reason: SIGSEGV
──────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x56557d2f → checkblock(bp=0xf69c4028)
[#1] 0x56557e62 → checkheap(verbose=0x0)
[#2] 0x5655810e → mm_check(verbose=0x0)
[#3] 0x5655810e → mm_malloc(size=0x7f8)
[#4] 0x56556cb0 → eval_mm_valid(ranges=0xffffcd50, tracenum=<optimized out>, trace=0x5655e600)
[#5] 0x56556cb0 → main(argc=<optimized out>, argv=<optimized out>)
───────────────────────────────────────────────────────────────────────────────────────
gef➤  p *(int *)0xf69c4030
$15 = 0xf69c4020
gef➤  p *(int *)0xf69c4034
$16 = 0xf69c4020
gef➤  p *(int *)0xf69c5028
$17 = 0x1000
gef➤  p bp
$18 = (void *) 0xf69c4028
gef➤  
~~~
`bp` 的位置有问题