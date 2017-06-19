#include <sys/ipc.h>
#include <sys/shm.h>
#include "commons.h"
int shmid;
int SECONDS,newnop;              
pid_t pids[100],fishpid,i;
int workdone=0,stopcmd=0;

char* Itoa(int value, char* str, int radix) 
{
	static char dig[] ="0123456789abcdefghijklmnopqrstuvwxyz";
	int n = 0, neg = 0;
	unsigned int v;
	char* p, *q;
	char c;
	if (radix == 10 && value < 0) 
	{
		value = -value;
		neg = 1;
	}
	v = value;
	do 
	{
		str[n++] = dig[v%radix];
		v /= radix;
	}while (v);
	if (neg)
		str[n++] = '-';
	str[n] = '\0';
	for (p = str, q = p + (n-1); p < q; ++p, --q)
		c = *p, *p = *q, *q = c;
	return str;
}
void processdir(const struct dirent *dir)
{
     puts(dir->d_name);
}

int filter(const struct dirent *dir)
{
     uid_t user;
     struct stat dirinfo;
     int len = strlen(dir->d_name) + 7; 
     char path[len];

     strcpy(path, "/proc/");
     strcat(path, dir->d_name);
     user = getuid();
     if (stat(path, &dirinfo) < 0) {
	  perror("processdir() ==> stat()");
	  exit(EXIT_FAILURE);
     }
     return !fnmatch("[1-9]*", dir->d_name, 0) && user == dirinfo.st_uid;
}
int exists(pid_t pid)
{     
     struct dirent **namelist;
     int n,found=0;
     n = scandir("/proc", &namelist, filter, 0);
     if (n < 0)
	  perror("Not enough memory.");
     else 
     {
	char str[10];	
	  while(n--) 
	  {
	       if(namelist[n],Itoa(pid,str,10)==0)
	       {
	       	found=1;
	       	free(namelist[n]);
	       	break;
	       }
	       free(namelist[n]);
	  }
	  free(namelist);
     }
     return found;
}
void  ALARMhandler(int sig)
{
     signal(SIGALRM, SIG_IGN);     
     stopcmd=1;
     Stop();
     workdone=1;
}
void  INThandler(int sig)
{
     char  c;
     signal(sig, SIG_IGN); 
     printf("\nSignal Handled\n");
     workdone=1;           
     WaitAndStop(); 
}
int DoWorkInChild(int idx,pid_t cid)
{
	int rval=0;
	printf("\nWorking Child Idx=%d-----Pid=%d\n",idx,cid);
	if(idx==1)
	{
		fishpid=cid;
		if((rval==execl("./fish.o",(char*)0))<0)
		{
			perror("execl fish");
			return -1;
		}
		else
			printf("\nStarted Fish Pid=%d\n",cid);
	}
	else
	{		
		if((rval==execl("./pellet.o",(char*)0))<0)
		{
			perror("execl pellet");
			return -1;
		}
		else
			printf("\nStarted Pellet Pid=%d\n",cid);
	}
	return 0;
}
void clean()
{
	char cmd[100];
	char sid[15];
	strcpy(cmd,"ipcrm -m ");
	strcat(cmd,Itoa(shmid,sid,10));
	printf("Commnad=%s\n",cmd);
	system(cmd);	
	//free(pids);
}
int Stop()
{
	int idx;		
	printf("\nStopping Process\n");
	for(idx=1;idx<i;idx++)
	{
		kill(pids[idx],SIGQUIT);
	}	
	clean();
	printf("\nTimeout. Swim Mill Shutdown\n");	
	kill(fishpid,SIGQUIT);
	exit(0);	
}
int WaitAndStop()
{
	int status,idx=1;
	pid_t pid;		
	printf("\nPlease Wait....\tChild Processes Terminating\n");
	fp=fopen(filename,"a+");
	while (idx<=i) 
	{
	  pid = wait(&status);	  
	  fprintf(fp,"\nChild with PID %ld exited with status 0x%x.\n", (long)pid, status);
	  idx++;
	}	
	fclose(fp);
	clean();
	kill(fishpid,SIGINT);
	printf("\nSwim Mill Shutdown\n");
	exit(0);		
}

main()
{	
	int nop=5;
	key_t key;
	seashore *shm,*s;	
	printf("Enter Duration (in sec):");
	scanf("%d",&SECONDS);	
	key=SHM_KEY;
	printf("\nAlloating Shared Memory\n");
	fp=fopen(filename,"w");
	fclose(fp);
	if((shmid=shmget(key,sizeof(seashore),IPC_CREAT|0666))<0)
	{
		perror("shmget");
		exit(1);
	}
	printf("\nSwim mill attaching Shared Memory\n");
	if((shm=shmat(shmid,NULL,0))==(seashore*)-1)
	{
		perror("shmat");
		exit(1);
	}
	//s=shm;
	//pids=(pid_t*)malloc(sizeof(pid_t)*nop);	
	newnop=nop;
	printf("Initail Process Count=%d\n",nop);
	for(i=1;i<=nop;i++)
	{
		if ((pids[i] = fork()) < 0) 
		{
			perror("fork");
			abort();
		} 
		else if (pids[i] == 0) 
		{
			DoWorkInChild(i,getpid());
			//exit(0);			
		}
	}
	signal(SIGINT, INThandler);  
	sleep(1);
	signal(SIGALRM,ALARMhandler);
	alarm(SECONDS);
	pid_t pid;
	int status;
	while(!workdone)
	{		
		newnop++;
		nop++;
		if(nop>=MAX)
		{
			printf("No of Processes Exceed Max Limit.Waiting....");
			pid = wait(&status);
		        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
			nop--;
		}
		printf("\nSwim Mill Initiating New Process...\n");
		//pids=(pid_t*)realloc(pids,sizeof(pid_t)*newnop);
		if ((pids[i] = fork()) < 0) 
		{
			perror("fork");
			abort();
		} 
		else if (pids[i] == 0) 
		{
			DoWorkInChild(i,getpid());
			//exit(0);			
		}
		i++;					
		sleep(3);
	}
	printf("\nEnd of Swim Mill Process\n");
	//free(pids);
}











