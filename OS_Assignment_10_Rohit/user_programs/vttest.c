// Assignment 10, Q2 - vttest.c
// User program that exercises the vtop() system call: it passes several
// virtual addresses to vtop() and prints the physical address each one
// translates to, including one invalid / unmapped address (result -1).
#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int global_var = 42;        // lives in the data segment

static void
show(const char *what, int va)
{
  int pa = vtop(va);
  if(pa == -1)
    printf(1, "  %s: va 0x%x -> NOT MAPPED (vtop returned -1)\n", what, va);
  else
    printf(1, "  %s: va 0x%x -> pa 0x%x  (frame %d, offset 0x%x)\n",
           what, va, pa, pa >> 12, pa & 0xFFF);
}

int
main(void)
{
  char *heap;
  int local_var = 7;

  printf(1, "vttest: virtual -> physical address translation\n");

  heap = sbrk(2 * PGSIZE);          // grow heap by 2 pages
  if(heap == (char*)-1){
    printf(1, "vttest: sbrk failed\n");
    exit();
  }
  memset(heap, 1, 2 * PGSIZE);      // touch the pages

  show("heap page (start)", (int)heap);
  show("heap page + 100", (int)heap + 100);
  show("heap second page", (int)heap + PGSIZE);
  show("global variable", (int)&global_var);
  show("main() code", (int)main);
  show("stack (local variable)", (int)&local_var);
  show("invalid/unmapped address", 0x10000000);
  show("invalid/unmapped address", 0x7FFFFFFF);

  exit();
}
