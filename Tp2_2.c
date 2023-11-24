#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<linux/unistd.h>

#define NB_VOITURES 3
#define NB_TOURS 2
int f_BC = 1;
int f_DA = -1;

pthread_cond_t cond_BC = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_DA = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_BC = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_DA = PTHREAD_MUTEX_INITIALIZER;

pthread_t pthread_id[NB_VOITURES];
pthread_t pthread;


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
void * threadVoiture ( void * nbTour ) {
    int time = 0;
    int tour = 1;
    int i = pthread_self();
    
    initialise_generateur_aleatoire () ;
    printf("Départ de la voiture %d\n", i);
    time = valeur_aleatoire(4, 13);
    
    pthread_mutex_lock(&lock_BC);
    if(f_BC == -1){
    pthread_cond_wait(&cond_BC, &lock_BC);
    }
    pthread_mutex_unlock(&lock_BC);
    
    time += valeur_aleatoire(5, 11);
    printf("Voiture %d, tour numéro %d, temps jusqu’au point O: %d sec\n", i, tour, time);
    
    pthread_mutex_lock(&lock_DA);
    if(f_DA == -1){
    pthread_cond_wait(&cond_DA, &lock_DA);
    }
    pthread_mutex_unlock(&lock_DA);
    
    tour += 1;
    time += valeur_aleatoire(4, 13);
    
    pthread_mutex_lock(&lock_BC);
    if(f_BC == -1){
    pthread_cond_wait(&cond_BC, &lock_BC);
    }
    pthread_mutex_unlock(&lock_BC);
    
    time += valeur_aleatoire(5, 11);
    printf("Voiture %d, tour numéro %d, temps jusqu’au point O: %d sec\n", i, tour, time);
    
    pthread_mutex_lock(&lock_DA);
    if(f_DA == -1){
    pthread_cond_wait(&cond_DA, &lock_DA);
    }
    pthread_mutex_unlock(&lock_DA);
    
    printf("Arrivée de la voiture %d\n", i);
}

void * feu (){
    while(1){
        pthread_mutex_lock(&lock_DA);
        pthread_mutex_lock(&lock_BC);
        f_BC = -f_BC;
        f_DA = -f_DA;
        pthread_mutex_unlock(&lock_DA);
        pthread_mutex_unlock(&lock_BC);

        if(f_BC == 1){
        pthread_cond_signal(&cond_BC);
        printf("BC\n");
        sleep(2);
        }else{
        pthread_cond_signal(&cond_DA);
        printf("DA\n");
        sleep(2);
        }
    }
}

int main()
{
    int i, nb_tours;
    nb_tours = NB_TOURS;
    
    pthread_create( &pthread, NULL, feu, NULL);
    
    for(i = 0; i < NB_VOITURES; i++)
    {
        if( pthread_create( &pthread_id[i], NULL, threadVoiture, &nb_tours) == -1)
        {
        fprintf(stderr, " Erreur de creation du pthread numero %d\n", i);
        }
    }
    for(i = 0; i < NB_VOITURES; i++)
    {
        pthread_join( pthread_id[i], NULL);
    }
    return EXIT_SUCCESS;
}
