#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
/*
1. Exclusion Mutuelle (Mutual Exclusion)
    - Condition satisfaite : Chaque thread acquiert des verrous (`locks[left]` et `locks[right]`)
      de manière exclusive en utilisant `pthread_mutex_lock`. Cela empêche plusieurs threads
      d'accéder simultanément aux mêmes verrous.
    - Code pertinent :
        pthread_mutex_lock(&locks[left]);  // Acquisition du verrou gauche
        pthread_mutex_lock(&locks[right]); // Acquisition du verrou droit
    - Explication :
        - Les mutex garantissent que les ressources partagées (les fourchettes) sont
          en exclusion mutuelle.

2. Détention et Attente (Hold and Wait)
    - Condition satisfaite : Chaque thread détient un verrou (gauche) tout en attendant d'acquérir
      le second verrou (droit).
    - Code pertinent :
        pthread_mutex_lock(&locks[left]);  // Le thread détient le verrou gauche
        pthread_mutex_lock(&locks[right]); // Attente pour acquérir le verrou droit
    - Explication :
        - Un thread peut bloquer en attendant le verrou droit si celui-ci est déjà détenu par un autre thread.

3. Pas de Réquisition (No Preemption)
    - Condition satisfaite : Une fois qu'un thread acquiert un verrou, il ne peut pas
      être forcé à le libérer. Il doit appeler `pthread_mutex_unlock` pour libérer la ressource.
    - Code pertinent :
        pthread_mutex_lock(&locks[left]);  // Le thread acquiert le verrou
        pthread_mutex_unlock(&locks[left]); // Le thread libère le verrou
    - Explication :
        - Les mutex ne peuvent pas être préemptés, satisfaisant cette condition.

4. Attente Circulaire (Circular Wait)
    - Condition satisfaite : Les threads acquièrent les verrous dans un ordre circulaire :
      le thread 0 attend le verrou 1, le thread 1 attend le verrou 2, ..., et le thread 4 attend le verrou 0.
    - Code pertinent :
        int left = id;
        int right = (id + 1) % NUM_THREADS; // Dépendance circulaire créée par l'ordre
    - Explication :
        - Une attente circulaire se produit car les threads forment une chaîne
          d'attente où chacun dépend du suivant, créant un cycle.
*/

/*
2. Identification de la situation d’interblocage :
    - Ce programme illustre une version simplifiée du problème classique des philosophes
      qui dînent (Problème des Philosophes).
    - Description :
        - Chaque thread représente un philosophe qui tente de prendre deux fourchettes
          (les verrous gauche et droit) pour "manger".
        - Les philosophes doivent acquérir deux fourchettes, mais ils les acquièrent dans un ordre
          circulaire. Cela crée une dépendance circulaire entre les threads.
    - Problème :
        - Si tous les threads acquièrent simultanément un verrou (gauche), ils bloqueront en
          attendant le second verrou (droit), entraînant un interblocage.
*/

#define NUM_THREADS 5
#define MAX_ROUNDS 3

pthread_mutex_t locks[NUM_THREADS];

/*
    Section modifie
        Dans la configuration initiale, chaque thread acquiert ses verrous dans un ordre circulaire
        (gauche, puis droit), ce qui crée une dépendance circulaire et un interblocage potentiel.
        La solution consiste à imposer un ordre strict d'acquisition des verrous pour briser la dépendance circulaire.
        On modifie le code pour que chaque thread acquière d'abord le verrou avec l'identifiant le plus bas,
        puis le second verrou. Cela élimine l'attente circulaire.
*/
void *cons(void *arg)
{
    int id = *(int *)arg;
    int rounds = 0;

    while (rounds < MAX_ROUNDS)
    {
        sleep(1);

        int left = id;
        int right = (id + 1) % NUM_THREADS;

        if (left < right)
        {
            pthread_mutex_lock(&locks[left]);
            sleep(1);
            pthread_mutex_lock(&locks[right]);
        }
        else
        {
            pthread_mutex_lock(&locks[right]);
            sleep(1);
            pthread_mutex_lock(&locks[left]);
        }

        sleep(1);
        rounds++;
        pthread_mutex_unlock(&locks[left]);
        pthread_mutex_unlock(&locks[right]);
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_mutex_init(&locks[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], NULL, cons, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}