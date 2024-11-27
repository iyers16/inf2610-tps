#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
/*
1. Exclusion Mutuelle (Mutual Exclusion)
    - Condition satisfaite : Le sémaphore `sem_critical` garantit qu'un seul thread peut entrer
      dans la section critique à un moment donné.
    - Code pertinent :
        sem_wait(&sem_critical);  // Acquisition du sémaphore
        sem_post(&sem_critical); // Libération du sémaphore
    - Explication :
        - Le sémaphore empêche plusieurs threads de vérifier simultanément la clé,
          satisfaisant la condition d'exclusion mutuelle.

2. Détention et Attente (Hold and Wait)
    - Condition satisfaite : Un thread détient le sémaphore `sem_critical` tout en exécutant
      `try_lock(key)` et peut potentiellement attendre indéfiniment dans la boucle `while(1)`
      pour une clé valide.
    - Code pertinent :
        sem_wait(&sem_critical);  // Détention du sémaphore
        if (try_lock(key))        // Attente pour valider la clé
    - Explication :
        - Les threads peuvent rester bloqués à l'intérieur de la boucle et continuer à attendre.

3. Pas de Réquisition (No Preemption)
    - Condition satisfaite : Une fois qu'un thread acquiert le sémaphore avec `sem_wait`,
      il ne peut pas être forcé de le libérer avant d'appeler explicitement `sem_post`.
    - Code pertinent :
        sem_wait(&sem_critical);  // Acquisition explicite
        sem_post(&sem_critical); // Libération explicite
    - Explication :
        - Le sémaphore ne peut pas être réquisitionné par d'autres threads.

4. Attente Circulaire (Circular Wait)
    - Condition non satisfaite : Les threads acquièrent le sémaphore de manière séquentielle,
      donc il n'y a pas de dépendance circulaire entre plusieurs ressources.
*/

/*
2. Identification de la situation d’interblocage :
    - Ce programme ne correspond pas à un modèle classique comme Producteur-Consommateur
      ou Lecteur-Rédacteur, mais il présente un risque d'interblocage si un thread entre dans une
      boucle infinie après avoir acquis le sémaphore.
    - Description :
        - Si un thread valide la clé avec `try_lock(key)` mais oublie de libérer le sémaphore avant
          `pthread_exit`, les autres threads seront bloqués.
*/

#define ENTRY_CODE 7
#define N_THREADS 5

sem_t sem_critical;
/*
    Section modifie
        Dans la configuration initiale, un thread peut quitter la boucle `while(1)` après avoir
        validé la clé dans `try_lock(key)`, mais sans libérer le sémaphore. Cela entraîne un blocage
        pour les autres threads.
        La solution consiste à libérer explicitement le sémaphore avant d'appeler `pthread_exit` tout simplement.
*/
int try_lock(int key)
{
    sem_post(&sem_critical);

    if (key == ENTRY_CODE)
    {
        printf("Door unlocked\n");
        return 1;
    }
    sleep(1);
    return 0;
}

void *door(void *args)
{
    while (1)
    {
        sem_wait(&sem_critical);
        int key = rand() % 30;
        if (try_lock(key))
            pthread_exit(NULL);
    }
}

int main()
{

    sem_init(&sem_critical, 0, 1);
    pthread_t threads[N_THREADS];

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, door, NULL);
    }

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}