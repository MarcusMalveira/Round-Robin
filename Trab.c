#include <stdio.h>
#include <stdlib.h>

// Structs

typedef struct {
    int id; // Identificador do processo
    int tempoChegada; // Instante em que o processo chega
    int duracao; // Tempo total de CPU necessário (burst time)
    int prioridade; // Menor valor = maior prioridade
} Processo;

typedef struct No {
    Processo processo;
    struct No *prox; // Ponteiro para o proximo da fila
} No;

typedef struct {
    No *inicio; // Ponteiro para inicio da fila
    No *fim;    // Ponteiro para o fim da fila
} Fila;

Fila *criar_fila(){
    Fila *f = (Fila*) malloc(sizeof(Fila)); // Aloca memória para a fila
    f->inicio = NULL; // Fila começa vazia
    f->fim = NULL;
    return f;
}

// Funcao para inserir elemento no final da fila
void enfileirar(Fila *f, Processo p){
    No *novo = (No*) malloc(sizeof(No)); // Cria um novo no
    novo->processo = p;
    novo->prox = NULL; // Ultimo no aponta para NULL

    // Caso a fila estiver vazia, inicio aponta para novo no, senao antigo fim aponta para novo no
    if(f->inicio == NULL){
        f->inicio = novo;
    } else {
        f->fim->prox = novo;
    }

    f->fim = novo; // Atualiza fim da fila
}
// remove e retorna o processo do inicio da fila
Processo desenfileirar(Fila *f){
    No *aux = f->inicio; // guarda nó que sera removido
    Processo p = aux->processo; // guarda o processo do nó

    f->inicio = aux->prox;  // avanca o inicio da fila
    free(aux); // libera memoria do nó removido

    // verificar se a fila ficou vazia
    if(f->inicio == NULL){
        f->fim = NULL;
    }

    return p; // retorna processo removido
}

// Verifica se a fila está vazia
int filaVazia(Fila *f){
    return f->inicio == NULL;
}


int main(){
    int n, quantum;
    int tempoAtual = 0, finalizados = 0; // Relogio do sistema e contador de processos finalizados

    // Leitura da quantidade de processos
    printf("Quantidade de processos: ");
    scanf("%d", &n);

    // Leitura do Quantum(intervalo) do programa
    printf("Quantum: ");
    scanf("%d", &quantum);


    Processo *processos = (Processo*) malloc(n * sizeof(Processo));
    int *inserido = (int*) calloc(n, sizeof(int));
    int *burstOriginal = (int*) malloc(n * sizeof(int));
    int *tempoFinal = (int*) malloc(n * sizeof(int));
    int *primeiraExecucao = (int*) malloc(n * sizeof(int));

    for(int i = 0; i < n; i++)
        primeiraExecucao[i] = -1;

    // Cria a fila
    Fila *fila = criar_fila();

    // Leitura dos dados dos processos
    for(int i = 0; i < n; i++){
        printf("\nProcesso %d\n", i + 1);
        printf("ID: ");
        scanf("%d", &processos[i].id);
        printf("Tempo de chegada: ");
        scanf("%d", &processos[i].tempoChegada);
        printf("Duracao: ");
        scanf("%d", &processos[i].duracao);
        printf("Prioridade: ");
        scanf("%d", &processos[i].prioridade);

        burstOriginal[i] = processos[i].duracao;
    }

    printf("\nEscalonamento Round-Robin\n");

    // Loop para o escalonamento
    // Termina quando todos os dados acabarem
    while(finalizados < n){

        // Insere na fila os dados que chegam
        for(int i = 0; i < n; i++){
            if(inserido[i] == 0 && processos[i].tempoChegada <= tempoAtual){
                enfileirar(fila, processos[i]);
                inserido[i] = 1;
            }
        }

        // Se a fila estiver vazia, avança o tempo(para o sistema não ficar travado enquanto espera um processo que nunca chega)
        if(filaVazia(fila)){
            tempoAtual++;
            continue;
        }

        Processo atual = desenfileirar(fila); // Remove o próximo processo da fila

        int idx = atual.id - 1; // Índice auxiliar (assumindo IDs começando em 1)

        // Marca tempo da primeira execução, -1 para sinalizar que o processe nunca foi executado
        if(primeiraExecucao[idx] == -1){
            primeiraExecucao[idx] = tempoAtual;
        }

        // caso a duracao do processo seja maior que o quantum, a duracao é reduzida pelo valor de quantum e volta para o final da fila
        if(atual.duracao > quantum){
            printf("Tempo %d -> Processo %d executou %d\n", tempoAtual, atual.id, quantum);
            atual.duracao -= quantum;
            tempoAtual += quantum;
            enfileirar(fila, atual);
        } 
        // se a duracao do processo - quantom = 0, é finalizado
        else {
            printf("Tempo %d -> Processo %d executou %d e FINALIZOU\n", tempoAtual, atual.id, atual.duracao);
            tempoAtual += atual.duracao;
            tempoFinal[idx] = tempoAtual;
            finalizados++;
        }
    }

    //Metricas do programa

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
    printf("Throughput: %.2f processos/unidade de tempo\n", (double)n / tempoAtual);

    // Liberacao de memoria
    free(processos);
    free(inserido);
    free(burstOriginal);
    free(tempoFinal);
    free(primeiraExecucao);
    free(fila);

    return 0;
}