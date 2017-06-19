#include <sys/ipc.h>
#include <sys/shm.h>
#include "commons.h"
int pelletdone=0;
void  Eathandler(int sig)
{
     signal(SIGABRT, SIG_IGN);
     pelletdone=1;
     fp=fopen(filename,"a+");
     fprintf(fp,"\nEaten Pallet=[%d]\n",getpid());
     fclose(fp);
     exit(0);    
}
void Stophandler(int sig)
{
     signal(SIGQUIT, SIG_IGN);
     fp=fopen(filename,"a+");
     fprintf(fp,"\nPellet Interrupted=[%d]\n",getpid()); 
     fclose(fp);
     pelletdone=1;   
     exit(0);   
}
int getRand(int min_num, int max_num)
{
    int result=0,low_num=0,hi_num=0;
    if(min_num<max_num)
    {
        low_num=min_num;
        hi_num=max_num+1; 
    }else{
        low_num=max_num+1;
        hi_num=min_num;
    }
    srand(time('\0'));
    result = (rand()%(hi_num-low_num))+low_num;
    return result;
}
main(int argc,char *argv[])
{
	pellet pob;
	int shmid;
	key_t key;
	seashore *shm,*s;
	key=SHM_KEY;	
	pob.x=getRand(1,NOR-2);
	pob.y=getRand(1,NOC);	
	pob.status=0;
	pob.pid=getpid();
	printf("\nNew Pellet=[%d] Dropped @ Location=(%d,%d)\n",getpid(),pob.x,pob.y);
	fp=fopen(filename,"a+");
	fprintf(fp,"\nNew Pellet=[%d] Dropped @ Location=(%d,%d)\n",getpid(),pob.x,pob.y); 
	if((shmid=shmget(key,sizeof(seashore),0666))<0)
	{
		perror("shmget");
		exit(1);
	}
	if((shm=(seashore*)shmat(shmid,NULL,0))==(seashore*)-1)
	{
		perror("shmat");
		exit(1);
	}	
	pob.present=1;
	shm->loc[pob.x][pob.y]=pob;
	signal(SIGABRT,Eathandler);
	signal(SIGQUIT,Stophandler);
	while(shm==NULL)
		sleep(1);	
	while(!pelletdone)
	{
		sleep(2);			
		if(shm->loc[pob.x][pob.y].status=1)
		{
			pelletdone=1;
			break;
		}		
		shm->loc[pob.x][pob.y].present=0;
		shm->loc[pob.x][pob.y].pid=0;		
		pob.x++;
		if(pob.x==NOR)
		{
			pelletdone=1;
			break;
		}
		shm->loc[pob.x][pob.y].present=1;
		shm->loc[pob.x][pob.y].status=0;
		shm->loc[pob.x][pob.y].pid=pob.pid;
	}
	if(pob.x==NOR)
	{
		fp=fopen(filename,"a+");
		fprintf(fp,"\nProcess Over for Pallet=[%d]\n",getpid());	
		fclose(fp);
	}
	pob.x--;
	shm->loc[pob.x][pob.y].present=0;
	shm->loc[pob.x][pob.y].status=0;
	shm->loc[pob.x][pob.y].pid=0;
	
}



