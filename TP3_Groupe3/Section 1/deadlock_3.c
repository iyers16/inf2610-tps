#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
/*
1. Exclusion Mutuelle (Mutual Exclusion)
    - Condition satisfaite : Le sémaphore `mutex` garantit que l'accès à la section critique
      est en exclusion mutuelle. Un seul thread peut accéder à la section protégée par `sem_wait(&mutex)`
      à un moment donné.
    - Code pertinent :
        sem_wait(&mutex);  // Le thread acquiert l'accès exclusif à la section critique.
        sem_post(&mutex);  // Le thread libère l'accès exclusif à la section critique.
    - Explication :
        - L'utilisation du sémaphore `mutex` satisfait l'exclusion mutuelle, car il limite
          l'accès concurrent à la section critique partagée.

2. Détention et Attente (Hold and Wait)
    - Condition non satisfaite : Les threads acquièrent le sémaphore `mutex` sans attendre
      d'autres ressources. Aucun thread ne détient une ressource tout en attendant une autre.
    - Explication :
        - L'implémentation actuelle n'a pas de dépendance multi-ressources, ce qui signifie que
          les threads n'attendent pas indéfiniment pour d'autres ressources après avoir acquis `mutex`.

3. Pas de Réquisition (No Preemption)
    - Condition satisfaite : Une fois qu'un thread acquiert le sémaphore `mutex`, il ne peut pas
      être forcé à le libérer. Il doit appeler `sem_post` pour libérer volontairement la ressource.
    - Code pertinent :
        sem_wait(&mutex);  // Le thread acquiert le sémaphore.
        sem_post(&mutex);  // Le thread libère le sémaphore.
    - Explication :
        - Les sémaphores ne peuvent pas être préemptés, satisfaisant cette condition.

4. Attente Circulaire (Circular Wait)
    - Condition non satisfaite : Il n'y a qu'une seule ressource (le sémaphore `mutex`), donc
      une attente circulaire ne peut pas se produire. Les threads n'ont pas de dépendances
      cycliques entre plusieurs ressources.
    - Explication :
        - Avec une seule ressource à acquérir, une attente circulaire est impossible.
*/

/*
2. Identification de la situation d’interblocage :
    - Ce code ne représente pas un modèle classique d'interblocage tel que Lecteur-Rédacteur
      ou Producteur-Consommateur, car il n'y a pas de dépendances complexes entre plusieurs ressources.
    - Cependant, le comportement d'un interblocage peut survenir dans une situation particulière :
      si `key == 17`, un thread entre dans une boucle infinie, empêchant le sémaphore `mutex`
      d'être libéré pour les autres threads.
    - Ce problème peut être considéré comme un blocage artificiel dû à une logique mal conçue,
      mais pas comme un interblocage typique des modèles Lecteur-Rédacteur ou Producteur-Consommateur.
*/

#define N_THREADS 10

sem_t mutex;

int flag = 0;
int key;

void *is_this_a_deadlock(void *args)
{
    sem_wait(&mutex);
    int a = 0, b = 0, c = 0, d = 0;
    int i, j, k, m, n;

    a += 1;
    b += 2;
    c += 3;
    d += 4;
    a -= 1;
    b -= 2;
    c -= 3;
    d -= 4;

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            for (k = 0; k < 5; k++)
            {
                printf(".");
            }
        }
    }
    printf("\n");

    for (i = 0; i < 10; i++)
    {
        printf("Loop 1, Iteration: %d\n", i);
    }
    key = rand() % 100;
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            for (k = 0; k < 3; k++)
            {
                printf("+");
            }
        }
    }
    printf("\n");

    m = a + b;
    n = c - d;
    a = m + n;
    b = a - n;
    c = b + m;
    d = a + c;

    printf("Hello World 1\n");
    printf("Hello World 2\n");
    printf("Hello World 3\n");

    for (i = 0; i < 10; i++)
    {
        printf("Another loop: %d\n", i);
        a += i;
        b -= i;
        c += i * 2;
        d -= i * 2;
    }

    if (a > 0)
    {
        /*
            Section modifie
                Dans la configuration initiale, un interblocage peut se produire si `key == 17`,
                car un thread entre dans une boucle infinie tout en retenant le sémaphore `mutex`.
                Cela empêche les autres threads d'accéder à la section critique, ce qui provoque
                un blocage global du programme.
                La solution consiste à s'assurer que tous les threads libèrent toujours le sémaphore
                avant d'exécuter une boucle infinie ou des opérations longues.
                On ajoute une vérification et libération explicite du sémaphore avant la boucle infinie.
        */
        if (key == 17)
        {
            sem_post(&mutex);
            printf("Key is 17, entering infinite loop...\n");
            while (1)
                ;
        }
        printf("A is positive\n");
    }
    else
    {
        printf("A is not positive\n");
    }

    if (b < 0)
    {
        printf("B is negative\n");
    }
    else
    {
        printf("B is not negative\n");
    }

    printf("This function is long but doesn't do much.\n");
    sem_post(&mutex);
    return NULL;
}

int main()
{
    pthread_t threads[N_THREADS];
    sem_init(&mutex, 0, 1);
    srand(time(NULL));

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, is_this_a_deadlock, NULL);
    }

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}