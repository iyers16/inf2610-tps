#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
/*
1. Exclusion Mutuelle (Mutual Exclusion)
   - Condition satisfaite : Les descripteurs de fichier `fd[0]` (lecture) et `fd[1]` (écriture)
     représentent des ressources mutuellement exclusives. Une seule opération de lecture ou
     d'écriture peut avoir lieu à la fois.
   - Code pertinent :
       pipe(fd);                // Création d'un pipe avec des descripteurs mutuellement exclusifs
       read(fd[0], string, 6);  // Le descripteur `fd[0]` est utilisé pour la lecture
       write(fd[1], &string, strlen(string));  // Le descripteur `fd[1]` est utilisé pour l'écriture
   - Explication :
       - Les descripteurs de fichier ne peuvent pas être partagés simultanément par plusieurs processus
         pour la même opération (lecture/écriture), satisfaisant la condition d'exclusion mutuelle.

2. Détention et Attente (Hold and Wait)
   - Condition satisfaite : Un processus détient un descripteur de fichier tout en attendant une autre
     ressource pour continuer son exécution.
   - Code pertinent :
       close(fd[1]);          // Un processus ferme une extrémité du pipe et attend de l'autre côté.
       read(fd[0], string, 6);  // Le processus enfant attend des données sur le descripteur `fd[0]`.
       write(fd[1], &string, strlen(string));  // Le processus parent attend que l'autre processus lise les données.
   - Explication :
       - Les processus parent et enfant détiennent des descripteurs de fichier différents
         (fd[0] ou fd[1]) et attendent que l'autre processus effectue une opération
         pour poursuivre leur exécution.

3. Pas de Réquisition (No Preemption)
   - Condition satisfaite : Les descripteurs de fichier ne peuvent pas être retirés de force à un processus.
     Un processus doit libérer explicitement le descripteur via `close(fd[x])`.
   - Code pertinent :
       close(fd[1]);  // Libération explicite de l'extrémité du pipe
       close(fd[0]);  // Libération explicite de l'autre extrémité du pipe
   - Explication :
       - Les descripteurs de fichier dans les pipes sont des ressources non préemptives, car ils ne
         peuvent être repris que si le processus effectue une libération volontaire.

4. Attente Circulaire (Circular Wait)
   - Condition non satisfaite : Les processus parent et enfant ne créent pas de cycle dans les
     dépendances des ressources. Chaque processus effectue une opération séquentielle
     sans attendre mutuellement de manière cyclique.
   - Explication :
       - Le parent écrit dans le pipe, et l'enfant lit du pipe. Cependant, il n'y a pas de dépendance
         mutuelle entre leurs descripteurs `fd[0]` et `fd[1]` qui formerait un cycle.
*/
/*
2. Identification de la situation d’interblocage :
   - Ce code représente une situation classique du problème Producteur-Consommateur.
   - Description :
       - Le processus parent agit comme un producteur qui écrit des données dans le pipe.
       - L'un des processus enfants agit comme un consommateur qui lit les données du pipe.
       - Cette situation est une démonstration des défis liés à la synchronisation entre processus
         pour accéder à des ressources partagées telles que des pipes.
   - Problème potentiel :
       - Si le parent ou l'enfant bloque sur le pipe en raison d'un problème de synchronisation
         (e.g., le pipe est vide ou plein), cela peut entraîner un blocage.
*/

int main()
{
    int fd[2];

    pipe(fd);

    if (!fork())
    {
        close(fd[1]);
        char *string[6];
        read(fd[0], string, 6);
        exit(0);
    }

    if (!fork())
    {
        close(fd[1]);
        close(fd[0]);
        execlp("ls", "ls", "-l", NULL);
        exit(0);
    }
    /*
    Section modifie
        Dans la configuration initiale, le processus parent écrit des données dans le pipe apres avoir attendu que les enfants terminent.
        Mais l'enfant qui lit les données du pipe attend indéfiniment que le parent écrive des données dans le pipe.
        On bouge le wait(NULL) après l'écriture des données dans le pipe pour éviter ce blocage.
        On ferme aussi le descripteur de lecture du pipe pour éviter que le processus parent ne lise les données qu'il a lui-même écrit.
    */
    char string[6] = "Salut!";
    close(fd[0]);
    write(fd[1], &string, strlen(string));
    printf("Process completed!\n");
    while (wait(NULL) > 0)
        ;
    return 0;
}