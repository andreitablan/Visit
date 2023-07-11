#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>

#include <unistd.h>

#include <stdio.h>

#include <stdlib.h>

#include <netdb.h>

#include <string.h>

#include <arpa/inet.h>




//cod de eroare
extern int errno;



//portul de conectare la server
int port;


int main (int argc, char *argv[])

{

  int sd;			// descriptorul de socket

  struct sockaddr_in server;	// structura folosita la conectare

  char mesaj[1000];		// mesajul trimis


  // verificarea existenti tuturor argumentelor

  if (argc != 3)

    {

      printf ("Sintaxa corecta: %s <adresa_server> <port>\n", argv[0]);

      return -1;

    }



  /* stabilim portul */

  port = atoi (argv[2]);



  /* cream socketul */

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)

    {

      perror ("Eroare la socket().\n");

      return errno;

    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */

  /* familia socket-ului */

  server.sin_family = AF_INET;

  /* adresa IP a serverului */

  server.sin_addr.s_addr = inet_addr(argv[1]); // inet_addr("127.0.0.1");

  /* portul de conectare */

  server.sin_port = htons (port);

  

  /* ne conectam la server */

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)

    {

      perror ("[client]Eroare la connect().\n");

      return errno;

    }

  

  /* citirea mesajului */

  printf (" Bine ati venit! \n Comenzile disponibile sunt *start* sau *exit*.\n");

  

     while(1)

     {

          bzero (mesaj, sizeof(mesaj));

          fflush (stdout);

          read (0, mesaj, sizeof(mesaj));

                

          /* trimiterea mesajului la server */

          if (write (sd, mesaj, sizeof(mesaj)) <= 0)

            {

              perror ("[client]Eroare la write() spre server.\n");

              return errno;

            }

          

          if (strncmp(mesaj,"exit",4)==0)

                {return 0; exit(0);}

          

          /* citirea raspunsului dat de server 
             (apel blocant pana cand serverul raspunde) */

             

          if (read (sd, mesaj, sizeof(mesaj)) < 0)

            {

              perror ("[client]Eroare la read() de la server.\n");

              return errno;

            }

          /* afisam mesajul primit */

          printf ("%s\n", mesaj);

    }

  /* inchidem conexiunea */

  close (sd);

}
