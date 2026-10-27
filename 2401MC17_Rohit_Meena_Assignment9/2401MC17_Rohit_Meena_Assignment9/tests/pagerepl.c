#include "types.h"
#include "stat.h"
#include "user.h"

#define MAXREF 64
#define REQUIRED 20

int required[REQUIRED] = {7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1};

int
find(int *frames, int n, int page)
{
  int i;
  for(i = 0; i < n; i++)
    if(frames[i] == page) return i;
  return -1;
}

void
show(int *frames, int n, int page, int hit)
{
  int i;
  printf(1, "%d:", page);
  for(i = 0; i < n; i++)
    printf(1, " %d", frames[i]);
  printf(1, " %s\n", hit ? "hit" : "fault");
}

int
fifo(int *refs, int count, int n, int verbose)
{
  int frames[MAXREF], next = 0, faults = 0, i, slot, hit;
  for(i = 0; i < n; i++) frames[i] = -1;
  for(i = 0; i < count; i++){
    slot = find(frames, n, refs[i]); hit = slot >= 0;
    if(!hit){ frames[next] = refs[i]; next = (next + 1) % n; faults++; }
    if(verbose) show(frames, n, refs[i], hit);
  }
  return faults;
}

int
lru(int *refs, int count, int n, int verbose)
{
  int frames[MAXREF], age[MAXREF], faults = 0, i, j, slot, oldest, hit;
  for(i = 0; i < n; i++){ frames[i] = -1; age[i] = 0; }
  for(i = 0; i < count; i++){
    for(j = 0; j < n; j++) age[j]++;
    slot = find(frames, n, refs[i]); hit = slot >= 0;
    if(!hit){
      slot = 0; oldest = age[0];
      for(j = 0; j < n; j++) if(frames[j] == -1){ slot = j; break; }
      if(frames[slot] != -1)
        for(j = 1; j < n; j++) if(age[j] > oldest){ oldest = age[j]; slot = j; }
      frames[slot] = refs[i]; faults++;
    }
    age[slot] = 0;
    if(verbose) show(frames, n, refs[i], hit);
  }
  return faults;
}

int
main(int argc, char **argv)
{
  int frames = 3, i, f, count = REQUIRED, fifo_faults, lru_faults;
  int refs[MAXREF];
  for(i = 0; i < REQUIRED; i++) refs[i] = required[i];
  if(argc > 1) frames = atoi(argv[1]);
  if(frames < 1 || frames > MAXREF){ printf(2, "usage: pagerepl [frames]\n"); exit(); }
  /* Rohit Meena Assignment 9 Q3: argv[2..] is an optional reference string. */
  if(argc > 2){
    count = argc - 2;
    if(count > MAXREF) count = MAXREF;
    for(i = 0; i < count; i++) refs[i] = atoi(argv[i + 2]);
  }

  printf(1, "ROHIT PAGEREPL reference string, frames=%d, references=%d\n", frames, count);
  fifo_faults = fifo(refs, count, frames, 1);
  lru_faults = lru(refs, count, frames, 1);
  printf(1, "three-frame check: FIFO faults=%d LRU faults=%d\n", fifo_faults, lru_faults);
  printf(1, "frames  FIFO faults/hits  LRU faults/hits\n");
  for(i = 0; i < 3; i++){
    f = i + 3;
    fifo_faults = fifo(refs, count, f, 0);
    lru_faults = lru(refs, count, f, 0);
    printf(1, "  %d          %d/%d             %d/%d\n", f, fifo_faults,
           count - fifo_faults, lru_faults, count - lru_faults);
  }
  if(count == REQUIRED && fifo(refs, count, 3, 0) == 15 && lru(refs, count, 3, 0) == 12)
    printf(1, "ROHIT PAGEREPL PASS\n");
  exit();
}
