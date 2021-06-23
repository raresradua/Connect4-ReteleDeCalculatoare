# Connect4 - Proiect Retele de calculatoare

Implementarea jocului [Connect Four][connect4] in limbajul C, intr-o aplicatie client/server, folosind un server TCP concurent (multithreaded)

## Compilare

```c
Server
gcc sv.c -o server

Pornire server
./server

Client
gcc cl.c -o client

./client 127.0.0.1 2908
(trebuie conectati minim 2 jucatori)
```

## Descriere joc

Primul jucator conectat va alege ce culoare va dori, R(osu) sau G(alben) si va alege daca va dori sa inceapa primul sau al doilea, apoi jocul va incepe
Fiecare jucator va alege pe ce coloana va dori sa puna piesa (va alege un numar de la 1 la 7)
Primul jucator care va reusi sa aiba 4 piese de culoarea sa una langa alta pe linie, coloana sau diagonala, va castiga. Daca niciun jucator nu resueste sa aiba 4 piese de aceeasi culoare una langa alta va fi remiza.
In caz de castig sau remiza, jucatorii mai au posibilitatea de a juca, dar trebuie ca fiecare sa fie de acord sa mai joace o partida.



[//]: # 
   [connect4]: <https://en.wikipedia.org/wiki/Connect_Four>
