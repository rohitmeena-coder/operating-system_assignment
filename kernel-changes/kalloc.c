// Physical page allocator with free-page accounting.
#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[];
struct run { struct run *next; };
struct { struct spinlock lock; int use_lock; struct run *freelist; uint free_pages; uint total_pages; } kmem;

void kinit1(void *vstart, void *vend) { initlock(&kmem.lock,"kmem"); kmem.use_lock=0; kmem.free_pages=kmem.total_pages=0; freerange(vstart,vend); }
void kinit2(void *vstart, void *vend) { freerange(vstart,vend); kmem.use_lock=1; }
void freerange(void *vstart, void *vend) { char *p=(char*)PGROUNDUP((uint)vstart); for(;p+PGSIZE<=(char*)vend;p+=PGSIZE) kfree(p); }
void kfree(char *v) {
  struct run *r; if((uint)v%PGSIZE||v<end||V2P(v)>=PHYSTOP) panic("kfree");
  memset(v,1,PGSIZE); if(kmem.use_lock) acquire(&kmem.lock); r=(struct run*)v; r->next=kmem.freelist; kmem.freelist=r; kmem.free_pages++; kmem.total_pages++; if(kmem.use_lock) release(&kmem.lock);
}
char *kalloc(void) { struct run *r; if(kmem.use_lock) acquire(&kmem.lock); r=kmem.freelist; if(r){kmem.freelist=r->next; kmem.free_pages--;} if(kmem.use_lock) release(&kmem.lock); return (char*)r; }
void getmemcounts(uint *total,uint *free) { acquire(&kmem.lock); *total=kmem.total_pages; *free=kmem.free_pages; release(&kmem.lock); }
