#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<linux/unistd.h>
#include <semaphore.h>
#include<string.h> //strlen
#include<sys/socket.h> //socket libery
#include<arpa/inet.h> //inet_addr
#include<unistd.h> //write



#define NB_VOITURES 1
#define NB_TOURS 2




pthread_t pthread_id[NB_VOITURES];

int gettid() {
    return syscall(SYS_gettid);
}


/*** initalise le generateur aleatoire.*/
void initialise_generateur_aleatoire()
{
    srandom(time(NULL) + getpid());
}


/*** Retourne une valeur aleatoire entre
* min (inclus) et max (inclus).*/
int valeur_aleatoire(int min, int max)
{
    int val;
    float f = random() * (max-min);
    f=(f / RAND_MAX) +min;
    return (f + 0.5);
}




/*** Procedure faisant effectuer les tours de pistes
* a une voiture.*/
void *threadVoiture(void *nbTour)
{


    int sock;
    struct sockaddr_in server;
    char message[10] , server_reply[10];
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) {
        printf("Could not create socket");
        exit(1);
    }

    puts("Socket created");
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed. Error");
        exit(1);
    }
    puts("Connected\n");




    int i, t, ttrajet=0;
	initialise_generateur_aleatoire();


	printf("Depart de la voiture %d\n",gettid());
	for (i = 1;i <= * ((int *)nbTour);i++)
	{
		/* section OA */
        printf("section OA - %d\n",gettid());
		t = valeur_aleatoire(1,3);
		ttrajet += t;
		sleep(t);
    
		/* section AB */
        printf("section AB - %d\n",gettid());
		t = valeur_aleatoire(2,7);
		ttrajet += t;
		sleep(t);

		while(1){
            message[0] = 'B';
            message[1] = 'O';
            // Envoyer le message = "BO" au serveur pour indiquer le sens
            if( send(sock , message , strlen(message) , 0) < 0) {
                puts("Send failed");
                exit(1);
            }
            //Receive a reply from the server
            printf("waiting for server reply %d\n", gettid());
            if( recv(sock , server_reply , 10 , 0) < 0) {
                puts("recvfailed");
                break;
            } else {
                // Si la reponse du serveur est "pass", la voiture peut passer
                if (strcmp(server_reply, "OK") == 0) {
                    printf("Server allowed me to pass - %d\n", gettid());
                    break;
                } else {
                    printf("Server Didn't allow me to pass, trying after 1 second - %d\n", gettid());
                    sleep(1);
                    ttrajet += 1;
                }
            }
        }
     
        /* section BO */
        printf("section BO - %d\n",gettid());
		t = valeur_aleatoire(1,3);
		ttrajet += t;
		sleep(t);
		/* section OC */
        printf("section OC - %d\n",gettid());
		t = valeur_aleatoire(1,3);
		ttrajet += t;
		sleep(t);

        /* section CD */
        printf("section CD - %d\n",gettid());
		t = valeur_aleatoire(3,5);
		ttrajet += t;
		sleep(t);

		while(1){
            message[0] = 'D';
            message[1] = 'O';
            // Envoyer le message = "DO" au serveur pour indiquer le sens
            if( send(sock , message , strlen(message) , 0) < 0) {
                puts("Send failed");
                exit(1);
            }
            //Receive a reply from the server
            if( recv(sock , server_reply , 2000 , 0) < 0) {
                puts("recvfailed");
                break;
            } else {
                // Si la reponse du serveur est "pass", la voiture peut passer
                if (strcmp(server_reply, "OK") == 0) {
                    printf("Server allowed me to pass - %d\n", gettid());
                    break;
                } else {
                    printf("Server Didn't allow me to pass, trying after 1 second - %d\n", gettid());
                    sleep(1);
                    ttrajet += 1;
                }
            }


        }
        /* section DO */
        printf("section DO %d\n",gettid());
		t = valeur_aleatoire(1,3);
		ttrajet += t;
		sleep(t);

		printf("***Voiture %d, tour numero %d, temps jusqu'au point O: %d sec***\n",gettid(), i, ttrajet);
	}
	printf("******Arrivée de la voiture %d******\n",gettid());
}


int main()
{
    int i, nb_tours;
    nb_tours = NB_TOURS;


    for(i = 0; i < NB_VOITURES; i++){
        if( pthread_create( &pthread_id[i], NULL, threadVoiture,&nb_tours) == -1) {
            fprintf(stderr, " Erreur de creation du pthread numero%d", i);
        }
    }
    for(i = 0; i < NB_VOITURES; i++) {
        pthread_join( pthread_id[i], NULL);
    }

    return EXIT_SUCCESS;
}
