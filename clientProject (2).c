#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

struct Dane{
	char login [6];
	char haslo [50];
	int t_or_n;
}dane;
static struct Dane proba[9];

struct buffer{
	long mtype;
	int dokogo;
	char kto[6];
	char text[200];
}client;

struct Fun_zalog{
	long mtype;
	char nick[6];
}client_log;

struct Test{
	long mtype;
	int dokogo;
	int tescik[9];
}test;

struct Grupa{
	long mtype;
	int id;
	int gr;
}grupa;

struct bufferr{
	long mtype;
	int dokogo;
	char kto[6];
	char text[100];
}clientt;

void czyt_plik(struct Dane proba[]){
	FILE *fp = fopen("login_haslo.txt","r"); 
	if(fp == NULL) // sprawdza czy plik sie otworzyl
		printf("Nie czyta pliku\n");
	for(int i=0; i<10;i++){
		fscanf(fp,"%s %s %d", proba[i].login, proba[i].haslo, &proba[i].t_or_n);
	}
}


int main(){
	
	czyt_plik(proba);
	int msqid, len, gru, b = 1, decyzja;
	long moje_id;
	char nick[6];
	
	while(b){
		printf("Login:\n");
		scanf("%s", nick);
		strncpy(dane.login, nick,sizeof(dane.login));
		strncpy(client.kto, nick,sizeof(dane.login));
		strncpy(clientt.kto, nick,sizeof(dane.login));
		strncpy(client_log.nick, nick,sizeof(dane.login));
		for(int k=0; k<10;k++){
			if(strcmp(proba[k].login, dane.login)==0){
				printf("Wpisz haslo:\n");
				scanf("%s", dane.haslo);
				if(strcmp(proba[k].haslo, dane.haslo)==0){
					printf("Udalo sie zalogowac\n");
					moje_id = k+2; // moje id 
					b = 0;
					break;
				}
				else{
					printf("Bledne haslo\n");
				}
			}
			else{
				continue;

			}
		}
	}
	
	key_t key1 = 3500;
	
	if((msqid = msgget(key1,0666))==-1){
		printf("Server nie uruchiomiony\n");
		exit(1);
	}
	
	
	printf("Wybierz grupe:\n [1]-grupa1\n [2]-grupa2\n [3]-grupa3\n");
	scanf("%d", &gru);
	
	grupa.mtype = 5;
	grupa.id = moje_id-1;
	grupa.gr = gru;
	msgsnd(msqid, &grupa, sizeof(grupa)-sizeof(grupa.mtype),0);
	
	
	client_log.mtype = 2;
	msgsnd(msqid, &client_log, sizeof(client_log.nick),0);
	
	
	test.dokogo = moje_id;
	test.mtype = 4;

	client.mtype = 1;
	
	clientt.mtype = 6;
	clientt.dokogo = gru;
	
	printf("Klient gotowy!\n");
	while(1){
		printf("Wybierz polecenie:\n [1] - Wyslij wiadomosc do uzytkownika\n [2] - Wyslij wiadomosc do grupy\n [3] - Odbierz wiadomosci\n [4] - Odbierz od grupy\n [5] - Podglad listy zalogowanych uzytkownikow\n [7] - Log out\n");
		scanf("%i", &decyzja);
		if (decyzja == 1) // wysylanie +
		{
			printf("Do kogo wyslac wiadomosc? - podaj id uzytkownika\n");
				scanf("%d\n", &client.dokogo);
				client.dokogo = client.dokogo+1;
			if(fgets(client.text,sizeof(client.text),stdin) != NULL){
				len = strlen(client.text);
				if(client.text[len-1] =='\n')
				client.text[len-1] = '\0';
				msgsnd(msqid, &client,sizeof(client.text)+sizeof(client.dokogo)+sizeof(client.kto),0);
			}
		}
		if(decyzja == 2){ //wysylanie do grup 
			getchar();
			if(fgets(clientt.text,sizeof(clientt),stdin) != NULL){
				len = strlen(clientt.text);
				if(clientt.text[len-1] =='\n'){
					clientt.text[len-1] = '\0';
				}
				msgsnd(msqid, &clientt,sizeof(clientt)-sizeof(clientt.mtype),0);
			}
			
		}
		if(decyzja == 3){ // odbieranie 
			while(msgrcv(msqid, &client, sizeof(client),moje_id, IPC_NOWAIT) != -1){
				printf("%s: %s \n",client.kto, client.text);
				client.mtype = 1;
			}
		}
		if(decyzja == 4){ // odbieranie 
			while(msgrcv(msqid, &clientt, sizeof(clientt),moje_id, IPC_NOWAIT) != -1){
				printf("%s: %s \n",clientt.kto, clientt.text);
				clientt.mtype = 6;
			}
		}
		if(decyzja == 5){ // kto zalogowany proba 
			struct ping{long mtype; int me;}acc;
			acc.me = moje_id;
			acc.mtype = 4;
			msgsnd(msqid, &acc, sizeof(acc)-sizeof(acc.mtype),0); 
			msgrcv(msqid, &test, sizeof(test.tescik)+sizeof(test.dokogo),moje_id, 0);
			for(int i = 0; i <9; i++){
				if(test.tescik[i] == 1){
					printf("%s\n", proba[i].login);
				}	
			}	
		}
		
		if(decyzja == 7){ //logout +
			client_log.mtype = 3;
			msgsnd(msqid, &client_log,sizeof(client_log.nick),0);
			msgctl(msqid, IPC_RMID,0);
			exit(0);
		}
	}
	return 0;
}
  

  


