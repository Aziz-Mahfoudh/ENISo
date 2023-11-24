 
#include<stdio.h>
#include<string.h>
 //strlen
#include<stdlib.h>
 //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>
 //write
#include<pthread.h> //for threading , link with lpthread
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/types.h>


//the thread function
#define NB_VOITURES 3
#define NB_TOURS 2

pthread_t pthread_id[NB_VOITURES],feu;

pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;

int feu_vert=0;

//generer id de la voiture
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
//
void* changer_feu()
{
while(1){
	sleep(2);
	pthread_mutex_lock(&Mutex);
	feu_vert =!feu_vert;
	pthread_mutex_unlock(&Mutex);
}}
//
/*** Procedure faisant effectuer les tours de pistes
* a une voiture.*/
void * threadVoiture ( void * nbTour ) {

	int i_NT;
	int t_NT;
	int trajet_NT = 0;

	initialise_generateur_aleatoire () ;

	printf("Depart de la voiture %d \n",gettid());
	for (i_NT = 1;i_NT <= *((int *)nbTour);i_NT++)
	{
		//
		pthread_mutex_lock(&Mutex);
		while (!feu_vert){
		pthread_mutex_unlock(&Mutex);
		sleep(1);
		pthread_mutex_lock(&Mutex);
		}pthread_mutex_unlock(&Mutex);


		/*OA*/

		t_NT = valeur_aleatoire(1,3);
		trajet_NT += t_NT;
		sleep(t_NT);

		/*AB */

		t_NT = valeur_aleatoire(2,7);
		trajet_NT += t_NT;
		sleep(t_NT);

		/*BO */

		t_NT = valeur_aleatoire(1,3);
		trajet_NT += t_NT;
		sleep(t_NT);

		//
		pthread_mutex_lock(&Mutex);
		while (feu_vert){
		pthread_mutex_unlock(&Mutex);
		sleep(1);
		pthread_mutex_lock(&Mutex);
		}pthread_mutex_unlock(&Mutex);
		/*OC */

        t_NT=valeur_aleatoire(1,3);
		trajet_NT += t_NT ;
		sleep(t_NT);

		/*CD */

		t_NT = valeur_aleatoire(3,5);
		trajet_NT += t_NT;
		sleep(t_NT);

		/*DO */

		t_NT = valeur_aleatoire(1,3);
		trajet_NT += t_NT;
		sleep(t_NT);

		printf("Voiture %d avec numero de tour %d et le temps jusqu'au point O: %d sec\n",gettid(), i_NT, trajet_NT);
	}
	printf("la voiture arrive %d\n",gettid());  }

void *connection_handler(void *);
int main(int argc , char *argv[])
{
    pthread_t sniffer_thread;

int socket_desc , client_sock , c , *new_sock;
struct sockaddr_in server , client;
int nbrConnection =0;
//Create socket
socket_desc = socket(AF_INET , SOCK_STREAM , 0);
if (socket_desc == -1)
{
printf("Could not create socket");
}
puts("Socket created");
//Prepare the sockaddr_in structure
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons( 8888 );
//Bind
if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
{
//print the error message
perror("bind failed. Error");
return 1;
}
puts("bind done");
//Listen
listen(socket_desc , 3);
//Accept and incoming connection
puts("Waiting for incoming connections...");
c = sizeof(struct sockaddr_in);
//Accept and incoming connection
puts("Waiting for incoming connections...");
c = sizeof(struct sockaddr_in);
while( (client_sock = accept(socket_desc, (struct sockaddr *)&client,
(socklen_t*)&c)) )
{
nbrConnection++ ;
puts("New Connection accepted");
printf("Le nombre de connections en cours est %d \n",
nbrConnection);

new_sock = malloc(1);
*new_sock = client_sock;
if(
 pthread_create(
 &sniffer_thread
 ,
 NULL
 ,
connection_handler , (void*) new_sock) < 0)
{
perror("could not create thread");
return 1;
}
//Now join the thread , so that we dont terminate before the thread
//pthread_join( sniffer_thread , NULL);
puts("Handler assigned");
}
if (client_sock < 0) {
perror("accept failed");
return 1;
}
return 0;
}
/* * This will handle connection for each client
* */
void *connection_handler(void *socket_desc)
{

	int i, nb_tours;
	nb_tours = NB_TOURS;
	pthread_create(&feu, NULL , changer_feu, NULL);
for(i = 0; i < NB_VOITURES; i++)
{
	if( pthread_create( &pthread_id[i], NULL, threadVoiture, &nb_tours) == -1)
{
	fprintf(stderr, " Erreur de creation du pthread numero %d", i);
}
}
for(i = 0; i < NB_VOITURES; i++)
{
	pthread_join( pthread_id[i], NULL);
}
//Get the socket descriptor
int sock = *(int*)socket_desc;
int read_size;
char *message , client_message[2000];

//Send some messages to the client
message = "Greetings! I am your connection handler\n";
write(sock , message , strlen(message));
message = "Now type something and i shall repeat what you type \n";
write(sock , message , strlen(message));
//Receive a message from client
while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
{
puts(client_message);
//Send the message back to client
write(sock , client_message , strlen(client_message));
}
if(read_size == 0)
 {
puts("Client disconnected");
fflush(stdout);
}
else if(read_size == -1)
 {
perror("recv failed");
}
//Free the socket pointer
free(socket_desc);
return EXIT_SUCCESS;

}
