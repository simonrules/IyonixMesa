/*
 * GLX Hardware Device Driver common code
 * Copyright (C) 1999 Keith Whitwell
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * KEITH WHITWELL, OR ANY OTHER CONTRIBUTORS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 */

/* $Id: mm.c,v 2.1 2002/01/31 10:02:41 theferret Exp $ */

#include <stdlib.h>
#include <stdio.h>

//rudolf: disabled: apparantly not even needed here..
//#include "Xmd.h"
//#include "os.h" /* ErrorF */

/*
 BeOS includes
 */
#include "glh_mm.h"
/*
 * GLX includes.
 */
//#include "hwlog.h"
//#include "glx_symbols.h"

#define ISFREE(bptr) ((bptr)->free)
//#define PRINTF(f,a...) hwMsg(1,f, ## a)

void mmDumpMemInfo( memHeap_t *heap )
{
  TMemBlock *p;

//  PRINTF("Memory heap %p:\n", heap);
  if (heap == 0) {
//    PRINTF("  heap == 0\n");
  } else {
    p = (TMemBlock *)heap;
    while (p) {
//      PRINTF("  Offset:%08x, Size:%08x, %c%c\n",p->ofs,p->size,
//	     p->free ? '.':'U',
//	     p->reserved ? 'R':'.');
      p = p->next;
    }
  }
//  PRINTF("End of memory blocks\n");
}

memHeap_t *mmInit(int ofs,
		  int size)
{
   PMemBlock blocks;
  
   if (size <= 0) {
      return 0;
   }
   blocks = (TMemBlock *) calloc(1,sizeof(TMemBlock));
   if (blocks) {
      blocks->ofs = ofs;
      blocks->size = size;
      blocks->free = 1;
      return (memHeap_t *)blocks;
   } else
      return 0;
}

/* Kludgey workaround for existing i810 server.  Remove soon.
 */
memHeap_t *mmAddRange( memHeap_t *heap,
		       int ofs,
		       int size )
{
   PMemBlock blocks;
   blocks = (TMemBlock *) calloc(2,sizeof(TMemBlock));
   if (blocks) {
      blocks[0].size = size;
      blocks[0].free = 1;
      blocks[0].ofs = ofs;
      blocks[0].next = &blocks[1];

      /* Discontinuity - stops JoinBlock from trying to join non-adjacent
       * ranges.
       */
      blocks[1].size = 0;
      blocks[1].free = 0;
      blocks[1].ofs = ofs+size;
      blocks[1].next = (PMemBlock) heap;      
      return (memHeap_t *)blocks;
   } 
   else
      return heap;
}

static TMemBlock* SliceBlock(TMemBlock *p, 
			     int startofs, int size, 
			     int reserved, int alignment)
{
  TMemBlock *newblock;

  /* break left */
  if (startofs > p->ofs) {
    newblock = (TMemBlock*) calloc(1,sizeof(TMemBlock));
    newblock->ofs = startofs;
    newblock->size = p->size - (startofs - p->ofs);
    newblock->free = 1;
    newblock->next = p->next;
    p->size -= newblock->size;
    p->next = newblock;
    p = newblock;
  }

  /* break right */
  if (size < p->size) {
    newblock = (TMemBlock*) calloc(1,sizeof(TMemBlock));
    newblock->ofs = startofs + size;
    newblock->size = p->size - size;
    newblock->free = 1;
    newblock->next = p->next;
    p->size = size;
    p->next = newblock;
  }

  /* p = middle block */
  p->align = alignment;
  p->free = 0;
  p->reserved = reserved;
  return p;
}

PMemBlock mmAllocMem( memHeap_t *heap, int size, int align2, int startSearch)
{
  int mask,startofs,endofs;
  TMemBlock *p;

  if (!heap || align2 < 0 || size <= 0)
    return NULL;
  mask = (1 << align2)-1;
  startofs = 0;
  p = (TMemBlock *)heap;
  while (p) {
    if (ISFREE(p)) {
      startofs = (p->ofs + mask) & ~mask;
      if ( startofs < startSearch ) {
      	startofs = startSearch;
      }
      endofs = startofs+size;
      if (endofs <= (p->ofs+p->size))
	break;
    }
    p = p->next;
  }
  if (!p)
    return NULL;
  p = SliceBlock(p,startofs,size,0,mask+1);
  p->heap = heap;
  return p;
}

static __inline__ int Join2Blocks(TMemBlock *p)
{
  if (p->free && p->next && p->next->free) {
    TMemBlock *q = p->next;
    p->size += q->size;
    p->next = q->next;
    free(q);
    return 1;
  }
  return 0;
}

int mmFreeMem(PMemBlock b)
{
  TMemBlock *p,*prev;

  if (!b)
    return 0;
  if (!b->heap) {
     fprintf(stderr, "no heap\n");
     return -1;
  }
  p = b->heap;
  prev = NULL;
  while (p && p != b) {
    prev = p;
    p = p->next;
  }
  if (!p || p->free || p->reserved) {
     if (!p)
	fprintf(stderr, "block not found in heap\n");
     else if (p->free)
	fprintf(stderr, "block already free\n");
     else
	fprintf(stderr, "block is reserved\n");
    return -1;
  }
  p->free = 1;
  Join2Blocks(p);
  if (prev)
    Join2Blocks(prev);
  return 0;
}

int mmReserveMem(memHeap_t *heap, int offset,int size)
{
  int endofs;
  TMemBlock *p;

  if (!heap || size <= 0)
    return -1;
  endofs = offset+size;
  p = (TMemBlock *)heap;
  while (p && p->ofs <= offset) {
    if (ISFREE(p) && endofs <= (p->ofs+p->size)) {
      SliceBlock(p,offset,size,1,1);
      return 0;
    }
    p = p->next;
  }
  return -1;
}

int mmFreeReserved(memHeap_t *heap, int offset)
{
  TMemBlock *p,*prev;

  if (!heap)
    return -1;
  p = (TMemBlock *)heap;
  prev = NULL;
  while (p && p->ofs != offset) {
    prev = p;
    p = p->next;
  }
  if (!p || !p->reserved)
    return -1;
  p->free = 1;
  p->reserved = 0;
  Join2Blocks(p);
  if (prev)
    Join2Blocks(prev);
  return 0;
}

void mmDestroy(memHeap_t *heap)
{
  TMemBlock *p,*q;

  if (!heap)
    return;
  p = (TMemBlock *)heap;
  while (p) {
    q = p->next;
    free(p);
    p = q;
  }
}
