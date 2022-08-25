#include <system.h>

#define MAX_PROCESS_NAME 30
#define INTERRUPT 8

typedef struct bcp {
    char name[MAX_PROCESS_NAME];
    enum { /* estados possiveis de um processo*/
           ativado,
           bloqueado,
           finalizado
    } state;
    PTR_DESC contexto;
    struct bcp *filaSemaforo;
    struct bcp *prox_bcp;
} BCP;

typedef BCP *PTR_BCP;

typedef struct
{
    int valor;               /* Valor do semaforo*/
    PTR_BCP fila_bloqueados; /* Fila de processos bloqueados*/
} SEMAFORO;

void far iniciaFilaProc(); /*Funcao para iniciar fila de processos que estao prontos para a execucao*/

void far criaProcessos(void far (*p_endereco)(), char p_nome[MAX_PROCESS_NAME]); /*Aloca BCP e insere no fim da fila de processos*/

void far terminaProcesso(); /*Encerra um processo*/

void far ativaEscalador(); /*Ativa o escalonador de processos*/

/*Funcoes para a utilizacao de semaforos*/

void far iniciaSemaforo(SEMAFORO *semaforo_usuario, int tam_semaforo); /*Inicia um semaforo*/

void far downSemaforo(SEMAFORO *semaforo_usuario); /*Down do semaforo*/

void far upSemaforo(SEMAFORO *semaforo_usuario); /*Up do semaforo*/
