#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_va2pa(void){ int va; if(argint(0,&va)<0)return -1; return va2pa(myproc()->pgdir,(uint)va); }
int sys_getmeminfo(void){
  char *u; struct meminfo mi; uint total,free; int d,i;
  if(argptr(0,&u,sizeof(mi))<0)return -1; getmemcounts(&total,&free);
  mi.total_pages=total; mi.free_pages=free; mi.user_pages=0; mi.pagetable_pages=0;
  for(d=0;d<NPDENTRIES;d++) if(myproc()->pgdir[d]&PTE_P){
    if(PGADDR(d,0,0)>=KERNBASE)continue; mi.pagetable_pages++;
    pte_t *pt=(pte_t*)P2V(PTE_ADDR(myproc()->pgdir[d]));
    for(i=0;i<NPTENTRIES;i++)if((pt[i]&PTE_P)&&(pt[i]&PTE_U))mi.user_pages++;
  }
  memmove(u,&mi,sizeof(mi)); return 0;
}
int sys_vmprint(void){ vmprint(myproc()->pgdir); return 0; }
int sys_fork(void){return fork();}
int sys_exit(void){exit();return 0;}
int sys_wait(void){return wait();}
int sys_kill(void){int pid;if(argint(0,&pid)<0)return -1;return kill(pid);}
int sys_getpid(void){return myproc()->pid;}
int sys_sbrk(void){int addr,n;if(argint(0,&n)<0)return -1;addr=myproc()->sz;if(n>0){if(addr+n>=KERNBASE)return -1;myproc()->sz+=n;}else if(growproc(n)<0)return -1;return addr;}
int sys_sleep(void){int n;uint ticks0;if(argint(0,&n)<0)return -1;acquire(&tickslock);ticks0=ticks;while(ticks-ticks0<n){if(myproc()->killed){release(&tickslock);return -1;}sleep(&ticks,&tickslock);}release(&tickslock);return 0;}
int sys_uptime(void){uint t;acquire(&tickslock);t=ticks;release(&tickslock);return t;}
