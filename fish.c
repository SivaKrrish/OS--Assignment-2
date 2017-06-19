#include <sys/ipc.h>
#include <sys/shm.h>
#include "commons.h"
seashore *shm,*s;
fish fob;
int fishdone=0;
void  Quithandler(int sig)
{
     signal(SIGQUIT, SIG_IGN);
     fishdone=1;    
     fp=fopen(filename,"a+");
     fprintf(fp,"\nFish Interrupted=[%d]\n",getpid()); 
     fclose(fp);
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
double dist(pellet pob)
{
	return sqrt(((pob.x-fob.x)*(pob.x-fob.x))+((pob.y-fob.y)*(pob.y-fob.y)));
}
void LookupAndCapture()
{
	int i,j,mx,my,cnt;
	double mindist,curdist;
	for(i=0;i<NOR;i++)
	{
		for(j=0;j<NOC;j++)
		{
			if(!shm->loc[i][j].present || shm->loc[i][j].pid==0)
				continue;
			curdist=dist(shm->loc[i][j]);
			if(cnt==0)
			{
				mindist=curdist;
				mx=i;
				my=j;
				cnt++;
			}
			else if(mindist>curdist)
			{
				mindist=curdist;
				mx=i;
				my=j;
			}
		}
	}
	if(cnt>0)
	{
		if(shm->loc[mx][my].pid>0)
		{
			shm->loc[mx][my].status=1;
			shm->loc[mx][my].present=0;				
			kill(shm->loc[mx][my].pid,SIGABRT);
			fob.y=my;
			printf("\nFish Captured Pellet=[%d] @ Location=(%d,%d)\n",shm->loc[mx][my].pid,mx,my);
			shm->loc[mx][my].pid=0;
		}
		else
			fob.y=(fob.y+1)%NOC;
	}
	
}
main(int argc,char *argv[])
{	
	int shmid;
	key_t key;	
	key=SHM_KEY;
	printf("\nFish Entered\n");
	fob.x=NOR-1;
	fob.y=getRand(1,NOC);	
	fob.status=0;
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
	signal(SIGQUIT,Quithandler);
	while(shm==NULL)
		sleep(1);	
	while(!fishdone)
	{
		//fob.y=(fob.y+1)%NOC;
		LookupAndCapture();
		sleep(1);
	}
}



