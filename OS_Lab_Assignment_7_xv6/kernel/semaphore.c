#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define NSEMS 10
struct semaphore { int value; int initialized; struct spinlock lock; };
static struct semaphore sems[NSEMS];
static struct spinlock sem_table_lock;

void seminit(void) {
  initlock(&sem_table_lock, "sem_table");
  for(int i=0;i<NSEMS;i++) { initlock(&sems[i].lock, "semaphore"); sems[i].value=0; sems[i].initialized=0; }
}
int ksem_init(int id, int value) {
  if(id<0 || id>=NSEMS || value<0) return -1;
  acquire(&sem_table_lock); acquire(&sems[id].lock);
  sems[id].value=value; sems[id].initialized=1;
  release(&sems[id].lock); release(&sem_table_lock); return 0;
}
int ksem_wait(int id) {
  if(id<0 || id>=NSEMS) return -1;
  struct semaphore *s=&sems[id]; acquire(&s->lock);
  if(!s->initialized) { release(&s->lock); return -1; }
  for(;;) {
    if(s->value>0) { s->value--; release(&s->lock); return 0; }
    sleep_prepare(&s->value); release(&s->lock); sleep(); acquire(&s->lock);
    if(!s->initialized) { release(&s->lock); return -1; }
  }
}
int ksem_signal(int id) {
  if(id<0 || id>=NSEMS) return -1;
  struct semaphore *s=&sems[id]; acquire(&s->lock);
  if(!s->initialized) { release(&s->lock); return -1; }
  s->value++; wakeup(&s->value); release(&s->lock); return 0;
}
