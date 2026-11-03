#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// ===== Assignment 10: new system call implementations =====

// Q1: memstats() - display physical memory statistics.
// The counters live in kalloc.c; memstats() there does the printing.
int
sys_memstats(void)
{
  memstats();
  return 0;
}

// Q2: vtop(va) - translate a virtual address of the calling process into
// its physical address.  Uses walkpgdir() (through getpte()) to find the
// PTE, checks the Present bit, then combines the physical frame address
// with the page offset.  Returns the physical address, or -1 when the
// page is not mapped / not present.
int
sys_vtop(void)
{
  int va;
  pte_t *pte;
  uint pa;

  if(argint(0, &va) < 0)
    return -1;
  if(va < 0)
    return -1;
  pte = getpte(myproc()->pgdir, (uint)va);
  if(pte == 0 || (*pte & PTE_P) == 0)
    return -1;                       // unmapped or not present
  pa = PTE_ADDR(*pte) + ((uint)va & 0xFFF);   // frame base + offset
  return (int)pa;
}

// Q3a: pageinfo(va) - report whether va is mapped, its physical frame
// number and its PTE flag bits (P = present, W = writable, U = user).
// Returns 0 if the page is mapped and present, -1 otherwise.
int
sys_pageinfo(void)
{
  int va;
  pte_t *pte;

  if(argint(0, &va) < 0)
    return -1;
  if(va < 0)
    return -1;
  pte = getpte(myproc()->pgdir, (uint)va);
  if(pte == 0 || (*pte & PTE_P) == 0){
    cprintf("pageinfo: va 0x%x is NOT mapped (no page present)\n", va);
    return -1;
  }
  cprintf("pageinfo: va 0x%x -> physical frame number %d (frame base 0x%x)\n",
          va, PTE_ADDR(*pte) >> 12, PTE_ADDR(*pte));
  cprintf("          flags: P(Present)=%d  W(Writable)=%d  U(User)=%d\n",
          (*pte & PTE_P) ? 1 : 0,
          (*pte & PTE_W) ? 1 : 0,
          (*pte & PTE_U) ? 1 : 0);
  return 0;
}

// Q3b: setreadonly(va) - remove write permission from the page that
// contains va.  The real work (PTE edit + TLB flush) is in vm.c.
int
sys_setreadonly(void)
{
  int va;

  if(argint(0, &va) < 0)
    return -1;
  if(va < 0)
    return -1;
  return setreadonly_vm(myproc()->pgdir, (uint)va);
}
// ===== end Assignment 10 system calls =====
