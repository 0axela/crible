#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#define k 4 // nombre de threads
//version séquentielle
bool* prime;
//pthread_mutex_t lck;
void CribleErathosthene(int n)
{

    // On crée un tableau de booléens, on initialise tous les éléments à true
    prime = (bool*) malloc((n + 1) * sizeof(bool));
    memset(prime, true, (n + 1) * sizeof(bool));

    for (int i = 2; i * i <= n; i++) { // on va jusqu'à la racine carrée de n
        if (prime[i] == true) {
            for (int j = i * i; j <= n; j += i)
                prime[j] = false;
        }
    }

    /*
    for (int p = 2; p <= n; p++)
        if (prime[p])
            printf("%d ",p);
    */
    free(prime);
}


//debut et fin pour chaque thread
typedef struct {
    int number; //numero du thread
    int n;
} Arguments;
void* Travail(void* arg) {
    Arguments *args = (Arguments *) arg;
    int number = args->number;
    int n = args->n;
    for (int i = 0; (2 * i + 3) * (2 * i + 3) <= n; i++) {
        if (prime[i] == true) {
            int currentValue = 2 * i + 3;
            if (currentValue == 3) {
                for (int j = currentValue * currentValue + ((int)((n - currentValue * currentValue) / (k * 2 * currentValue) + 1)) * number * 2 * currentValue; j <= n; j += 2 * currentValue) {
                    prime[(j - 3) / 2] = false;
                    //printf("prime[%d] = %d (%llu)\n", j, prime[j], pthread_self());
                }
            } else {
                for (int j = currentValue * currentValue + ((int)((n - currentValue * currentValue) / (k * 2 * currentValue) + 1)) * number * 2 * currentValue; j <= currentValue * currentValue + (((int)((n - currentValue * currentValue) / (k * 2 * currentValue) + 1)) * (number + 1) - 1) * 2 * currentValue; j += 2 * currentValue) {
                    if (j > n) break;
                    prime[(j - 3) / 2] = false;
                    //printf("prime[%d] = %d (%llu)\n", j, prime[j], pthread_self());
                }
            }
        }
    }
}

void CribleParallele(int n) {
    prime = (bool*) malloc(((n + 1) / 2) * sizeof(bool));
    memset(prime, true, ((n + 1) / 2) * sizeof(bool));

    pthread_t threads[k];
    Arguments args[k];

    for (int nt = 0; nt < k; nt++) {
        args[nt].number = nt;
        args[nt].n = n;
        pthread_create(&threads[nt], NULL, &Travail, (void*) &args[nt]);
    }

    for (int i = 0; i < k; i++)
        pthread_join(threads[i], NULL);
    /*
    int cpt = 0;
    for (int i = 0; 2 * i + 3 <= n; i++) {
        if (prime[i]) {
            cpt++;
            printf("%d ", 2 * i + 3);
        }
    }
    printf("%d ", cpt);
    */
    free(prime);
}


int main()
{
    double tab_par[100];
    double tab_seq[100];
    double time_spent_par = 0.0;
    double time_spent_seq = 0.0;
    for (int i = 0; i<100; i++) {
        time_spent_par = 0.0;
        clock_t begin = clock();
        CribleParallele(4000000);
        clock_t end = clock();
        time_spent_par += (double) (end - begin) / CLOCKS_PER_SEC;
        //printf("The elapsed time (parallele) is %f seconds", time_spent_par);
        tab_par[i] = time_spent_par;

        time_spent_seq = 0.0;
        begin = clock();
        CribleErathosthene(4000000);
        end = clock();
        time_spent_seq += (double) (end - begin) / CLOCKS_PER_SEC;
        tab_seq[i] = time_spent_seq;
        //printf("The elapsed time (pas parallele) is %f seconds", time_spent_seq);
    }

    //calcul de la moyenne
    double moyenne_par = 0.0;
    double moyenne_seq = 0.0;
    for (int i = 0; i<100; i++) {
        moyenne_par += tab_par[i];
        moyenne_seq += tab_seq[i];
    }
    moyenne_par = moyenne_par/100;
    moyenne_seq = moyenne_seq/100;
    printf("La moyenne du temps d'exécution pour la version parallèle est de %f secondes\n", moyenne_par);
    printf("La moyenne du temps d'exécution pour la version séquentielle est de %f secondes\n", moyenne_seq);
    //CribleParallele(100000);
    return 0;
}
