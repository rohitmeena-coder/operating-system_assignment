#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int va2pa(pde_t *pgdir, uint va) {
  pte_t *pte=walkpgdir(pgdir,(void*)va,0);
  uint flags=pte?PTE_FLAGS(*pte):0;
  cprintf("va2pa: VA=0x%x PDE=%d PTE=%d offset=%d flags(P=%d W=%d U=%d)\n",va,PDX(va),PTX(va),va&(PGSIZE-1),(flags&PTE_P)!=0,(flags&PTE_W)!=0,(flags&PTE_U)!=0);
  if(!pte || !(*pte&PTE_P) || !(*pte&PTE_U)) return -1;
  return PTE_ADDR(*pte)+(va&(PGSIZE-1));
}

void vmprint(pde_t *pgdir) {
  int d,t,kernel=0;
  cprintf("page directory %p\n",pgdir);
  for(d=0;d<NPDENTRIES;d++) if(pgdir[d]&PTE_P){
    uint base=PGADDR(d,0,0); if(base>=KERNBASE){kernel++;continue;}
    pte_t *pt=(pte_t*)P2V(PTE_ADDR(pgdir[d]));
    cprintf("  PDE[%d] table=%p flags(P=%d W=%d U=%d)\n",d,pt,!!(pgdir[d]&PTE_P),!!(pgdir[d]&PTE_W),!!(pgdir[d]&PTE_U));
    for(t=0;t<NPTENTRIES;t++) if(pt[t]&PTE_P){
      uint va=PGADDR(d,t,0); if(va>=KERNBASE)continue;
      cprintf("    PTE[%d] va=0x%x pa=0x%x flags(P=%d W=%d U=%d)\n",t,va,PTE_ADDR(pt[t]),!!(pt[t]&PTE_P),!!(pt[t]&PTE_W),!!(pt[t]&PTE_U));
    }
  }
  cprintf("kernel page-directory entries: %d\n",kernel);
}
