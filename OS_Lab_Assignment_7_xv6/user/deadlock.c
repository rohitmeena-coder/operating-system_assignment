#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define N 5
#define M 3
int alloc[N][M], req[N][M], avail[M], finish[N];
int leq(int r[],int a[]){for(int j=0;j<M;j++)if(r[j]>a[j])return 0;return 1;}
int detect(void){int work[M],done=0;for(int j=0;j<M;j++)work[j]=avail[j];for(int i=0;i<N;i++)finish[i]=0;for(;;){int changed=0;for(int i=0;i<N;i++)if(!finish[i]&&leq(req[i],work)){finish[i]=1;done++;changed=1;for(int j=0;j<M;j++)work[j]+=alloc[i][j];}if(!changed)break;}return done;}
void show(char *name){printf("%s: Available=[%d %d %d] finished=",name,avail[0],avail[1],avail[2]);int d=detect();for(int i=0;i<N;i++)if(!finish[i])printf(" P%d",i);printf(" (%d/%d)\n",d,N);}
int main(void){
  for(int i=0;i<N;i++)for(int j=0;j<M;j++){alloc[i][j]=0;req[i][j]=0;} avail[0]=3;avail[1]=3;avail[2]=2; for(int i=0;i<N;i++){req[i][0]=1;req[i][1]=1;req[i][2]=1;} show("safe matrix");
  avail[0]=0;avail[1]=0;avail[2]=0;alloc[0][0]=1;alloc[1][1]=1;req[0][1]=1;req[1][0]=1;show("deadlocked matrix");
  avail[0]=1;avail[1]=0;avail[2]=0;alloc[0][1]=1;req[0][0]=1;req[1][1]=1;show("unsafe-after-request matrix");
  printf("pipe circular-wait model: A holds pipe1 and requests pipe2; B holds pipe2 and requests pipe1 => deadlock detected\n"); exit(0);
}
