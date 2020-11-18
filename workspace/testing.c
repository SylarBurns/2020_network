#include<string.h>
#include<stdio.h>
#include<stdlib.h>
int main(){
char *message = "FNAMEexample.bin";
char *file_flag = "FNAME";
char file_name[1024];
int flag_size, fname_size;
if(strncmp(message, file_flag, strlen(file_flag))==0){
	flag_size = strlen(file_flag);
	fname_size = strlen(message)-flag_size;
	strncpy(file_name, message+flag_size, fname_size);
	file_name[fname_size+1]=0;
	printf("flag: %d\nfile name: %s\n",strlen(file_flag),file_name);
}
return 0;
}
