#include "types.h"
#include "stat.h"
#include "user.h"

// Classic Banker's Algorithm textbook scenario:
// 5 processes (P0..P4), 3 resource types (A, B, C).
#define NPROC 5
#define NRES  3

static int allocation[NPROC][NRES] = {
  {0, 1, 0},
  {2, 0, 0},
  {3, 0, 2},
  {2, 1, 1},
  {0, 0, 2},
};

static int max[NPROC][NRES] = {
  {7, 5, 3},
  {3, 2, 2},
  {9, 0, 2},
  {2, 2, 2},
  {4, 3, 3},
};

static int available[NRES] = {3, 3, 2};

static int need[NPROC][NRES];

static void
compute_need(void)
{
  int i, j;
  for(i = 0; i < NPROC; i++)
    for(j = 0; j < NRES; j++)
      need[i][j] = max[i][j] - allocation[i][j];
}

// Runs the safety algorithm against the given allocation/available snapshot.
// If safe, fills seq[] with a valid safe sequence and returns 1.
// If unsafe, returns 0.
static int
is_safe(int alloc[NPROC][NRES], int avail[NRES], int seq[NPROC])
{
  int work[NRES];
  int finish[NPROC];
  int i, j, count;

  for(j = 0; j < NRES; j++)
    work[j] = avail[j];
  for(i = 0; i < NPROC; i++)
    finish[i] = 0;

  count = 0;
  while(count < NPROC){
    int found = 0;
    for(i = 0; i < NPROC; i++){
      if(finish[i])
        continue;
      int ok = 1;
      for(j = 0; j < NRES; j++){
        if(need[i][j] > work[j]){
          ok = 0;
          break;
        }
      }
      if(ok){
        for(j = 0; j < NRES; j++)
          work[j] += alloc[i][j];
        finish[i] = 1;
        seq[count++] = i;
        found = 1;
      }
    }
    if(!found)
      return 0;
  }
  return 1;
}

static void
print_state(char *label)
{
  int i, j;
  printf(1, "%s\n", label);
  printf(1, "Proc  Allocation      Max             Need            \n");
  for(i = 0; i < NPROC; i++){
    printf(1, "P%d    ", i);
    for(j = 0; j < NRES; j++)
      printf(1, "%d ", allocation[i][j]);
    printf(1, "      ");
    for(j = 0; j < NRES; j++)
      printf(1, "%d ", max[i][j]);
    printf(1, "      ");
    for(j = 0; j < NRES; j++)
      printf(1, "%d ", need[i][j]);
    printf(1, "\n");
  }
  printf(1, "Available: %d %d %d\n\n", available[0], available[1], available[2]);
}

// Attempts to grant Request[pid], following the standard Resource-Request
// Algorithm: bounds check against Need and Available, tentatively grant,
// re-run the safety algorithm, then commit or roll back.
static void
request_resources(int pid, int req[NRES])
{
  int i, j;

  printf(1, "Request from P%d: %d %d %d\n", pid, req[0], req[1], req[2]);

  for(j = 0; j < NRES; j++){
    if(req[j] > need[pid][j]){
      printf(1, "Request denied -- exceeds Need[%d]\n\n", pid);
      return;
    }
  }
  for(j = 0; j < NRES; j++){
    if(req[j] > available[j]){
      printf(1, "Request denied -- exceeds Available\n\n");
      return;
    }
  }

  // Tentatively grant on a scratch copy.
  int trial_alloc[NPROC][NRES];
  int trial_avail[NRES];
  int seq[NPROC];

  for(i = 0; i < NPROC; i++)
    for(j = 0; j < NRES; j++)
      trial_alloc[i][j] = allocation[i][j];
  for(j = 0; j < NRES; j++)
    trial_avail[j] = available[j];

  for(j = 0; j < NRES; j++){
    trial_alloc[pid][j] += req[j];
    trial_avail[j] -= req[j];
  }
  need[pid][0] -= req[0];
  need[pid][1] -= req[1];
  need[pid][2] -= req[2];

  if(is_safe(trial_alloc, trial_avail, seq)){
    for(i = 0; i < NPROC; i++)
      for(j = 0; j < NRES; j++)
        allocation[i][j] = trial_alloc[i][j];
    for(j = 0; j < NRES; j++)
      available[j] = trial_avail[j];
    printf(1, "Request granted -- system remains safe\n");
    printf(1, "Safe sequence: ");
    for(i = 0; i < NPROC; i++)
      printf(1, "P%d ", seq[i]);
    printf(1, "\n\n");
  } else {
    // Roll back -- undo the tentative Need adjustment too.
    need[pid][0] += req[0];
    need[pid][1] += req[1];
    need[pid][2] += req[2];
    printf(1, "Request denied -- would lead to unsafe state\n\n");
  }
}

int
main(void)
{
  int seq[NPROC];
  int i;

  compute_need();
  print_state("Initial state:");

  if(is_safe(allocation, available, seq)){
    printf(1, "System is in a SAFE state.\n");
    printf(1, "Safe sequence: ");
    for(i = 0; i < NPROC; i++)
      printf(1, "P%d ", seq[i]);
    printf(1, "\n\n");
  } else {
    printf(1, "System is in an UNSAFE state.\n\n");
  }

  // Scenario 1: classic textbook request from P1, (1,0,2) -- must be granted.
  int req1[NRES] = {1, 0, 2};
  request_resources(1, req1);
  print_state("State after P1 request:");

  // Scenario 2: P0 requests (0,2,0). This passes the Need and Available
  // bounds checks, but tentatively granting it leaves no process able to
  // finish -- the safety algorithm correctly rejects it.
  int req2[NRES] = {0, 2, 0};
  request_resources(0, req2);
  print_state("State after P0 request:");

  printf(1, "BANKERS DONE\n");
  exit();
}
