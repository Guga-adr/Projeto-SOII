/* Implementation of some processes, with simple printf function,
for testing the Kernel Scheduler */

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TIME 10000

void far process1() {
    int i = 0;
    while (i < MAX_TIME) {
        printf("1\n");
        i++;
    }

    /* Terminando o Processo...*/
    terminaProcesso();
}

void far process2() {
    int i = 0;
    while (i < MAX_TIME) {
        printf("2\n");
        i++;
    }

    /* Terminando o Processo...*/
    terminaProcesso();
}

void far process3() {
    int i = 0;
    while (i < MAX_TIME) {
        printf("3\n");
        i++;
    }

    /* Terminando o Processo...*/
    terminaProcesso();
}

void far process4() {
    int i = 0;
    while (i < MAX_TIME) {
        printf("4\n");
        i++;
    }

    /* Terminando o Processo...*/
    terminaProcesso();
}

void far process5() {
    int i = 0;
    while (i < MAX_TIME) {
        printf("5\n");
        i++;
    }

    /* Terminando o Processo...*/
    terminaProcesso();
}

void main() {
    /* Iniciando a fila de processos */
    iniciaFilaProc();

    /* Criacao dos Processos */
    criaProcessos(process1, "P1");
    criaProcessos(process2, "P2");
    criaProcessos(process3, "P3");
    criaProcessos(process4, "P4");
    criaProcessos(process5, "P5");

    /* Transferindo o controle para o Escalonador */
    ativaEscalador();
}