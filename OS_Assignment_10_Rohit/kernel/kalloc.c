// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

// ===== Assignment 10, Q1: physical memory allocation tracking =====
// Counters maintained by kalloc() and kfree().
//   total_allocated_pages : pages handed out by kalloc() since boot-time
//                           initialisation finished (see mem_counting).
//   total_freed_pages     : pages returned through kfree() in the same period.
//   free_pages            : pages currently sitting on the free list
//                           (counted from the very beginning, so it always
//                           reflects the true number of available pages).
// The boot-time freerange() calls in kinit1()/kinit2() dump every physical
// page onto the free list; counting those as "frees" would drown the real
// runtime statistics, so alloc/free *event* counters are only enabled
// (mem_counting = 1) at the end of kinit2().  free_pages is always exact.
static uint total_allocated_pages = 0;
static uint total_freed_pages = 0;
static uint free_pages = 0;
static int mem_counting = 0;
// ===== end Q1 additions (globals) =====


// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
  mem_counting = 1;   // Q1: start alloc/free event counting from here on
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  // Q1: one more page is back on the free list.
  free_pages++;
  if(mem_counting)
    total_freed_pages++;
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    // Q1: one page leaves the free list and becomes allocated.
    free_pages--;
    if(mem_counting)
      total_allocated_pages++;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

// ===== Assignment 10, Q1: kernel side of the memstats() system call =====
// Prints the memory statistics gathered above.  "Currently allocated" is
// derived as (allocated - freed) over the counted period; "available" is
// the exact current length of the free list.
void
memstats(void)
{
  uint allocated, freed, current, avail;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  allocated = total_allocated_pages;
  freed = total_freed_pages;
  avail = free_pages;
  if(kmem.use_lock)
    release(&kmem.lock);
  current = allocated - freed;

  cprintf("\n===== Physical Memory Statistics =====\n");
  cprintf("Total allocated pages       : %d\n", allocated);
  cprintf("Total freed pages           : %d\n", freed);
  cprintf("Currently allocated pages   : %d\n", current);
  cprintf("Available (free) pages      : %d\n", avail);
  cprintf("Available physical memory   : %d KB\n", avail * PGSIZE / 1024);
  cprintf("Total physical memory pages : %d (PHYSTOP = %d MB)\n",
          PHYSTOP / PGSIZE, PHYSTOP / 1024 / 1024);
  cprintf("======================================\n");
}
// ===== end Q1 additions =====


