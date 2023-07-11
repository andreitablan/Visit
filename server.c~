#include <sys/types.h>

#include <sys/socket.h>

#include <sys/time.h>

#include <stdlib.h>

#include <netinet/in.h>

#include <unistd.h>

#include <errno.h>

#include <stdio.h>

#include <arpa/inet.h>

#include <pthread.h>

#include <sqlite3.h>

#include <string.h>

#include <sys/wait.h>



/* portul folosit */



#define PORT 2001



extern int errno;	/* eroarea returnata de unele apeluri */



sqlite3 *db;



struct vcard

{       int user[20];       

        char nume[50];

        char prenume[50];

        char adresa[256];

        char email[50];

        char telefon[20];

        char URL[256];

}newcard;



typedef struct threads

{

  int numar_thread; /*numarul threadului*/

  int client;    /*descriptorul*/

  sqlite3 *db;  /*baza de date*/

} threads;



/* functie de convertire a adresei IP a clientului in sir de caractere */

char * conv_addr (struct sockaddr_in address)

{

  static char str[25];

  char port[7];



  /* adresa IP a clientului */

  strcpy (str, inet_ntoa (address.sin_addr));	

  /* portul utilizat de client */

  bzero (port, 7);

  sprintf (port, ":%d", ntohs (address.sin_port));	

  strcat (str, port);

  return (str);

}

static void *rezolva_firul_de_executie(void *); /* functia care realizeaza comunicarea cu clientii prin intermediul threadurilor */



char arguments[10][256], maxim[256], Nume[256], User[256];



static int callback_maxim(void *data, int argc, char **argv, char **azColName)

{

  bzero(maxim, sizeof(maxim));

  strcat(maxim, argv[0]);

  return 0;

}



static int callback_user(void *data, int argc, char **argv, char **azColName)

{

  bzero(Nume, sizeof(Nume));

  strcat(Nume, argv[0]);

  return 0;

}



static int callback_nume_prenume(void *data, int argc, char **argv, char **azColName)

{

  bzero(User, sizeof(User));

  strcat(User, argv[0]);

  return 0;

}



static int callback(void *nimic, int argc, char **argv, char **azColName) 

{

    nimic = 0;

    

    for (int i = 0; i < argc; i++) 

    {   

        if(argv[i]) strcpy(arguments[i],argv[i]);

        else strcpy(arguments[i],"NULL");

    }

    return 0;

}



int main ()

{



  sqlite3_open("vcard.db", &db);

  struct sockaddr_in server;	/* structurile pentru server si clienti */

  struct sockaddr_in from;

  

  int index = 0;

  int sd, client;		/* descriptori de socket */

  int optval=1; 		/* optiune folosita pentru setsockopt()*/ 

  int fd;			/* descriptor folosit pentru parcurgerea listelor de descriptori */

  pthread_t maximum_threads[100];			

  

  /* creare socket */

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)

    {

      perror ("[server] Eroare la socket().\n");

      return errno;

    }



  /*setam pentru socket optiunea SO_REUSEADDR */ 

  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));



  /* pregatim structurile de date */

  bzero (&server, sizeof (server));

  bzero (&from, sizeof(from));      

  /* umplem structura folosita de server */

  server.sin_family = AF_INET;

    /* acceptam orice adresa */

  server.sin_addr.s_addr = htonl (INADDR_ANY);

    /* utilizam un port utilizator */

  server.sin_port = htons (PORT);



  /* atasam socketul */

  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)

    {

      perror ("[server] Eroare la bind().\n");

      return errno;

    }

    

  /* serverul asteapta coectarea clientilor */

  if (listen (sd, 2) == -1)

    {

      perror ("[server] Eroare la listen().\n");

      return errno;

    }

  

  /* servim in mod concurent clientii */

  while (1)

    { 

      int client;

      threads *fir;

      int length = sizeof(from);      

      

      printf("[server]Asteptam la portul %d...\n", PORT);

      fflush(stdout);



	  /* a venit un client, acceptam conexiunea */

	  client = accept (sd, (struct sockaddr *) &from, &length);



	  /* eroare la acceptarea conexiunii de la un client */

	  if (client < 0)

	    {

	      perror ("[server] Eroare la accept().\n");

	      continue;

	    }



	printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n",client, conv_addr (from));

	fflush (stdout);

	

        fir = (struct threads *)malloc(sizeof(struct threads));

        fir->numar_thread = index++;

        fir->client = client;

        fir->db = db;

        

        pthread_create(&maximum_threads[index], NULL, &rezolva_firul_de_executie, fir);

    

    }/* while */

		

}/* main */			

