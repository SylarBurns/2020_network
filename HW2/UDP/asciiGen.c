#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#define FSIZE 1000000
int main(int argc, char *argv[]){
	FILE *fp;
	char arr[FSIZE];
	srand((unsigned)time(NULL));
	int i =0;
	if((fp = fopen(argv[1], "w"))==NULL){
		printf("File open error\n");
		exit(1);
	}

	for(i=0;i<FSIZE;i++){
		if(i%1000==0){
		arr[i]='\n';
		}else{
		arr[i]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand()%26];
		}
	}
	fwrite(arr,sizeof(char),FSIZE,fp);
	fclose(fp);
	return 0;
}