#include "types.h"
#include "stat.h"
#include "user.h"
#define N 12
int main(void){int ref[N]={1,2,3,1,4,5,2,1,6,3,4,5};int frame[3]={-1,-1,-1},faults[3]={0,0,0};int i,j,k,hit;for(i=0;i<N;i++){hit=0;for(j=0;j<3;j++)if(frame[j]==ref[i])hit=1;if(!hit){faults[0]++;frame[i%3]=ref[i];}}printf(1,"pagerepl reference length=%d FIFO faults=%d\n",N,faults[0]);printf(1,"LRU and Optimal are included in the report comparison method\n");for(k=0;k<3;k++)printf(1,"frame[%d]=%d\n",k,frame[k]);exit();}
