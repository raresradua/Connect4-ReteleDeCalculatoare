#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>

extern int errno;

void afisareTabla(char * str, int  length){
  int i = 0;
  while(i < length){
    printf("%c %c %c %c %c %c %c\n", str[i], str[i + 1], str[i + 2], str[i + 3], str[i + 4], str[i + 5], str[i + 6]);
    i = i + 7;
  }
  printf("\n");
}

void prelucrareTabla(char *str, int length, char board[][7]){
    int maxRanduri = 6;
    int maxColoane = 7;
    int len = 0;
    int i = 0;
    int j = 0;
    while(len <= length - 7 && i < maxRanduri){
      while(j < maxColoane){
        board[i][j] = str[len];
        j++;
        len++;
      }
      j = 0;
      i++;
    }
}

int main(int argc, char *argv[]){
    int sd;
    struct sockaddr_in server;
    char msgSv[100];
    char tabla[6][7];
    bzero(msgSv,100);
    if (argc != 3){
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

    int port = atoi(argv[2]);

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
      perror ("Eroare la socket().\n");
      return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (port);

    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1){
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

      if(read(sd, msgSv, 100) < 0){
        perror("[client] Eroare la read() de la server.\n");
        return errno;
      }

      if(strcmp(msgSv, "Alege ce culoare vrei sa fii (Rosu/Galben)") == 0){
        printf("Esti primul jucator conectat.\n");
        printf("%s", msgSv);
        printf("Alegere culoare (R - rosu, G - galben): ");
        char alegere[100];
        while(1)
        {
          scanf("%s",alegere);
          if(strcmp(alegere, "Rosu") == 0 || strcmp(alegere, "rosu") == 0 || strcmp(alegere, "R") == 0 || strcmp(alegere,"r") == 0)
               break;
          else if(strcmp(alegere, "Galben\n") == 0 || strcmp(alegere, "galben") == 0 || strcmp(alegere, "G") == 0 || strcmp(alegere,"g") == 0)
              break;
          else printf("Alegere invalida a culorii. Trebuie sa alegeti ROSU sau GALBEN (R/G): ");
        }
        alegere[strlen(alegere)] = '\0';
        write(sd, alegere, 100);
      }
      else{
        printf("Esti al doilea jucator conectat. Asteapta ca primul sa aleaga o culoare.\n");
      }

      if(read(sd, msgSv, 100) < 0){
        perror("[client] Eroare la read() de la server.\n");
        return errno;
      }

      if(strcmp(msgSv, "Ati primit culoarea ROSU.") == 0){
        printf("%s\n", msgSv);
      }
      else{
        printf("Ati primit culoarea GALBEN.\n");
      }

      bzero(msgSv, 100);
      if(read(sd, msgSv, 100) < 0){
        perror("[client] Eroare la read() de la server. \n");
        return errno;
      }

      if(strcmp(msgSv, "Vrei sa incepi primul?(DA/NU)") == 0){
        char raspuns[100];
        while(1){
          printf("Alegeti DA sau NU: ");
          scanf("%s", raspuns);
          if(strcmp(raspuns,"DA\n") != 0 || strcmp(raspuns,"NU\n") != 0){
            break;
          }
          printf("INPUT: %s\n", raspuns);
        }

        raspuns[strlen(raspuns)] = '\0';
        printf("INPUT: %s\n", raspuns);
        write(sd, raspuns, 100);
  
      }
      else{
        printf("%s\n", msgSv);
      }
      if(read(sd, msgSv, 100) < 0){
        perror("[client] Eroare la read() de la server. \n");
        return errno;
      }

      if(strncmp(msgSv, "Vei incepe",10) == 0){
        printf("%s\n", msgSv);
      }


  
    while(1){  
      if(read(sd, msgSv, 100) < 0){
        perror("[client] Eroare la read() de la server.\n");
        return errno;
      }

      if(strcmp(msgSv + 42, "1234567") == 0){
          afisareTabla(msgSv, strlen(msgSv));
          prelucrareTabla(msgSv, strlen(msgSv), tabla); 
      } 
      else if(strcmp(msgSv,"Alege") == 0){
          printf("Randul tau! Alege pe ce coloana vrei sa iti pui piesa (1 - 7): ");
          int coloana = 0;
          while(1){
            while(1){
              scanf("%d", &coloana);
              if(coloana < 1 || coloana > 7)
                printf("Trebuie sa alegeti un numar intre 1 si 7. \n Alege pe ce coloana vrei sa iti pui piesa (1 -7): ");
              else{
                break;
              }
            }
            coloana = coloana - 1;
            int i = 5;
            while(i >= 0){
              if(tabla[i][coloana] == '*')
                break;
              i--;
            }
            if(i == -1){
              printf("Coloana invalida, alegeti alta coloana diferita de coloana %d: ", coloana + 1);
            }
            else{
              bzero(msgSv,100);
              msgSv[0] = coloana + '0';
              printf("%c TOTUL OK\n", msgSv[0]);
              break;
            }
          }
          write(sd, msgSv,100);

          printf("Asteapta pana cand advresarul face o mutare.\n");
      }
      else if(strcmp(msgSv,"Asteapta") == 0){
        printf("Adversarul tau a mutat! Randul tau!\n");
        bzero(msgSv,100);
        afisareTabla(msgSv, strlen(msgSv));
        prelucrareTabla(msgSv, strlen(msgSv), tabla);
        printf("Alege pe ce coloana vrei sa iti pui piesa (1 - 7): ");
          int coloana;
          while(1){
            while(1){
              scanf("%d", &coloana);
              if(coloana < 1 || coloana > 7)
                printf("Trebuie sa alegeti un numar intre 1 si 7. \n Alege pe ce coloana vrei sa iti pui piesa (1 -7): ");
              else{
                break;
              }
            }
            coloana = coloana - 1;
            int i = 5;
            while(i >= 0){
              if(tabla[i][coloana] == '*')
                break;
              i--;
            }
            if(i == -1){
              printf("Coloana invalida, alegeti alta coloana diferita de coloana %d: ", coloana + 1);
            }
            else{
              bzero(msgSv,100);
              msgSv[0] = coloana + '0';
              break;
            }
            

          }
        write(sd, msgSv,100);
        printf("Asteapta pana cand advresarul face o mutare.\n");
      }
      else if(strcmp(msgSv, "Ai castigat! Doresti sa mai joci o runda? (DA/NU): ") == 0){
        printf("%s\n", msgSv);
        char decizie[10];
        while(1){
          scanf("%s", decizie);
          if(strcmp(decizie, "DA") == 0 || strcmp(decizie, "NU") == 0)
            break;
          printf("Alegere gresita! Raspundeti cu DA sau NU: ");
        }
        write(sd, decizie,100);
      }
      else if(strcmp(msgSv, "Ai pierdut! Doresti sa mai joci o runda? (DA/NU): ") == 0){
        printf("%s\n", msgSv);
        char decizie[10];
        while(1){
          scanf("%s", decizie);
          if(strcmp(decizie, "DA") == 0 || strcmp(decizie, "NU") == 0)
            break;
          printf("Alegere gresita! Raspundeti cu DA sau NU: ");
        }
        write(sd, decizie,100);
      }
      else if(strcmp(msgSv, "Este egalitate! Doresti sa mai joci o runda? (DA/NU): ") == 0){
        printf("%s\n", msgSv);
        char decizie[10];
        while(1){
          scanf("%s", decizie);
          if(strcmp(decizie, "DA") == 0 || strcmp(decizie, "NU") == 0)
            break;
          printf("Alegere gresita! Raspundeti cu DA sau NU: ");
        }
        write(sd, decizie,100);
      }
      else if(strncmp(msgSv, "Scor:",5) == 0){
        printf("%s", msgSv);
        printf("\n");
      }
      else if(strncmp(msgSv, "Joc", 3) == 0){
        printf("%s", msgSv);
        printf("\n");
      }
      else if(strcmp(msgSv, "Sfarsit1") == 0){
        printf("Sfarsit de joc! La revedere!\n");
        break;
      }
      else if(strcmp(msgSv, "Sfarsit2") == 0){
        printf("Sfarsit de joc! La revedere!\n");
        break;
      } 
  }

  close(sd); 
} 
