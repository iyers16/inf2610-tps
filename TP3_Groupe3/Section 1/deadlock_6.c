#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
/*
1. Exclusion Mutuelle (Mutual Exclusion)
    - Condition non satisfaite : Ce programme ne possède pas de section critique où plusieurs threads
      accèdent à une ressource partagée nécessitant un verrou ou une exclusion mutuelle.

2. Détention et Attente (Hold and Wait)
    - Condition non satisfaite : Les threads n'acquièrent aucune ressource qu'ils retiennent tout
      en attendant une autre ressource.

3. Pas de Réquisition (No Preemption)
    - Condition non applicable : Les threads ne détiennent aucune ressource qui pourrait être
      réquisitionnée.

4. Attente Circulaire (Circular Wait)
    - Condition non satisfaite : Les threads ne forment pas une chaîne d'attente circulaire pour
      des ressources, car aucune ressource n'est utilisée.
*/
/*
2. Identification de la situation d’interblocage :
    - Ce code ne représente pas un modèle classique comme Lecteur-Rédacteur, Producteur-Consommateur,
      ou le Problème des Philosophes.
    - Cependant, il peut causer un comportement non défini ou inefficace à cause de la boucle infinie
      dans laquelle les threads attendent que `flag` soit mis à jour.
    - Cela peut être considéré comme un problème de "boucle active" (busy waiting), où les threads
      consomment inutilement des ressources CPU en vérifiant continuellement `flag` sans cession
      de temps processeur.
*/

#define N_THREADS 5

int flag = 0;

void *function_that_does_something(void *arg)
{
    while (1)
    {
        if (flag)
            break;
    }
    pthread_exit(NULL);
}

void changeFlag()
{
    /*
        Section modifie
            Avec la configuration precedente il n'y rendait jamais a 10, donc j'y ai inverse la condition.
            pour qu'il declenche toujours en recevant le signal et en gardant la meme structure.
    */
    if (rand() % 10 != 10)
        flag = 1;
    printf("Signal called\n");
}

int main()
{
    pthread_t threads[N_THREADS];

    signal(SIGALRM, &changeFlag);

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, function_that_does_something, NULL);
    }

    alarm(1);

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}