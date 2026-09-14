#include "types.h"
#include "stat.h"
#include "user.h"

// Deadlock detection via the wait-for graph derived from a
// Resource-Allocation-Graph-style allocation/request model.
#define NPROC 5
#define NRES  3

// allocation[i][r] = 1 if resource r is currently held by process i.
// request[i][r]    = 1 if process i is waiting for resource r.
// Each resource type here has exactly one instance, so "held by" is
// single-valued and building the wait-for edge P_i -> P_j (i waits on a
// resource that j holds) is direct.
static int allocation[NPROC][NRES];
static int request[NPROC][NRES];
static int waitfor[NPROC][NPROC]; // waitfor[i][j] = 1 if P_i -> P_j

static void
clear_all(void)
{
  int i, j;
  for(i = 0; i < NPROC; i++){
    for(j = 0; j < NRES; j++){
      allocation[i][j] = 0;
      request[i][j] = 0;
    }
    for(j = 0; j < NPROC; j++)
      waitfor[i][j] = 0;
  }
}

static void
build_waitfor_graph(void)
{
  int i, j, r;
  for(i = 0; i < NPROC; i++){
    for(r = 0; r < NRES; r++){
      if(!request[i][r])
        continue;
      for(j = 0; j < NPROC; j++){
        if(j != i && allocation[j][r]){
          waitfor[i][j] = 1;
        }
      }
    }
  }
}

static void
print_graph(void)
{
  int i, j;
  printf(1, "Wait-for graph edges:\n");
  for(i = 0; i < NPROC; i++)
    for(j = 0; j < NPROC; j++)
      if(waitfor[i][j])
        printf(1, "  P%d -> P%d\n", i, j);
  printf(1, "\n");
}

static int visited[NPROC];
static int instack[NPROC];
static int path[NPROC];
static int pathlen;

// DFS-based cycle detection. Returns 1 and fills path[]/pathlen with the
// cycle if one is found starting reachable from node u.
static int
dfs(int u)
{
  int v;
  visited[u] = 1;
  instack[u] = 1;
  path[pathlen++] = u;

  for(v = 0; v < NPROC; v++){
    if(!waitfor[u][v])
      continue;
    if(instack[v]){
      // Found a back-edge -- rotate path[] so it starts at v.
      int start = 0, i;
      for(i = 0; i < pathlen; i++){
        if(path[i] == v){
          start = i;
          break;
        }
      }
      int newlen = 0;
      int tmp[NPROC + 1];
      for(i = start; i < pathlen; i++)
        tmp[newlen++] = path[i];
      tmp[newlen++] = v;
      for(i = 0; i < newlen; i++)
        path[i] = tmp[i];
      pathlen = newlen;
      return 1;
    }
    if(!visited[v]){
      if(dfs(v))
        return 1;
    }
  }

  instack[u] = 0;
  pathlen--;
  return 0;
}

// Returns 1 if a cycle exists anywhere in the wait-for graph; prints it.
static int
detect_deadlock(void)
{
  int i;
  for(i = 0; i < NPROC; i++){
    visited[i] = 0;
    instack[i] = 0;
  }

  for(i = 0; i < NPROC; i++){
    if(visited[i])
      continue;
    pathlen = 0;
    if(dfs(i)){
      int j;
      printf(1, "DEADLOCK DETECTED -- cycle: ");
      for(j = 0; j < pathlen; j++){
        printf(1, "P%d", path[j]);
        if(j != pathlen - 1)
          printf(1, " -> ");
      }
      printf(1, "\n\n");
      return 1;
    }
  }
  printf(1, "No deadlock -- wait-for graph is acyclic.\n\n");
  return 0;
}

static void
run_scenario(char *label)
{
  printf(1, "== %s ==\n", label);
  build_waitfor_graph();
  print_graph();
  detect_deadlock();
}

int
main(void)
{
  // Scenario 1: no deadlock. P0 holds R0, P1 holds R1 and waits for R0.
  // P2 holds R2. No cycle exists.
  clear_all();
  allocation[0][0] = 1;            // P0 holds R0
  allocation[1][1] = 1;            // P1 holds R1
  allocation[2][2] = 1;            // P2 holds R2
  request[1][0] = 1;               // P1 waits for R0 (held by P0)
  request[3][1] = 1;               // P3 waits for R1 (held by P1)
  run_scenario("Scenario 1: acyclic wait-for graph");

  // Scenario 2: deadlock among 3 processes in a circular wait,
  // plus an extra process waiting on the cycle (not part of it).
  // P0 holds R0 and wants R1 (held by P1).
  // P1 holds R1 and wants R2 (held by P2).
  // P2 holds R2 and wants R0 (held by P0)  -> cycle P0->P1->P2->P0.
  // P3 holds nothing and waits on R0 (held by P0), so it is blocked by
  // the deadlock without being part of the cycle itself.
  clear_all();
  allocation[0][0] = 1;
  allocation[1][1] = 1;
  allocation[2][2] = 1;
  request[0][1] = 1;   // P0 -> P1
  request[1][2] = 1;   // P1 -> P2
  request[2][0] = 1;   // P2 -> P0  (closes the cycle)
  request[3][0] = 1;   // P3 -> P0  (blocked, but not in the cycle)
  run_scenario("Scenario 2: 3-process circular wait");

  printf(1, "DEADLOCKDETECT DONE\n");
  exit();
}