static void* rezolva_firul_de_executie(void *arg)

{

  struct threads th;

  char buffer[100];



  while(1)

  {

    th = *((struct threads *)arg);

    char mesaj_primit[1000];

    char mesaj_de_trimis[1000];

    char comanda_sql[1000];

    char *err_msg=0;


    printf("[thread]- %d - Asteptam mesajul...\n", th.numar_thread);

    fflush(stdout);

    pthread_detach(pthread_self());

    

    bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

    

    if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

    {

      printf("[Thread %d]\n", th.numar_thread);

      perror("Eroare la read() de la client.\n");

    }

 

    if(strncmp(mesaj_primit, "start", 5) == 0 || strncmp(mesaj_primit, "1", 1) == 0 || strncmp(mesaj_primit, "2", 1) == 0 || strncmp(mesaj_primit, "3", 1) == 0 || strncmp(mesaj_primit, "4", 1) == 0 || strncmp(mesaj_primit, "5", 1) == 0 || strncmp(mesaj_primit, "exit", 4) == 0)

    {   

      /* 0. COMANDA DE START */

    if (strncmp(mesaj_primit, "start", 5) == 0)

    {

       bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

       sprintf(mesaj_de_trimis, " Folositi cifrele pentru a apela urmatoarele comenzi:\n  1 - Cauta cartea de vizita dupa user\n  2 - Cauta cartea dupa nume si prenume\n  3 - Adauga carte de vizita\n  4 - Actualizeaza cartea dupa user\n  5 - Sterge carte dupa user\n  *exit*\n");
        
        printf("[SERVER] S-a apelat functia de meniu\n");

    }

     

      //////////////////////////////////////////////////////////////////////////////////////////////////////

    

      // 1. COMANDA DE CAUTARE A UNEI CARTI DE VIZITA DUPA USER 

     

     else if (strncmp(mesaj_primit, "1", 1) == 0)

     {

       int us=0;

       bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

       bzero(mesaj_primit, sizeof(mesaj_primit));

       

       strcpy(mesaj_de_trimis,"user:");

        

       if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

       {

         printf("[Thread %d] ", th.numar_thread);

         perror("[Thread]Eroare la write() catre client.\n");

       }

       

       if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

              {

                printf("[Thread %d]\n", th.numar_thread);

                perror("Eroare la read() de la client.\n");

              }

       

       us=atoi(mesaj_primit);

       

       printf("[SERVER] S-a cerut cartea cu userul %d.", us);

       

       

       bzero(comanda_sql, sizeof(comanda_sql));

       bzero(buffer, sizeof(buffer));

       sprintf(comanda_sql, "SELECT nume FROM carti where user=%d;", us);

       sqlite3_exec(th.db, comanda_sql, callback_user, buffer, &err_msg);

       

       if(strcmp(Nume,"")!=0)

       {   

       bzero(comanda_sql, sizeof(comanda_sql));

       bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

       

       sprintf(comanda_sql, "SELECT user, nume, prenume, adresa, email, telefon, URL FROM carti where user=%d;", us);

       sqlite3_exec(th.db, comanda_sql, callback, 0, &err_msg);
       
       bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

       

       strcpy(mesaj_de_trimis, "Cartea ceruta este:\n");

       

       strcat(mesaj_de_trimis, "user: ");

       strcat(mesaj_de_trimis, arguments[0]);

       strcat(mesaj_de_trimis, "\n");

       

       strcat(mesaj_de_trimis, "Nume: ");

       strcat(mesaj_de_trimis, arguments[1]);

       strcat(mesaj_de_trimis, "\n");

       

       strcat(mesaj_de_trimis, "Prenume: ");

       strcat(mesaj_de_trimis, arguments[2]);

       strcat(mesaj_de_trimis, "\n");

       

       strcat(mesaj_de_trimis, "Adresa: ");

       strcat(mesaj_de_trimis, arguments[3]);

       strcat(mesaj_de_trimis, "\n");

       

       strcat(mesaj_de_trimis, "Email: ");

       strcat(mesaj_de_trimis, arguments[4]);

       strcat(mesaj_de_trimis, "\n");

       

       strcat(mesaj_de_trimis, "Telefon: ");

       strcat(mesaj_de_trimis, arguments[5]);

       strcat(mesaj_de_trimis, "\n");

       

       strcat(mesaj_de_trimis, "URL: ");

       strcat(mesaj_de_trimis, arguments[6]);

       strcat(mesaj_de_trimis, "\n");

       

       strcat(mesaj_de_trimis, "\nVa rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");

       }

       else  strcpy(mesaj_de_trimis, "\nAti introdus un user inexistent!\nVa rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");

       //sleep(3);

       bzero(mesaj_primit, sizeof(mesaj_primit));

       strcpy(mesaj_primit, "1");

      }

      

      //////////////////////////////////////////////////////////////////////////////////////////////////////



      

      // 2. COMANDA DE CAUTARE A UNEI CARTI DE VIZITA DUPA NUME SI PRENUME 

        

       else if (strncmp(mesaj_primit, "2", 1) == 0)

       {

        char nume[256], prenume [256];

                      

        bzero(nume, sizeof(nume));   

        bzero(prenume, sizeof(prenume));

                      

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));



        strcpy(mesaj_de_trimis,"Nume:");

                      

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

         printf("[Thread %d] ", th.numar_thread);

         perror("[Thread]Eroare la write() catre client.\n");

        }             

                     

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

          printf("[Thread %d]\n", th.numar_thread);

          perror("Eroare la read() de la client.\n");

        }

                      

         strcpy(nume, mesaj_primit);

                      

         bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

         bzero(mesaj_primit, sizeof(mesaj_primit));

                      

         sprintf(mesaj_de_trimis,"Prenume:");

                      

         if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

         {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

         }



         if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

         {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

         }

         

         strcpy(prenume, mesaj_primit);

         printf("[SERVER]S-a cerut cartea care are numele %s si prenumele %s \n", nume, prenume);

         bzero(comanda_sql, sizeof(comanda_sql));

         bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

         

         bzero(comanda_sql, sizeof(comanda_sql));

         sprintf(comanda_sql, "SELECT user FROM carti where nume like '%s' and prenume like '%s';", nume, prenume);

         sqlite3_exec(th.db, comanda_sql, callback_nume_prenume, 0, &err_msg);    

         

         if(strcmp(User,"")!=0)

         {

         sprintf(comanda_sql, "SELECT user, nume, prenume, adresa, email, telefon, URL FROM carti where nume LIKE '%s' and prenume LIKE '%s';", nume, prenume);

         sqlite3_exec(th.db, comanda_sql, callback, 0, &err_msg);    

         

         bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

       

         strcpy(mesaj_de_trimis, "Cartea ceruta este:\n");

       

         strcat(mesaj_de_trimis, "user: ");

         strcat(mesaj_de_trimis, arguments[0]);

         strcat(mesaj_de_trimis, "\n");

       

         strcat(mesaj_de_trimis, "Nume: ");

         strcat(mesaj_de_trimis, arguments[1]);

         strcat(mesaj_de_trimis, "\n");

       

         strcat(mesaj_de_trimis, "Prenume: ");

         strcat(mesaj_de_trimis, arguments[2]);

         strcat(mesaj_de_trimis, "\n");

       

         strcat(mesaj_de_trimis, "Adresa: ");

         strcat(mesaj_de_trimis, arguments[3]);

         strcat(mesaj_de_trimis, "\n");

       

         strcat(mesaj_de_trimis, "Email: ");

         strcat(mesaj_de_trimis, arguments[4]);

         strcat(mesaj_de_trimis, "\n");

       

         strcat(mesaj_de_trimis, "Telefon: ");

         strcat(mesaj_de_trimis, arguments[5]);

         strcat(mesaj_de_trimis, "\n");

       

         strcat(mesaj_de_trimis, "URL: ");

         strcat(mesaj_de_trimis, arguments[6]);

         strcat(mesaj_de_trimis, "\n");

         

         strcat(mesaj_de_trimis, "\nVa rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");

         }

         else {strcpy(mesaj_de_trimis, "\nAti introdus niste date inexistente!\nVa rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");}

         

         //sleep(3);

         bzero(mesaj_primit, sizeof(mesaj_primit));

         strcpy(mesaj_primit, "2");

        }

              

      //////////////////////////////////////////////////////////////////////////////////////////////////////        

              

                      

      // 3. COMANDA DE ADAUGARE A UNEI CARTI DE VIZITA

                 

      else if (strncmp(mesaj_primit, "3", 1) == 0)

      {

        //Numele

        

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        strcpy(mesaj_de_trimis,"Nume:");

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

                              

        bzero(newcard.nume, sizeof(newcard.nume));

        strcpy(newcard.nume, mesaj_primit);

                              

        //Prenumele

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        strcpy(mesaj_de_trimis,"Prenume:");

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

                        

        bzero(newcard.prenume, sizeof(newcard.prenume));

        strcpy(newcard.prenume, mesaj_primit);

                             

        //Adresa

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        strcpy(mesaj_de_trimis,"Adresa:");

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

                    

        bzero(newcard.adresa, sizeof(newcard.adresa));

        strcpy(newcard.adresa, mesaj_primit);

        

        //Email

                              

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        strcpy(mesaj_de_trimis,"Email:");

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

            

        bzero(newcard.email, sizeof(newcard.email));

        strcpy(newcard.email, mesaj_primit);

        

        //Telefon

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        strcpy(mesaj_de_trimis,"Telefon:");

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

            

        bzero(newcard.telefon, sizeof(newcard.telefon));

        strcpy(newcard.telefon, mesaj_primit);

                              

        //URL

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        strcpy(mesaj_de_trimis,"URL:");

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

            

        bzero(newcard.URL, sizeof(newcard.URL));

        strcpy(newcard.URL, mesaj_primit);

                              

        //selectam numarul userului maxim pentru a sti ce user sa introducem

        int user_maxim=0;

        bzero(buffer, sizeof(buffer)); 

        bzero(comanda_sql, sizeof(comanda_sql));

        

        sqlite3_exec(th.db,"SELECT max(user) FROM carti;", callback_maxim, 0, &err_msg);

        

        user_maxim = atoi(maxim)+1;

        printf("[SERVER] userul nou are valoarea %d", user_maxim);

                              

        bzero(comanda_sql, sizeof(comanda_sql)); 

        sprintf(comanda_sql, "INSERT INTO carti VALUES(%d,'%s','%s','%s','%s','%s','%s');",user_maxim, newcard.nume, newcard.prenume, newcard.adresa, newcard.email, newcard.telefon, newcard.URL);

        sqlite3_exec(th.db, comanda_sql, 0, 0, &err_msg);

        

        printf("[server] CARTEA ADAUGATA ESTE: \n user: %d Nume: %s Prenume: %s Adresa: %s Email: %s Telefon: %s URL: %s \n",user_maxim, newcard.nume, newcard.prenume, newcard.adresa, newcard.email, newcard.telefon, newcard.URL);

        

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        

        sprintf(mesaj_de_trimis, "Cartea de vizita a fost adaugata cu succes si are userul = %d", user_maxim);

        strcat(mesaj_de_trimis, "\n");

        strcat(mesaj_de_trimis, "\nVa rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");

        

        bzero(mesaj_primit, sizeof(mesaj_primit));

        strcpy(mesaj_primit,"3");

        }

        

        //////////////////////////////////////////////////////////////////////////////////////////////////////



        /* 4. COMANDA DE ACTUALIZARE A UNEI CARTI DE VIZITA DUPA USER */

                               

        else if (strncmp(mesaj_primit, "4", 1) == 0)

         { 

         

         bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

         bzero(mesaj_primit, sizeof(mesaj_primit));

         

         strcpy(mesaj_de_trimis,"Ce user are cartea pe care doriti sa o actualizati?");

         

         if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

         {

          printf("[Thread %d] ", th.numar_thread);

          perror("[Thread]Eroare la write() catre client.\n");

         }

                                              

         if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

         {

                printf("[Thread %d]\n", th.numar_thread);

                perror("Eroare la read() de la client.\n");

         }

         

        int us=atoi(mesaj_primit);

        printf("[SERVER] S-a cerut cartea cu userul: %d \n", us);

        bzero(comanda_sql, sizeof(comanda_sql));

        bzero(buffer, sizeof(buffer));

        sprintf(comanda_sql, "SELECT nume FROM carti where user=%d;", us);

        sqlite3_exec(th.db, comanda_sql, callback_user, buffer, &err_msg);

       

        if(strcmp(Nume,"")!=0)

       {   

        //Numele

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        bzero(comanda_sql, sizeof(comanda_sql));

        bzero(buffer, sizeof(buffer));

        sprintf(comanda_sql, "SELECT nume FROM carti where user=%d;", us);

        sqlite3_exec(th.db, comanda_sql, callback_user, 0, &err_msg);

        strcpy(buffer, Nume);

        

        sprintf(mesaj_de_trimis,"Nume curent: %s \nNume nou:", buffer);

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

                              

        bzero(newcard.nume, sizeof(newcard.nume));

        strcpy(newcard.nume, mesaj_primit);

                              

        //Prenumele

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        bzero(comanda_sql, sizeof(comanda_sql));

        bzero(buffer, sizeof(buffer));

        sprintf(comanda_sql, "SELECT prenume FROM carti where user=%d;", us);

        sqlite3_exec(th.db, comanda_sql, callback_user, 0, &err_msg);

        strcpy(buffer, Nume);

        

        sprintf(mesaj_de_trimis,"Prenume curent: %s \nPrenume nou:", buffer);

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

                        

        bzero(newcard.prenume, sizeof(newcard.prenume));

        strcpy(newcard.prenume, mesaj_primit);

                             

        //Adresa

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        bzero(comanda_sql, sizeof(comanda_sql));

        bzero(buffer, sizeof(buffer));

        sprintf(comanda_sql, "SELECT adresa FROM carti where user=%d;", us);

        sqlite3_exec(th.db, comanda_sql, callback_user, 0, &err_msg);

        strcpy(buffer, Nume);

        

        sprintf(mesaj_de_trimis,"Adresa curenta: %s \nAdersa noua:", buffer);

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

                    

        bzero(newcard.adresa, sizeof(newcard.adresa));

        strcpy(newcard.adresa, mesaj_primit);

        

        //Email

                              

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        bzero(comanda_sql, sizeof(comanda_sql));

        bzero(buffer, sizeof(buffer));

        sprintf(comanda_sql, "SELECT email FROM carti where user=%d;", us);

        sqlite3_exec(th.db, comanda_sql, callback_user, 0, &err_msg);

        strcpy(buffer, Nume);

        

        sprintf(mesaj_de_trimis,"Email curent: %s \nEmail nou:", buffer);

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

            

        bzero(newcard.email, sizeof(newcard.email));

        strcpy(newcard.email, mesaj_primit);

        

        //Telefon

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        bzero(comanda_sql, sizeof(comanda_sql));

        bzero(buffer, sizeof(buffer));

        sprintf(comanda_sql, "SELECT telefon FROM carti where user=%d;", us);

        sqlite3_exec(th.db, comanda_sql, callback_user, 0, &err_msg);

        strcpy(buffer, Nume);

        

        sprintf(mesaj_de_trimis,"Telefon curent: %s \nTelefon nou:", buffer);

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

            

        bzero(newcard.telefon, sizeof(newcard.telefon));

        strcpy(newcard.telefon, mesaj_primit);

                              

        //URL

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        bzero(mesaj_primit, sizeof(mesaj_primit));

        

        bzero(comanda_sql, sizeof(comanda_sql));

        bzero(buffer, sizeof(buffer));

        sprintf(comanda_sql, "SELECT URL FROM carti where user=%d;", us);

        sqlite3_exec(th.db, comanda_sql, callback_user, 0, &err_msg);

        strcpy(buffer, Nume);

        

        sprintf(mesaj_de_trimis,"URL curent: %s \nURL nou:", buffer);

                              

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

           printf("[Thread %d] ", th.numar_thread);

           perror("[Thread]Eroare la write() catre client.\n");

        }

                      

        if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

        {

           printf("[Thread %d]\n", th.numar_thread);

           perror("Eroare la read() de la client.\n");

        }

            

        bzero(newcard.URL, sizeof(newcard.URL));

        strcpy(newcard.URL, mesaj_primit);

                              

        bzero(comanda_sql, sizeof(comanda_sql)); 

        sprintf(comanda_sql, "UPDATE carti SET nume = '%s', prenume = '%s', adresa = '%s', email = '%s', telefon = '%s',URL = '%s' WHERE user = %d;", newcard.nume, newcard.prenume, newcard.adresa, newcard.email, newcard.telefon, newcard.URL, us);

        sqlite3_exec(th.db, comanda_sql, 0, 0, &err_msg);

        printf("[SERVER] Cartea de vizita cu userul=%d a fost actualizata cu succes\n", us);

        bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

        strcpy(mesaj_de_trimis, "Cartea de vizita a fost actualizata cu succes!\n");

        strcat(mesaj_de_trimis, "\nVa rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");

        

        }

       else  strcpy(mesaj_de_trimis, "\nAti introdus un user inexistent!\n Va rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");

        bzero(mesaj_primit, sizeof(mesaj_primit));

        strcpy(mesaj_primit,"4");

        }

    

        /* 5. COMANDA DE STERGERE A UNEI CARTI DE VIZITA DUPA USER */

                                    

        else if (strncmp(mesaj_primit, "5", 1) == 0)

        {

         bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

         bzero(mesaj_primit, sizeof(mesaj_primit));

         

         strcpy(mesaj_de_trimis,"Ce user are cartea pe care doriti sa o stergeti?");

         if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

         {

          printf("[Thread %d] ", th.numar_thread);

          perror("[Thread]Eroare la write() catre client.\n");

         }

                                              

         if (read(th.client, mesaj_primit, sizeof(mesaj_primit)) <= 0)

         {

                printf("[Thread %d]\n", th.numar_thread);

                perror("Eroare la read() de la client.\n");

         }

                                            

         int us=atoi(mesaj_primit);

         printf("[SERVER] Se va sterge cartea cu userul %d/n", us);

         bzero(comanda_sql, sizeof(comanda_sql));

         bzero(buffer, sizeof(buffer));

         sprintf(comanda_sql, "SELECT nume FROM carti where user=%d;", us);

         sqlite3_exec(th.db, comanda_sql, callback_user, buffer, &err_msg);

        

         if(strcmp(Nume,"")!=0)

         {

         sprintf(comanda_sql, "DELETE FROM carti WHERE user=%d;", us);

         sqlite3_exec(th.db, comanda_sql, 0, 0, &err_msg);

         

         bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

         sprintf(mesaj_de_trimis, "S-a sters cartea cu userul %d \n", us);

         strcat(mesaj_de_trimis, "\nVa rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");

         

         }

          else  strcpy(mesaj_de_trimis, "\nAti introdus un user inexistent!\n Va rugam sa redeschideti meniul *start* sau sa scrieti *exit* pentru a iesi din program!\n");


         bzero(mesaj_primit, sizeof(mesaj_primit));

         strcpy(mesaj_primit,"5");

        }

                                                                                 

         else if (strncmp(mesaj_primit, "exit", 4) == 0)

         {

          close(th.client);

          break;

         }

        

       if (strncmp(mesaj_primit, "exit", 4) != 0)

      {

      

        if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

        {

         printf("[Thread %d] ", th.numar_thread);

         perror("[Thread]Eroare la write() catre client.\n");

        }

        else

          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", th.numar_thread);

        sleep(1);

      }

     }

     else 

       { 

         bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

         strcpy(mesaj_de_trimis, "\nAti introdus o comanda gresita, va rugam incercati din nou *start* sau *exit*!\n");

         if (write(th.client, mesaj_de_trimis, sizeof(mesaj_de_trimis)) <= 0)

         {

          printf("[Thread %d] ", th.numar_thread);

          perror("[Thread]Eroare la write() catre client.\n");

         }

         else

           printf("[Thread %d]Mesajul transmis a fost receptat.\n", th.numar_thread);

       }

      bzero(mesaj_primit, sizeof(mesaj_primit));

      bzero(mesaj_de_trimis, sizeof(mesaj_de_trimis));

  } //while      

        return (NULL);

}
