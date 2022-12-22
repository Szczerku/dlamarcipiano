#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h> 
#include <string.h>
#include <errno.h>


struct buffer{
	long mtype;
	int dokogo;
	char kto[6];
	char text[200];
}serwer;

struct Zaloguj{
	int t_or_n;
	char nick[6];
	char haslo[50];	
};

struct Fun_zalog{
	long mtype;
	char nick[6];
}serwer_log;

struct Test{
	long mtype;
	int dokogo;
	int tescik[9];
}test_ser;

struct Grupa{
	long mtype;
	int id;
	int gr;
}serwer_gr;

struct bufferr{
	long mtype;
	int dokogo;
	char kto[6];
	char text[100];
}serwerr;

struct ping{long mtype; int me;}acc;


static struct Zaloguj osoby[9];
void czyt_plik(struct Zaloguj osoby[]){
	FILE *fp = fopen("login_haslo.txt","r"); 
	if(fp == NULL)
		printf("Nie czyta pliku\n");
	for(int i=0; i<10;i++){
		fscanf(fp,"%s %s %d", osoby[i].nick, osoby[i].haslo, &osoby[i].t_or_n);
	}
}

void main(){
	czyt_plik(osoby);
	int msqid;
	int grupa1[9] = {0,0,0,0,0,0,0,0,0};
	int grupa2[9] = {0,0,0,0,0,0,0,0,0};
	int grupa3[9] = {0,0,0,0,0,0,0,0,0};
	key_t key1 = 3500;
	
	if((msqid = msgget(key1, 0666|IPC_CREAT))==-1){
		perror("msgget error clien");
		exit(1);
	}
	printf("serwer gotowy\n");
	while(1){
		if(msgrcv(msqid, &serwer, sizeof(serwer.text)+sizeof(serwer.dokogo)+sizeof(serwer.kto), 1, IPC_NOWAIT) != -1){ // odbior wido od usera
			printf("tekst indywidualny: %s \n", serwer.text);
			serwer.mtype = serwer.dokogo;
			msgsnd(msqid, &serwer,sizeof(struct buffer),0);		
		}
		if(msgrcv(msqid, &serwer_log, sizeof(serwer_log.nick), 2, IPC_NOWAIT) != -1){
			printf("zamienia osoby na zalogowane\n");
			for(int i = 0; i < 9; i++){
				if(strcmp(osoby[i].nick, serwer_log.nick)==0){
					osoby[i].t_or_n = 1; 
				}
			}
		}
		if(msgrcv(msqid, &serwer_log,sizeof(serwer_log.nick), 3, IPC_NOWAIT) != -1){
			printf("zmienia na niezalogowanych\n");
			for(int i = 0; i < 9; i++){
				if(strcmp(osoby[i].nick, serwer_log.nick)==0){
					osoby[i].t_or_n = 0;
				}
			}
		}
		if(msgrcv(msqid, &acc,sizeof(acc)-sizeof(acc.mtype), 4, IPC_NOWAIT) != -1){
			printf("wysylanie tablicy zalogowanych\n");
			for(int j = 0;j<9;j++){
				test_ser.tescik[j] = osoby[j].t_or_n;
			}
			test_ser.mtype = acc.me;
			msgsnd(msqid, &test_ser,sizeof(test_ser)-sizeof(test_ser.mtype),0);
		}
		if(msgrcv(msqid, &serwer_gr,sizeof(serwer_gr)-sizeof(serwer_gr.mtype), 5, IPC_NOWAIT) != -1){
			printf("wpisany do grupy!\n");
			if(serwer_gr.gr == 1){
				grupa1[serwer_gr.id] = 1;
			}
			if(serwer_gr.gr == 2){
				grupa2[serwer_gr.id] = 1;
			}
			if(serwer_gr.gr == 3){
				grupa3[serwer_gr.id] = 1;
			}
		}

		if(msgrcv(msqid, &serwerr, sizeof(serwerr)-sizeof(serwerr.mtype), 6, IPC_NOWAIT) != -1){ // odbior wido od usera i do grup
			printf("przekaz wiado do grup\n");	
			if(serwerr.dokogo == 1){
				for(int g=0; g<9;g++){
					if(grupa1[g] == 1){
						serwerr.mtype = g+1;
						msgsnd(msqid, &serwerr,sizeof(struct bufferr),0);		
					}
				}
			}
			if(serwerr.dokogo == 2){
				for(int g=0; g<9;g++){
					if(grupa1[g] == 1){
						serwerr.mtype = g+1;
						msgsnd(msqid, &serwerr,sizeof(struct bufferr),0);		
					}
				}
			}
			if(serwerr.dokogo == 3){
				for(int g=0; g<9;g++){
					if(grupa1[g] == 1){
						serwerr.mtype = g+1;
						msgsnd(msqid, &serwerr,sizeof(struct bufferr),0);		
					}
				}
			}		
		}
	}
}
		
	
