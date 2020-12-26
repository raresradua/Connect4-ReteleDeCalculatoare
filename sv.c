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
  int culoare;
  int scor;
  char decizie[10];
}jucator;

char msg[100];
void createBoard(char board[][7]){
  for(int i = 0; i < 6; i++){
      for(int j = 0; j < 7; j++)
            board[i][j] = '*';
  }
}

void alegeCuloare(jucator* unu, jucator* doi, thData* tdL){
    char msg[100];
    bzero(msg,100);
    if(write(tdL->jucator1,"Alege ce culoare vrei sa fii (Rosu/Galben)", 100) <= 0)
            {
                perror("[server] Eroare la write() catre primul jucator.\n");
                
            }
  if(write(tdL->jucator2, "Esti al doilea", 100) <= 0){
                perror("[server] Eroare la write() catre al doilea jucator.\n");
                
            }
  if(read(tdL->jucator1, msg, 100) <= 0){
                perror("[server]Eroare la read() de la primul jucator.\n");
                close(tdL->jucator1);
                
            }

  if(strcmp(msg, "r") == 0 || strcmp(msg, "R") == 0 || strcmp(msg, "Rosu") == 0 || strcmp(msg, "rosu") == 0){
                
                unu->culoare = 'R';
                //al doilea write() catre client
                if(write(tdL->jucator1,"Ati primit culoarea ROSU.", 100) <= 0){
                    perror("[server] Eroare la write() catre primul jucator.\n");
                    
                }

                if(write(tdL->jucator2,"Ati primit culoarea GALBEN.", 100) <= 0){
                    perror("[server] Eroare la write() catre al doilea jucator.\n");
                    
                }

                doi->culoare = 'G';
            }
  else if(strcmp(msg, "g") == 0 || strcmp(msg, "G") == 0 || strcmp(msg, "Galben") == 0 || strcmp(msg, "galben") == 0){
                unu->culoare = 'G';
                //ramura sau al doiela write() catre client
                if(write(tdL->jucator1,"Ati primit culoarea GALBEN.", 100) <= 0){
                    perror("[server] Eroare la write() catre primul jucator.\n");
                   
                }

                if(write(tdL->jucator2,"Ati primit culoarea ROSU.", 100) <= 0){
                    perror("[server] Eroare la write() catre al doilea jucator.\n");
                   
                }

                doi->culoare = 'R';
            } 


}

void alegeRand(thData* tdL, jucator* unu, jucator* doi){
  char msg[100];
  bzero(msg,100);
  if(write(tdL->jucator1, "Vrei sa incepi primul?(DA/NU)", 100) <= 0)
            {
                perror("[server] Eroare la write() catre primul jucator conectat. \n");
               
            }

  if(write(tdL->jucator2,"Asteapta ca primul sa aleaga daca incepe primul sau nu.", 100) <= 0){
                perror("[server] Eroare la write() catre al doilea jucator conectat. \n");
                
            }

  if(read(tdL->jucator1, msg, 100) <= 0){
                perror("[server] Eroare la read() de la primul jucator conectat.\n");
                close(tdL->jucator1);
                
            }            
            
            if(strcmp(msg,"DA") == 0){
                //al patrulea write() catre client
                write(tdL->jucator1, "Vei incepe primul", 100);
                write(tdL->jucator2, "Vei incepe al doilea, asteapta ca adversarul tau sa mute", 100);
            }
            else if(strcmp(msg,"NU") == 0){
                int aux = tdL->jucator2;
                tdL -> jucator2 = tdL->jucator1;
                tdL->jucator1 = aux;

                int auxCul = unu->culoare;
                unu->culoare = doi->culoare;
                doi->culoare = auxCul;
                //ramura else al patrulea write() catre client
                write(tdL->jucator1, "Vei incepe primul", 100);
                write(tdL->jucator2, "Vei incepe al doilea, asteapta ca adversarul tau sa mute", 100);
            } 
}

int egalitate(char board[][7]){
  for(int i = 0; i < 6; i++)
  {
    for(int j = 0;j < 7; j++)
    {
      if(board[i][j] == '*')
        return 0;
    }
  }
  return 1;
}

