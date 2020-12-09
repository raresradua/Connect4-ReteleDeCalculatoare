#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int jucator1;
  int jucator2;
}thData;

typedef struct jucator{
  int id;
  int culoare;
  int scor;
}jucator;

char msg[100];

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {
      int primul_jucator;
      int al_doilea_jucator;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (primul_jucator = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
      if ( (al_doilea_jucator = accept (sd, (struct sockaddr *) &from, &length)) < 0)
  {
    perror ("[server]Eroare la accept().\n");
    continue;
  }      
	

	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->jucator1=primul_jucator;
    td->jucator2=al_doilea_jucator;

	pthread_create(&th[i], NULL, &treat, td);	      
				
	}//while    
}				
static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
}


void raspunde(void *arg)
{
  int nr, i=0;
  struct thData tdL; 
  tdL= *((struct thData*)arg);
  int castig = 0;
  int restart = 0;
  jucator P1;
  jucator P2;

  char tabla[6][7];
  for(int i = 0; i < 6; i++){
      for(int j = 0; j < 7; j++)
            tabla[i][j] = '*';
  }
  char boardstr[50];
  strcpy(boardstr,"******************************************1234567");

  bzero(msg,100);
  if(write(tdL.jucator1,"Alege ce culoare vrei sa fii (Rosu/Galben)", 100) <= 0)
            {
                perror("[server] Eroare la write() catre primul jucator.\n");
                
            }
  if(write(tdL.jucator2, "Esti al doilea", 100) <= 0){
                perror("[server] Eroare la write() catre al doilea jucator.\n");
                
            }
  if(read(tdL.jucator1, msg, 100) <= 0){
                perror("[server]Eroare la read() de la primul jucator.\n");
                close(tdL.jucator1);
                
            }

  if(strcmp(msg, "r") == 0 || strcmp(msg, "R") == 0 || strcmp(msg, "Rosu") == 0 || strcmp(msg, "rosu") == 0){
                
                P1.culoare = 'R';
                //al doilea write() catre client
                if(write(tdL.jucator1,"Ati primit culoarea ROSU.", 100) <= 0){
                    perror("[server] Eroare la write() catre primul jucator.\n");
                    
                }

                if(write(tdL.jucator2,"Ati primit culoarea GALBEN.", 100) <= 0){
                    perror("[server] Eroare la write() catre al doilea jucator.\n");
                    
                }

                P2.culoare = 'G';
            }
  else if(strcmp(msg, "g") == 0 || strcmp(msg, "G") == 0 || strcmp(msg, "Galben") == 0 || strcmp(msg, "galben") == 0){
                P1.culoare = 'G';
                //ramura sau al doiela write() catre client
                if(write(tdL.jucator1,"Ati primit culoarea GALBEN.", 100) <= 0){
                    perror("[server] Eroare la write() catre primul jucator.\n");
                   
                }

                if(write(tdL.jucator2,"Ati primit culoarea ROSU.", 100) <= 0){
                    perror("[server] Eroare la write() catre al doilea jucator.\n");
                   
                }

                P2.culoare = 'R';
            } 

if(write(tdL.jucator1, "Vrei sa incepi primul?(DA/NU)", 100) <= 0)
            {
                perror("[server] Eroare la write() catre primul jucator conectat. \n");
               
            }

if(write(tdL.jucator2,"Asteapta ca primul sa aleaga daca incepe primul sau nu.", 100) <= 0){
                perror("[server] Eroare la write() catre al doilea jucator conectat. \n");
                
            }

if(read(tdL.jucator1, msg, 100) <= 0){
                perror("[server] Eroare la read() de la primul jucator conectat.\n");
                close(tdL.jucator1);
                
            }            
            
            if(strcmp(msg,"DA") == 0){
                P1.id = tdL.jucator1;
                P2.id = tdL.jucator2;
                //al patrulea write() catre client
                write(P1.id, "Vei incepe primul", 100);
                write(P2.id, "Vei incepe al doilea", 100);
            }
            else if(strcmp(msg,"NU") == 0){
                P1.id = tdL.jucator2;
                P2.id = tdL.jucator1;
                //ramura else al patrulea write() catre client
                write(P1.id, "Vei incepe primul", 100);
                write(P2.id, "Vei incepe al doilea", 100);
            }

while(1){
    bzero(msg,100);
    //trimit tabla si aleg coloana
    strcpy(msg, boardstr);
    //al cincilea write()
    write(P1.id, msg, 100); //trimit tabla amandurora
    write(P2.id, msg, 100);
    //al saselea write()
    write(P1.id, "Alege", 100);
    write(P2.id, "Asteapta", 100);

    bzero(msg,100);

    //al 3 lea read()
    read(P1.id, msg, 100);
    int coloana = msg[0] - '0';
    int i = 5;
    while(i >= 0){
        if(tabla[i][coloana] == '*')
        {    
            tabla[i][coloana] = P1.culoare;
            break;
        }
        i--;
    }

    char aux[49]="";
    for(int i = 0; i<6; i++){
      for(int j = 0; j<7; j++){
        strncat(aux,&tabla[i][j],1);
      }
    }
    strcat(aux,"1234567");
    strcpy(boardstr,aux);
    strcpy(msg, boardstr);

    
    write(P2.id, msg, 100);
    bzero(msg,100);
    read(P2.id, msg, 100);
    coloana = msg[0] - '0';
    i = 5;
    while(i >= 0){
        if(tabla[i][coloana] == '*')
        {    
            tabla[i][coloana] = P2.culoare;
            break;
        }
        i--;
    }

    strcpy(aux,"");
    for(int i = 0; i<6; i++){
      for(int j = 0; j<7; j++){
        strncat(aux,&tabla[i][j],1);
      }
    }
    strcat(aux,"1234567");
    strcpy(boardstr,aux);
    strcpy(msg, boardstr);
    break;


    //prelucreaza matricea cu ce a trimis P1

  } 


}
