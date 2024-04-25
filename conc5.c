#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include "timer.h"

int N; // tamanho do vetor
int nthreads; // número de threads
long long int contador = 0;
long long int i_global = 1; // variável global compartilhada entre as threads

pthread_mutex_t mutex; // mutex para controlar o acesso à variável compartilhada i_global

int ehPrimo(long long int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<=sqrt(n); i+=2)
        if(n%i==0) return 0;
    return 1;
}

void *tarefa(void *arg) {
    long long int inicio, fim;
    int id = *((int *) arg);

    // Calcula a faixa de números que esta thread irá verificar
    inicio = (N / nthreads) * id + 1;
    fim = (id == nthreads - 1) ? N : (N / nthreads) * (id + 1);

    // Verifica os números primos na faixa atribuída
    for (long long int i = inicio; i <= fim; i++) {
        if (ehPrimo(i) == 1){
            pthread_mutex_lock(&mutex); // Trava o mutex antes de acessar a variável compartilhada
            contador++;
            pthread_mutex_unlock(&mutex); // Destrava o mutex após acessar a variável compartilhada

        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    pthread_t *tid; // identificadores das threads
    int *id; // identificadores locais das threads
    double inicio, fim, total; // variaveis de monitoramento de tempo
    
    GET_TIME(inicio);
    // leitura e avaliação dos parâmetros de entrada
    if(argc < 3) {
        printf("Digite: %s <tamanho do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }
    
    N = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    // Inicializa o mutex
    pthread_mutex_init(&mutex, NULL);

    // Alocação das estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        printf("ERRO--malloc\n");
        return 8;
    }
    id = (int *) malloc(sizeof(int) * nthreads);
    if (id == NULL) {
        printf("ERRO--malloc\n");
        return 9;
    }

    // Criação das threads
    for (int i = 0; i < nthreads; i++) {
        *(id + i) = i;
        if(pthread_create(tid + i, NULL, tarefa, (void *) (id + i))) {
            printf("ERRO--pthread_create\n");
            return 10;
        }
    }

    // Espera pelo término das threads
    for (int i = 0; i < nthreads; i++) {
        pthread_join(*(tid + i), NULL);
    }

    printf("quantidade de primos: %lld\n", contador);

    // Libera memória e destrói o mutex
    free(id);
    free(tid);
    pthread_mutex_destroy(&mutex);

    //calcula tempo total
    GET_TIME(fim);
    total = fim - inicio;
    printf("Tempo de execução:%lf\n", total);

    return 0;
}