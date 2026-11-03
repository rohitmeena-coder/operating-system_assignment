// Assignment 10, Q1 - memtest.c
// User program that exercises the memstats() system call:
//   1. display memory statistics,
//   2. allocate memory with sbrk(),
//   3. display statistics again (allocated pages must have grown),
//   4. release the memory with a negative sbrk(),
//   5. display statistics again (pages must be handed back).
#include "types.h"
#include "stat.h"
#include "user.h"

#define NPAGES 4            // pages allocated in step 2
#define PGSIZE 4096

int
main(void)
{
  char *oldbreak, *newbreak;

  printf(1, "memtest: memory statistics BEFORE allocation\n");
  memstats();

  oldbreak = sbrk(0);
  newbreak = sbrk(NPAGES * PGSIZE);
  if(newbreak == (char*)-1){
    printf(1, "memtest: sbrk failed\n");
    exit();
  }
  printf(1, "\nmemtest: allocated %d pages (%d bytes) with sbrk, break 0x%x -> 0x%x\n",
         NPAGES, NPAGES * PGSIZE, oldbreak, oldbreak + NPAGES * PGSIZE);

  // Touch every new page so it is really in use.
  memset(newbreak, 0xAB, NPAGES * PGSIZE);

  printf(1, "\nmemtest: memory statistics AFTER allocation\n");
  memstats();

  // Release the memory again by shrinking the break back.
  if(sbrk(-NPAGES * PGSIZE) == (char*)-1){
    printf(1, "memtest: sbrk release failed\n");
    exit();
  }
  printf(1, "\nmemtest: released %d pages with sbrk\n", NPAGES);

  printf(1, "\nmemtest: memory statistics AFTER release\n");
  memstats();

  exit();
}
