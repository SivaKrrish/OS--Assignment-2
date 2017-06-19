#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#define NOR 10
#define NOC 10
#define SHM_KEY 1313
#define MAX 20

typedef struct 
{
	int idx;
	pid_t pid;
	int x,y,status,present;	
}pellet;
typedef struct 
{
	int idx;
	pid_t pid;
	int x,y,status;	
}fish;
typedef struct 
{
	pellet loc[NOR][NOC];
}seashore;

FILE *fp;
char filename[]="output.txt";

