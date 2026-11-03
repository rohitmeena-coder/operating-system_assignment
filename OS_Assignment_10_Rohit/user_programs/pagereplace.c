// Assignment 10, Q4 - pagereplace.c
// Page replacement algorithms: FIFO, LRU and Optimal (OPT).
//
// For the test reference string the program first traces every page
// reference with 3 frames, showing the frame contents after each
// reference and whether it was a HIT or a FAULT.  It then compares all
// three algorithms with 3, 4 and 5 frames (faults, hits, hit ratio and
// fault ratio) and states which algorithm performs best.
#include "types.h"
#include "stat.h"
#include "user.h"

// Reference string given in the assignment (20 references).
int refstr[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
#define NREF 20
#define MAXFRAMES 5

#define ALG_FIFO 0
#define ALG_LRU  1
#define ALG_OPT  2

// Print the current frames, e.g. [ 7 0 - ]
static void
printframes(int *frames, int nframes)
{
  int i;
  printf(1, "[");
  for(i = 0; i < nframes; i++){
    if(frames[i] == -1)
      printf(1, " -");
    else
      printf(1, " %d", frames[i]);
  }
  printf(1, " ]");
}

// Simulate one algorithm.  If trace != 0, print every step.
// Returns the number of page faults.
static int
simulate(int alg, int nframes, int trace)
{
  int frames[MAXFRAMES];
  int lastused[MAXFRAMES];   // LRU: time of last use per frame slot
  int loadorder[MAXFRAMES];  // FIFO: time of loading per frame slot
  int faults = 0, time = 0, i, j, page, hit, victim;

  for(i = 0; i < nframes; i++){
    frames[i] = -1;
    lastused[i] = -1;
    loadorder[i] = -1;
  }

  for(i = 0; i < NREF; i++){
    page = refstr[i];
    time++;

    // Is the page already in a frame?
    hit = -1;
    for(j = 0; j < nframes; j++)
      if(frames[j] == page)
        hit = j;

    if(hit >= 0){
      lastused[hit] = time;              // LRU bookkeeping
      if(trace){
        printf(1, "  ref %d : ", page);
        printframes(frames, nframes);
        printf(1, "  HIT\n");
      }
      continue;
    }

    // Page fault: choose a victim frame.
    faults++;
    victim = -1;
    for(j = 0; j < nframes; j++)         // prefer an empty frame
      if(frames[j] == -1){ victim = j; break; }

    if(victim < 0){
      if(alg == ALG_FIFO){
        victim = 0;                      // frame loaded longest ago
        for(j = 1; j < nframes; j++)
          if(loadorder[j] < loadorder[victim])
            victim = j;
      } else if(alg == ALG_LRU){
        victim = 0;                      // frame used least recently
        for(j = 1; j < nframes; j++)
          if(lastused[j] < lastused[victim])
            victim = j;
      } else { // ALG_OPT: page whose next use is farthest in the future
        int farthest = -1;
        for(j = 0; j < nframes; j++){
          int next = -1, k;
          for(k = i + 1; k < NREF; k++)
            if(refstr[k] == frames[j]){ next = k; break; }
          if(next == -1){ victim = j; break; }   // never used again
          if(next > farthest){ farthest = next; victim = j; }
        }
      }
    }

    frames[victim] = page;
    lastused[victim] = time;
    loadorder[victim] = time;

    if(trace){
      printf(1, "  ref %d : ", page);
      printframes(frames, nframes);
      printf(1, "  FAULT\n");
    }
  }
  return faults;
}

// Print a percentage (hits or faults out of NREF) with 2 decimals,
// using integer arithmetic only (xv6 printf has no %f).
static void
printpercent(int part)
{
  int p = part * 10000 / NREF;   // percent * 100
  int frac = p % 100;
  printf(1, "%d.%d%d%%", p / 100, frac / 10, frac % 10);
}

int
main(void)
{
  int i, f, alg;
  int faults[3][3];              // [frames index][algorithm]
  char *names[3];

  names[0] = "FIFO";
  names[1] = "LRU";
  names[2] = "OPTIMAL";

  printf(1, "pagereplace: page replacement algorithms\n");
  printf(1, "Reference string (%d references):", NREF);
  for(i = 0; i < NREF; i++)
    printf(1, " %d", refstr[i]);
  printf(1, "\n");

  // ---- Part (b): full trace with 3 frames ----
  for(alg = 0; alg < 3; alg++){
    printf(1, "\n--- %s with 3 frames ---\n", names[alg]);
    faults[0][alg] = simulate(alg, 3, 1);
    printf(1, "  Total page faults (%s, 3 frames) = %d\n", names[alg], faults[0][alg]);
  }

  // ---- Part (c): comparison with 3, 4 and 5 frames ----
  for(f = 4; f <= 5; f++)
    for(alg = 0; alg < 3; alg++)
      faults[f - 3][alg] = simulate(alg, f, 0);

  printf(1, "\n--- Comparison table ---\n");
  printf(1, "Frames  Algorithm  Faults  Hits  HitRatio       FaultRatio\n");
  for(f = 3; f <= 5; f++){
    for(alg = 0; alg < 3; alg++){
      int fa = faults[f - 3][alg];
      int hi = NREF - fa;
      printf(1, "  %d     %s  %d     %d    ", f,
             alg == 0 ? "FIFO    " : alg == 1 ? "LRU     " : "OPTIMAL ", fa, hi);
      printpercent(hi);
      printf(1, "        ");
      printpercent(fa);
      printf(1, "\n");
    }
  }

  // Conclusion, computed from the results above.
  {
    int best = 0;
    for(alg = 1; alg < 3; alg++)
      if(faults[0][alg] < faults[0][best])
        best = alg;
    printf(1, "\nConclusion: for this reference string %s performs best\n", names[best]);
    printf(1, "(fewest page faults with 3 frames, and never worse than the others\n");
    printf(1, "at 4 and 5 frames, where LRU matches it). OPTIMAL is the theoretical\n");
    printf(1, "minimum - it looks into the future, so it cannot be implemented in a\n");
    printf(1, "real OS; LRU is the best practical algorithm here, and FIFO is the\n");
    printf(1, "worst of the three for this string.\n");
  }

  exit();
}
