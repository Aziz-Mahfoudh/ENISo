#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<time.h>
#include<unistd.h>

#define NB_VEHICULES 19
int NB_VOITURES = 15;
int NB_CAMIONS = 4;
int camionPresent = 0;
sem_t sem;
sem_t sem_compteur;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutProtect = PTHREAD_MUTEX_INITIALIZER;

pthread_t pthread_id[NB_VEHICULES];
void acceder_au_pont (int tonnes){
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&sem, 0, 3);
    if(tonnes == 5){
        pthread_mutex_lock (&mut);
        if(camionPresent == 1){
            pthread_mutex_unlock (&mut);
        }
        else{
         sem_wait(&sem);
         pthread_mutex_unlock (&mut);
        }

    }
    if(tonnes == 15){
        pthread_mutex_lock (&mutProtect);
        camionPresent ++;
        pthread_mutex_unlock (&mutProtect);
        pthread_mutex_lock (&mut);
        sem_wait(&sem);
        sem_wait(&sem);
        sem_wait(&sem);
        pthread_mutex_unlock (&mut);


    }

}
 void quitter_le_pont(int tonnes){
    if(tonnes == 5){
        sem_post(&sem);
    }
    if(tonnes == 15){
        sem_post(&sem);
        sem_post(&sem);
        sem_post(&sem);
        pthread_mutex_lock (&mutProtect);
        camionPresent --;
        pthread_mutex_unlock (&mutProtect);
    }


 }
 int valeur_aleatoire(int min, int max)
{

    int val;
    float f = random() * (max-min);
    f=(f / RAND_MAX) +min;
    return (f + 0.5);
}
void attendre(int t){
    sleep(valeur_aleatoire(5,t));
}
void initialise_generateur_aleatoire()
{
    srandom(time(NULL) + getpid());
}

 void* camion(void* arg)
{
    int pid=*((int*)arg);
    initialise_generateur_aleatoire() ;
    attendre(5);
    acceder_au_pont(15);
    printf("Camion %d : je traverse le pont\n",pid);
    attendre(5);
    printf("Camion %d : je quitte le pont\n",pid);
    quitter_le_pont(15);
    pthread_exit(NULL);
}
void* voiture(void* arg)
{
    int pid=*((int*)arg);
    initialise_generateur_aleatoire() ;
    attendre(5);
    acceder_au_pont(5);
    printf("Voiture %d :je traverse le pont\n",pid);
    attendre(5);
    printf("Voiture %d :je quitte le pont\n",pid);
    quitter_le_pont(5);
    pthread_exit(NULL);

}




int main(int argc, char* argv[]) {
    int i;
    pthread_t id;
    sem_init(&sem_compteur,0,3);
    for(i=0; i<NB_VEHICULES;i++){
        int*j=(int*)malloc(sizeof(int));
        *j=i;
        if (i<NB_CAMIONS)
            pthread_create(&pthread_id[i],NULL,camion,j);
        else pthread_create(&pthread_id[i],NULL,voiture,j);
    }
    for(i = 0; i < NB_VEHICULES; i++)
    {
        pthread_join( pthread_id[i], NULL);
    }
    pthread_exit(NULL);
};
