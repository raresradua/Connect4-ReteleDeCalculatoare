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

void welcome()
{
  printf("----------WELCOME TO CONNECT FOUR!----------\n");
  printf("----- Desfasurare joc: \n");
  printf("----- Este nevoie de doi jucatori conectati pentru a se putea incepe partida.\n");
  printf("----- Primul jucator conectat va avea prioritatea de a-si alege culoare si randul\n");
  printf("----- Jucatorii vor avea o tabla de joc de 6x7.\n");
  printf("----- Mutarea unui jucator consta in a alege o coloana pe care isi va pune piesa cu culoarea sa.\n");
  printf("----- Fiecare piesa 'va cadea' pe cea mai de jos pozitie a coloanei alese.\n");
  printf("----- Pentru fiecare jucator, dupa fiecare mutare se va afisa tabla actuala de joc.\n");
  printf("----- Scopul jocului este ca unul din jucatori sa reuseasca sa aiba pe tabla 4 piese de culoarea sa una langa alta pe linie, coloana sau diagonala.\n");
  printf("----- In cazul in care se umple tabla va fi egalitate!\n");
  printf("----- Scorul se va afisa dupa ce un joc se termina cu un castigator sau dupa ce va fi egalitate.\n");
  printf("----- Dupa terminarea unei partide, jucatorii au optinea sa mai joace cel putin inca o data daca vor dori.\n");
  printf("--------------------------------------------\n");
  printf("-----------SPOR LA JOC----------\n");
  printf("...Se asteapta conectarea a doi jucatori!\n\n\n\n");
}
void afisareTabla(char *str, int length)
{
  int i = 0;
  while (i < length)
  {
    printf("%c %c %c %c %c %c %c\n", str[i], str[i + 1], str[i + 2], str[i + 3], str[i + 4], str[i + 5], str[i + 6]);
    i = i + 7;
  }
  printf("\n");
}

void alegeCuloare(int sd, char msgSv[], char alegere[])
{
  int do_nothing = 0;
  if (read(sd, msgSv, 100) < 0)
  {
    perror("[client] Eroare la read() de la server.\n");
    exit(0);
  }

  if (strcmp(msgSv, "Alege ce culoare vrei sa fii (Rosu/Galben)") == 0)
  {
    printf("Esti primul jucator conectat.\n");
    printf("%s", msgSv);
    printf("Alegere culoare (R - rosu, G - galben): ");
    scanf("%s", alegere);
    alegere[strlen(alegere)] = '\0';
    write(sd, alegere, 100);
  }
  else
  {
    printf("Esti al doilea jucator conectat. Asteapta ca primul sa aleaga o culoare.\n");
  }
  while (1)
  {
    if (read(sd, msgSv, 100) < 0)
    {
      perror("[client] Eroare la read() de la server.\n");
      exit(0);
    }

    if (strcmp(msgSv, "Ati primit culoarea ROSU.") == 0)
    {
      printf("%s\n", msgSv);
      break;
    }
    else if (strcmp(msgSv, "Ati primit culoarea GALBEN.") == 0)
    {
      printf("%s\n", msgSv);
      break;
    }
    else if (strcmp(msgSv, "EroareCul") == 0)
    {
      printf("Alegere invalida a culorii. Trebuie sa alegeti ROSU sau GALBEN (R/G): ");
      scanf("%s", alegere);
      alegere[strlen(alegere)] = '\0';
      write(sd, alegere, 100);
    }
    else if (strcmp(msgSv, "EroareCul2") == 0)
    {
      do_nothing = 1;
    }
  }
}

void alegeRand(int sd, char msgSv[], char raspuns[])
{
  int do_nothing;
  if (read(sd, msgSv, 100) < 0)
  {
    perror("[client] Eroare la read() de la server. \n");
    exit(0);
  }

  if (strcmp(msgSv, "Vrei sa incepi primul?(DA/NU)") == 0)
  {

    printf("Vrei sa incepi primul? Alegeti DA sau NU: ");
    scanf("%s", raspuns);
    raspuns[strlen(raspuns)] = '\0';
    write(sd, raspuns, 100);
  }
  else
  {
    printf("%s\n", msgSv);
  }
  while (1)
  {
    if (read(sd, msgSv, 100) < 0)
    {
      perror("[client] Eroare la read() de la server. \n");
      exit(0);
    }

    if (strncmp(msgSv, "Vei incepe", 10) == 0)
    {
      printf("%s\n", msgSv);
      break;
    }
    else if (strcmp(msgSv, "ErrRand") == 0)
    {
      printf("Alegere invalida! Raspundeti cu DA sau NU: ");
      scanf("%s", raspuns);
      raspuns[strlen(raspuns)] = '\0';
      write(sd, raspuns, 100);
    }
    else if (strcmp(msgSv, "ErrRand2") == 0)
    {
      do_nothing = 0;
    }
  }
}
void alegeColoana(int sd, char msgSv[], int coloana)
{
  scanf("%d", &coloana);
  coloana = coloana - 1;
  bzero(msgSv, 100);
  msgSv[0] = coloana + '0';
  write(sd, msgSv, 100);
}

