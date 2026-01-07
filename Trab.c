#include <stdio.h>
#include <stdlib.h>


typedef struct {
    int id; // Identificador do processo
    int tempoChegada; // Tempo de chegada
    int duracao; // Burst time (tempo restante)
    int prioridade; // Prioridade (não usada no RR)
} Processo;

typedef struct No {
    Processo processo; // Processo armazenado no nó
    struct No *prox; // Próximo nó da fila
} No;

typedef struct {
    No *inicio; // Início da fila
    No *fim; // Fim da fila
} Fila;


// Cria uma fila vazia
Fila *criar_fila(){
    Fila *f = (Fila*) malloc(sizeof(Fila));
    f->inicio = NULL;
    f->fim = NULL;
    return f;
}

// Verifica se a fila está vazia
int filaVazia(Fila *f){
    return f->inicio == NULL;
}

// Insere um processo no final da fila
void enfileirar(Fila *f, Processo p){
    No *novo = (No*) malloc(sizeof(No));
    novo->processo = p;
    novo->prox = NULL;

    if(f->inicio == NULL){
        f->inicio = novo;
    } else {
        f->fim->prox = novo;
    }

    f->fim = novo;
}

// Remove e retorna o processo do início da fila
Processo desenfileirar(Fila *f){
    No *aux = f->inicio;
    Processo p = aux->processo;

    f->inicio = aux->prox;
    free(aux);

    if(f->inicio == NULL){
        f->fim = NULL;
    }

    return p;
}

int main(){
    int n, quantum;
    int tempoAtual = 0;
    int finalizados = 0;

    printf("Quantidade de processos: ");
    scanf("%d", &n);

    printf("Quantum: ");
    scanf("%d", &quantum);

    Processo *processos = (Processo*) malloc(n * sizeof(Processo));
    int *inserido = (int*) calloc(n, sizeof(int));
    int *burstOriginal = (int*) malloc(n * sizeof(int));
    int *tempoFinal = (int*) malloc(n * sizeof(int));
    int *primeiraExecucao = (int*) malloc(n * sizeof(int));

    for(int i = 0; i < n; i++)
        primeiraExecucao[i] = -1;

    Fila *fila = criar_fila();

    // Leitura dos processos
    for(int i = 0; i < n; i++){
        printf("\nProcesso %d\n", i + 1);

        processos[i].id = i + 1;

        printf("Tempo de chegada: ");
        scanf("%d", &processos[i].tempoChegada);

        printf("Duracao: ");
        scanf("%d", &processos[i].duracao);

        printf("Prioridade: ");
        scanf("%d", &processos[i].prioridade);

        burstOriginal[i] = processos[i].duracao;
    }

    printf("\nEscalonamento Round Robin\n");


    while(finalizados < n){

        // Insere processos que já chegaram
        for(int i = 0; i < n; i++){
            if(inserido[i] == 0 && processos[i].tempoChegada <= tempoAtual){
                enfileirar(fila, processos[i]);
                inserido[i] = 1;
            }
        }

        // Se a fila estiver vazia, avança o tempo
        if(filaVazia(fila)){
            tempoAtual++;
            continue;
        }

        // Retira processo da fila
        Processo atual = desenfileirar(fila);
        int idx = atual.id - 1;

        // Marca primeira execução
        if(primeiraExecucao[idx] == -1){
            primeiraExecucao[idx] = tempoAtual;
        }

        // Executa conforme o quantum
        if(atual.duracao > quantum){
            printf("Tempo %d -> Processo %d executou %d\n",
                   tempoAtual, atual.id, quantum);

            atual.duracao -= quantum;
            tempoAtual += quantum;

            enfileirar(fila, atual);
        } else {
            printf("Tempo %d -> Processo %d executou %d e FINALIZOU\n",
                   tempoAtual, atual.id, atual.duracao);

            tempoAtual += atual.duracao;
            tempoFinal[idx] = tempoAtual;
            finalizados++;
        }
    }


    double somaWT = 0, somaTT = 0, somaRT = 0;

    for(int i = 0; i < n; i++){
        int turnaround = tempoFinal[i] - processos[i].tempoChegada;
        int waiting = turnaround - burstOriginal[i];
        int response = primeiraExecucao[i] - processos[i].tempoChegada;

        somaWT += waiting;
        somaTT += turnaround;
        somaRT += response;
    }

    printf("\nMetricas\n");
    printf("Waiting Time medio: %.2f\n", somaWT / n);
    printf("Turnaround Time medio: %.2f\n", somaTT / n);
    printf("Response Time medio: %.2f\n", somaRT / n);
    printf("Throughput: %.2f processos/unidade de tempo\n",
           (double)n / tempoAtual);

    free(processos);
    free(inserido);
    free(burstOriginal);
    free(tempoFinal);
    free(primeiraExecucao);
    free(fila);

    return 0;
}