int castigator(char board[][7], jucator P){
    int rand = 5;
    int coloana = 0;
    while(coloana < 7){
      while(rand >= 3){
        if(board[rand][coloana] == board[rand - 1][coloana] && 
           board[rand - 1][coloana] == board[rand - 2][coloana] && 
           board[rand - 2][coloana] == board[rand - 3][coloana] && 
           board[rand - 3][coloana] == P.culoare
          )
            return 1;
        rand--;
      }
      rand = 5;
      coloana ++;
    } //verificare pe fiecare coloana daca a castigat

    rand = 5;
    coloana  = 0;
    while(rand >= 0){
      while(coloana <= 3){
        if(board[rand][coloana] == board[rand][coloana + 1] &&
           board[rand][coloana+1] == board[rand][coloana + 2] && 
           board[rand][coloana+2] == board[rand][coloana + 3] &&
           board[rand][coloana+3] == P.culoare
          )
            return 1;
        coloana++;
      }
      rand --;
      coloana = 0;
    } //verificare pe fiecare rand daca a castigat

    rand = 0;
    coloana = 0;
    while(rand <= 2){
      if( 
          (board[rand][0] == board[rand+1][1] && board[rand+1][1] == board[rand+2][2] && board[rand+2][2] == board[rand+3][3] && board[rand+3][3] == P.culoare) ||
          (board[rand][1] == board[rand+1][2] && board[rand+1][2] == board[rand+2][3] && board[rand+2][3] == board[rand+3][4] && board[rand+3][4] == P.culoare) ||
          (board[rand][2] == board[rand+1][3] && board[rand+1][3] == board[rand+2][4] && board[rand+2][4] == board[rand+3][5] && board[rand+3][5] == P.culoare) ||
          (board[rand][3] == board[rand+1][4] && board[rand+1][4] == board[rand+2][5] && board[rand+2][5] == board[rand+3][6] && board[rand+3][6] == P.culoare) ||
          (board[rand][6] == board[rand+1][5] && board[rand+1][5] == board[rand+2][4] && board[rand+2][4] == board[rand+3][3] && board[rand+3][3] == P.culoare) ||
          (board[rand][5] == board[rand+1][4] && board[rand+1][4] == board[rand+2][3] && board[rand+2][3] == board[rand+3][2] && board[rand+3][2] == P.culoare) ||
          (board[rand][4] == board[rand+1][3] && board[rand+1][3] == board[rand+2][2] && board[rand+2][2] == board[rand+3][1] && board[rand+3][1] == P.culoare) ||
          (board[rand][3] == board[rand+1][2] && board[rand+1][2] == board[rand+2][1] && board[rand+2][1] == board[rand+3][0] && board[rand+3][0] == P.culoare)
        )
        return 1;
      rand++;
    } //verificare pe diagonale

    return 0;

}
//static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
//void raspunde(void *);
void raspunde(void *arg)
{
  int nr, i=0;
  struct thData tdL; 
  tdL= *((struct thData*)arg);
  int castig = 0;
  int egal = 0;
  int restart = 0;
  char scor[100] = "";
  char buf[100]="";
  jucator P1;
  jucator P2;
  P1.scor = 0;
  P2.scor = 0;

  char tabla[6][7];
  createBoard(tabla); //cream tabla goala
  char boardstr[50]; //string prin care trimitem tabla catre jucatori
  strcpy(boardstr,"******************************************1234567");

  alegeCuloare(&P1, &P2, &tdL);
  alegeRand(&tdL, &P1, &P2);
  

  while(1){
    while( (castig != 1) && (egal != 1)){
      bzero(msg,100);
      //trimit tabla si aleg coloana
      strcpy(msg, boardstr);
      //al cincilea write()
      write(tdL.jucator1, msg, 100); //trimit tabla amandurora
      write(tdL.jucator2, msg, 100);
      //al saselea write()
      write(tdL.jucator1, "Alege", 100);
      //write(tdL.jucator2, "Asteapta", 100);

      bzero(msg,100);

      //al 3 lea read()
      read(tdL.jucator1, msg, 100);
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

      if(1 == (castig = castigator(tabla,P1))){
        strcat(aux,"1234567"); //
        strcpy(boardstr,aux);
        strcpy(msg, boardstr);
        write(tdL.jucator2, msg, 100);
        write(tdL.jucator1, msg, 100); //

        P1.scor ++;

        strcpy(scor,"");
        sprintf(scor, "Scor: jucator1 %d - %d jucator2", P1.scor, P2.scor);
        write(tdL.jucator1, scor, 100);
        write(tdL.jucator2, scor, 100);

        write(tdL.jucator1, "Ai castigat! Doresti sa mai joci o runda? (DA/NU): ", 100);
        write(tdL.jucator2, "Ai pierdut! Doresti sa mai joci o runda? (DA/NU): ", 100);
        
        read(tdL.jucator1, msg, 100);
        strcpy(P1.decizie, msg);
        read(tdL.jucator2, msg, 100);
        strcpy(P2.decizie, msg);
        if(strcmp(P1.decizie, P2.decizie) == 0 && (strcmp(P1.decizie,"DA") == 0)){
            createBoard(tabla); //cream tabla goala
            strcpy(boardstr,"******************************************1234567");

            int auxId = tdL.jucator1;
            tdL.jucator1 = tdL.jucator2;
            tdL.jucator2 = auxId;

            int auxScor = P1.scor;
            P1.scor = P2.scor;
            P2.scor = auxScor;
            castig = 0;
            write(tdL.jucator1, "Joc nou! Tu vei incepe primul acum! S-au schimbat culorile!",100);
            write(tdL.jucator2, "Joc nou! Tu vei incepe al doilea acum! S-au schimbat culorile!",100);

        }
        else{
          printf("Inchidere conexiune castigator jucator1\n");
          castig = 1;
          write(tdL.jucator1,"Sfarsit1", 100);
          write(tdL.jucator2,"Sfarsit2", 100);
        }
      }else if(1 == (egal = egalitate(tabla))){
            strcat(aux,"1234567"); //
            strcpy(boardstr,aux);
            strcpy(msg, boardstr);
            write(tdL.jucator2, msg, 100);
            write(tdL.jucator1, msg, 100); //

            P1.scor ++;
            P2.scor ++;

            strcpy(scor,"");
            sprintf(scor, "Scor: jucator1 %d - %d jucator2", P1.scor, P2.scor);
            write(tdL.jucator1, scor, 100);
            write(tdL.jucator2, scor, 100);

            write(tdL.jucator1,"Este egalitate! Doresti sa mai joci o runda? (DA/NU): ", 100);
            write(tdL.jucator2,"Este egalitate! Doresti sa mai joci o runda? (DA/NU): ", 100);
           
            read(tdL.jucator1, msg, 100);
            strcpy(P1.decizie, msg);
            read(tdL.jucator2, msg, 100);
            strcpy(P2.decizie, msg);
            if(strcmp(P1.decizie, P2.decizie) == 0 && (strcmp(P1.decizie,"DA") == 0)){
              printf("joc nou eg1\n");
              createBoard(tabla); //cream tabla goala
              strcpy(boardstr,"******************************************1234567");

              int auxId = tdL.jucator1;
              tdL.jucator1 = tdL.jucator2;
              tdL.jucator2 = auxId;

              int auxScor = P1.scor;
              P1.scor = P2.scor;
              P2.scor = auxScor;
              egal = 0;
              write(tdL.jucator1, "Joc nou! Tu vei incepe primul acum! S-au schimbat culorile!",100);
              write(tdL.jucator2, "Joc nou! Tu vei incepe al doilea acum! S-au schimbat culorile!",100);
            }
           else{
              printf("Inchidere conexiune egalitate\n");
              egal = 1;
              write(tdL.jucator1,"Sfarsit1", 100);
              write(tdL.jucator2,"Sfarsit2", 100);

           }
      }else{
            strcat(aux,"1234567");
            strcpy(boardstr,aux);
            strcpy(msg, boardstr);
            write(tdL.jucator2, msg, 100);
            write(tdL.jucator1, msg, 100);
            bzero(msg,100);

            write(tdL.jucator2, "Asteapta", 100);
            read(tdL.jucator2, msg, 100);
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

            if(1 == (castig = castigator(tabla,P2))){
            strcat(aux,"1234567"); //
            strcpy(boardstr,aux);
            strcpy(msg, boardstr);
            write(tdL.jucator2, msg, 100);
            write(tdL.jucator1, msg, 100); //

            P2.scor ++;

            strcpy(scor,"");
            sprintf(scor, "Scor: jucator1 %d - %d jucator2", P1.scor, P2.scor);
            write(tdL.jucator1, scor, 100);
            write(tdL.jucator2, scor, 100);

            write(tdL.jucator2, "Ai castigat! Doresti sa mai joci o runda? (DA/NU): ", 100);
            write(tdL.jucator1, "Ai pierdut! Doresti sa mai joci o runda? (DA/NU): ", 100);
            
            read(tdL.jucator1, msg, 100);
            strcpy(P1.decizie, msg);
            read(tdL.jucator2, msg, 100);
            strcpy(P2.decizie, msg);
            if(strcmp(P1.decizie, P2.decizie) == 0 && (strcmp(P1.decizie,"DA") == 0)){
              printf("joc nou castig 2\n");
              createBoard(tabla); //cream tabla goala
              strcpy(boardstr,"******************************************1234567");

              int auxId = tdL.jucator1;
              tdL.jucator1 = tdL.jucator2;
              tdL.jucator2 = auxId;

              int auxScor = P1.scor;
              P1.scor = P2.scor;
              P2.scor = auxScor;

              castig = 0;
              write(tdL.jucator1, "Joc nou! Tu vei incepe primul acum! S-au schimbat culorile!",100);
              write(tdL.jucator2, "Joc nou! Tu vei incepe al doilea acum! S-au schimbat culorile!",100);
            }
            else{
              printf("Inchidere conexexiune castigatorul rundei este jucator2\n");
              castig = 1;
              write(tdL.jucator1,"Sfarsit1", 100);
              write(tdL.jucator2,"Sfarsit2", 100);
            }
          }else if(1 == (egal = egalitate(tabla))){
              strcat(aux,"1234567"); //
              strcpy(boardstr,aux);
              strcpy(msg, boardstr);
              write(tdL.jucator2, msg, 100);
              write(tdL.jucator1, msg, 100); //

              P1.scor ++;
              P2.scor ++;

              strcpy(scor,"");
              sprintf(scor, "Scor: jucator1 %d - %d jucator2", P1.scor, P2.scor);
              write(tdL.jucator1, scor, 100);
              write(tdL.jucator2, scor, 100);

              write(tdL.jucator1,"Este egalitate! Doresti sa mai joci o runda? (DA/NU): ", 100);
              write(tdL.jucator2,"Este egalitate! Doresti sa mai joci o runda? (DA/NU): ", 100);
              
              read(tdL.jucator1, msg, 100);
              strcpy(P1.decizie, msg);
              read(tdL.jucator2, msg, 100);
              strcpy(P2.decizie, msg);
              if(strcmp(P1.decizie, P2.decizie) == 0 && (strcmp(P1.decizie,"DA") == 0)){
                printf("Joc nou.\n");
                createBoard(tabla); //cream tabla goala
                strcpy(boardstr,"******************************************1234567");

                int auxId = tdL.jucator1;
                tdL.jucator1 = tdL.jucator2;
                tdL.jucator2 = auxId;

                int auxScor = P1.scor;
                P1.scor = P2.scor;
                P2.scor = auxScor;

                egal = 0;
                write(tdL.jucator1, "Joc nou! Tu vei incepe primul acum! S-au schimbat culorile!",100);
                write(tdL.jucator2, "Joc nou! Tu vei incepe al doilea acum! S-au schimbat culorile!",100);
              }
              else{
                printf("Inchidere conexiune egalitate in ultima runda\n");
                egal = 1;
                write(tdL.jucator1,"Sfarsit1", 100);
                write(tdL.jucator2,"Sfarsit2", 100);
              }
         }else{
            strcat(aux,"1234567");
            strcpy(boardstr,aux);
            strcpy(msg, boardstr);
          }
      }
    }
    break;
  } 
}

static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acesti clienti, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
}


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
  if (listen (sd, 1) == -1)
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