void decizieJuc(int sd, char decizie[])
{
  scanf("%s", decizie);
  write(sd, decizie, 100);
}

int main(int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server;
  char msgSv[100];
  char tabla[6][7];
  char alegere[100] = "";
  char raspuns[100] = "";
  bzero(msgSv, 100);
  if (argc != 3)
  {
    printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }
  int port = atoi(argv[2]);

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Eroare la socket().\n");
    return errno;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons(port);

  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[client]Eroare la connect().\n");
    return errno;
  }

  welcome();

  alegeCuloare(sd, msgSv, alegere);
  bzero(msgSv, 100);
  alegeRand(sd, msgSv, raspuns);

  while (1)
  {
    if (read(sd, msgSv, 100) < 0)
    {
      perror("[client] Eroare la read() de la server.\n");
      return errno;
    }

    if (strcmp(msgSv + 42, "1234567") == 0)
    {
      afisareTabla(msgSv, strlen(msgSv));
    }
    else if (strcmp(msgSv, "Alege") == 0)
    {
      printf("Randul tau! Alege pe ce coloana vrei sa iti pui piesa (1 - 7): ");
      int coloana = 0;
      alegeColoana(sd, msgSv, coloana);
    }
    else if (strcmp(msgSv, "Asteapta") == 0)
    {
      printf("Adversarul tau a mutat! Randul tau!\n");
      bzero(msgSv, 100);
      afisareTabla(msgSv, strlen(msgSv));
      printf("Alege pe ce coloana vrei sa iti pui piesa (1 - 7): ");
      int coloana;
      alegeColoana(sd, msgSv, coloana);
    }
    else if (strcmp(msgSv, "EroareAlegere") == 0)
    {
      int coloana = 0;
      printf("Coloana aleasa nu este valida! Coloana este plina.\n");
      printf("Alege pe ce coloana vrei sa iti pui piesa (1 - 7): ");
      alegeColoana(sd, msgSv, coloana);
    }
    else if (strcmp(msgSv, "WrongNumCol") == 0)
    {
      int coloana = 0;
      printf("Coloana aleasa nu este valida! Nu ai introdus un numar intre 1-7.\n");
      printf("Alege pe ce coloana vrei sa iti pui piesa (1 - 7): ");
      alegeColoana(sd, msgSv, coloana);
    }
    else if (strcmp(msgSv, "Ai castigat! Doresti sa mai joci o runda? (DA/NU): ") == 0)
    {
      printf("%s", msgSv);
      char decizie[100];
      decizieJuc(sd, decizie);
    }
    else if (strcmp(msgSv, "Ai pierdut! Doresti sa mai joci o runda? (DA/NU): ") == 0)
    {
      printf("%s", msgSv);
      char decizie[100];
      decizieJuc(sd, decizie);
    }
    else if (strcmp(msgSv, "Este egalitate! Doresti sa mai joci o runda? (DA/NU): ") == 0)
    {
      printf("%s", msgSv);
      char decizie[100];
      decizieJuc(sd, decizie);
    }
    else if (strcmp(msgSv, "ErrDec") == 0)
    {
      printf("Raspundeti cu DA sau NU! Alegere: ");
      char decizie[100];
      decizieJuc(sd, decizie);
    }
    else if (strncmp(msgSv, "Scor:", 5) == 0)
    {
      printf("%s", msgSv);
      printf("\n");
    }
    else if (strncmp(msgSv, "Joc", 3) == 0)
    {
      printf("%s", msgSv);
      printf("\n");
    }
    else if (strcmp(msgSv, "Sfarsit") == 0)
    {
      printf("Nu au fost ambii jucatori de acord sa joace din nou!\n");
      printf("Sfarsit de joc! La revedere!\n");
      break;
    }
  }

  close(sd);
}
