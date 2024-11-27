#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

/*
1. Exclusion Mutuelle (Mutual Exclusion)
    - Condition satisfaite : Le sémaphore `sem_critical` garantit que l'accès au buffer
    est en exclusion mutuelle. Un seul thread (producteur ou consommateur) peut entrer
    dans la section critique et modifier les ressources partagées (`buffer`, `ip`, ou `ic`)
    à un moment donné.
    - Code pertinent :
        sem_wait(&sem_critical);  // Le producteur ou le consommateur acquiert la section critique
        // Section critique modifiant `buffer`, `ip` ou `ic`
        sem_post(&sem_critical);  // Le producteur ou le consommateur libère la section critique
    - Explication :
        - L'utilisation de `sem_critical` garantit l'exclusion mutuelle, une condition nécessaire
            pour un interblocage, car la ressource partagée (buffer) ne peut pas être accédée
            simultanément par plusieurs threads.

2. Détention et Attente (Hold and Wait)
    - Condition satisfaite : Un thread détient une ressource tout en attendant une autre.
        - Le producteur détient `sem_initial` (un emplacement vide) tout en attendant
            `sem_critical` (verrou de section critique).
        - Le consommateur détient `sem_busy` (un emplacement rempli) tout en attendant
            `sem_critical` (verrou de section critique).
    - Code pertinent (Producteur) :
        sem_wait(&sem_initial);  // Attend un emplacement vide
        sem_wait(&sem_critical); // Attend l'accès à la section critique pour modifier le buffer
    - Code pertinent (Consommateur) :
        sem_wait(&sem_busy);     // Attend un emplacement rempli
        sem_wait(&sem_critical); // Attend l'accès à la section critique pour modifier le buffer
    - Explication :
        - Les producteurs et les consommateurs peuvent détenir un sémaphore tout en attendant
            un autre, ce qui satisfait la condition de détention et attente, une condition nécessaire
            pour un interblocage.

3. Pas de Réquisition (No Preemption)
    - Condition satisfaite : Une fois qu'un thread acquiert un sémaphore (ressource), il
    ne peut pas être forcé à le libérer. Le thread doit le libérer volontairement en utilisant
    `sem_post`.
    - Code pertinent :
        sem_wait(&sem_initial);  // Le producteur acquiert un emplacement vide
        sem_post(&sem_initial);  // Le consommateur libère l'emplacement vide
        sem_wait(&sem_busy);     // Le consommateur acquiert un emplacement rempli
        sem_post(&sem_busy);     // Le producteur libère l'emplacement rempli
    - Explication :
        - Les sémaphores `sem_initial` et `sem_busy` sont non préemptifs. Les threads
            attendant ces sémaphores doivent attendre qu'ils soient libérés volontairement
            par les threads qui les détiennent. Cela satisfait la condition de pas de réquisition,
            une autre condition nécessaire pour un interblocage.

4. Attente Circulaire (Circular Wait)
    - Condition non satisfaite : L'ordre d'acquisition des sémaphores empêche une attente circulaire.
        - Le producteur acquiert toujours `sem_initial` avant `sem_critical`.
        - Le consommateur acquiert toujours `sem_busy` avant `sem_critical`.
        - Il n'y a aucun chevauchement dans les dépendances entre les ressources des producteurs
            et des consommateurs qui pourrait former un cycle.
    - Code pertinent (Producteur) :
        sem_wait(&sem_initial);  // Acquiert un emplacement vide en premier
        sem_wait(&sem_critical); // Acquiert ensuite l'accès à la section critique
    - Code pertinent (Consommateur) :
        sem_wait(&sem_busy);     // Acquiert un emplacement rempli en premier
        sem_wait(&sem_critical); // Acquiert ensuite l'accès à la section critique
    - Explication :
        - Comme le producteur et le consommateur n'ont pas de dépendances circulaires
            (aucun thread n'est bloqué en attendant une ressource détenue par un autre dans un cycle),
            la condition d'attente circulaire n'est pas satisfaite.
*/

/*
2. Identification de la situation d’interblocage :
    - Ce code illustre une situation classique de "Producteur-Consommateur".
    - Description :
        - Les producteurs ajoutent des éléments dans un buffer partagé de taille limitée (BUFFER_SIZE).
        - Les consommateurs retirent des éléments du même buffer.
        - Ce problème est une démonstration typique des défis de la programmation multithread,
            tels que la synchronisation des threads, l’accès aux ressources partagées, et la gestion
        des conditions d’interblocage.
    - L’interblocage dans ce programme survient lorsque tous les producteurs sortent après
    avoir détecté le drapeau (`flag`), laissant les consommateurs bloqués sur `sem_wait(&sem_busy)`
    car aucun autre producteur ne peut incrémenter `sem_busy`.
*/

#define BUFFER_SIZE 5
#define N_THREADS 1
#define N_THREADS_2 10

int buffer[BUFFER_SIZE];
int ip;
int ic;

sem_t sem_initial;
sem_t sem_busy;
sem_t sem_critical;

int flag = 0;

void *producer(void *arg)
{
    while (1)
    {
        sem_wait(&sem_initial);
        sem_wait(&sem_critical);
        buffer[ip] = rand() % 9 + 1;
        ip = (ip + 1) % BUFFER_SIZE;
        sem_post(&sem_critical);
        sem_post(&sem_busy);
        /* Section modifie
            Dans cette section, le producteur vérifie si le signal d'alarme a été déclenché.
            Si c'est le cas, il remplit le buffer avec des zéros et sort de la boucle infinie.
            Cela permet a tous les consommateurs de terminer leur travail et de sortir de la boucle infinie.
        */
        if (flag)
        {
            for (int i = 0; i < N_THREADS_2; i++)
            {
                sem_wait(&sem_initial);
                sem_wait(&sem_critical);
                buffer[ip] = 0;
                ip = (ip + 1) % BUFFER_SIZE;
                sem_post(&sem_critical);
                sem_post(&sem_busy);
            }
            break;
        }
    }
    pthread_exit(NULL);
}

void *consumer(void *arg)
{
    while (1)
    {
        sem_wait(&sem_busy);
        sem_wait(&sem_critical);
        int x = buffer[ic];
        ic = (ic + 1) % BUFFER_SIZE;
        sem_post(&sem_critical);
        sem_post(&sem_initial);
        if (x == 0)
        {
            break;
        }
    }
    pthread_exit(NULL);
}

void changeFlag()
{
    flag = 1;
    printf("Flag changed\n");
}

int main()
{
    pthread_t prod_thread[N_THREADS], cons_thread[N_THREADS_2];

    sem_init(&sem_initial, 0, BUFFER_SIZE);
    sem_init(&sem_busy, 0, 0);
    sem_init(&sem_critical, 0, 1);
    signal(SIGALRM, &changeFlag);

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_create(&prod_thread[i], NULL, producer, NULL);
    }

    for (int i = 0; i < N_THREADS_2; i++)
    {
        pthread_create(&cons_thread[i], NULL, consumer, NULL);
    }

    alarm(1);

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(prod_thread[i], NULL);
    }

    for (int i = 0; i < N_THREADS_2; i++)
    {
        buffer[ip] = 0;
        ip = (ip + 1) % BUFFER_SIZE;
    }

    for (int i = 0; i < N_THREADS_2; i++)
    {
        pthread_join(cons_thread[i], NULL);
    }

    sem_destroy(&sem_initial);
    sem_destroy(&sem_busy);
    sem_destroy(&sem_critical);

    return 0;
}