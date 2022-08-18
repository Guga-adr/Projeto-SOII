#include <system.h>

#define MAX_PROCESS_NAME 30;
#define INTERRUPT 8;

typedef struct bcp {
    char name[MAX_PROCESS_NAME];
    enum { //estado do processo
        ativado,
        bloqueado,
        finalizado
    } state;
    PTR_DESC contexto;
    struct bcp *filaSemaforo;
    struct bcp *prox_bcp;
} BCP;

typedef struct {
    int valor; //Valor do semaforo
    PTR_BCP fila_bloqueados; //Fila de processos bloqueados
} SEMAFORO;

PTR_BCP cabeca_fila_proc; //Cabeca da fila de processos
PTR_DESC contexto_escal; //Contexto do escalonador de processos

typedef struct registradores_8086 {
    unsigned bx1, es1; // Registradoresde flag dos servicos do DOS
} REG_8086;

typedef union regiao_critica {
    REG_8086; // Resultado dos registradores BX e ES
    char far *valor_reg; // Ponteiro para resultado da flag do DOS
}PTR_CR

PTR_CR = crDOS; // Ponteiro para a regiao critica do DOS

/*Funcao para iniciar fila de processos que estao prontos para a execucao*/
void far iniciaFilaProc() {
    cabeca_fila_proc = NULL;
}

void far retornaDOS() {
    disable(); // Desabilita interrupcões
    setvect(INTERRUPT, p_est->int_anterior); //Retorna o estado da interrupcao para padrao
    enable(); // Reabilita as interrupcões
    exit(0); // Retorna o controle para o DOS
}

/*Aloca BCP e insere no fim da fila de processos*/
void far criaProcessos(void far (*p_endereco)(), char p_nome[MAX_PROCESS_NAME]) {
    /*Alocacao do BCP (malloc)*/
    PTR_BCP aloc_process = (PTR_BCP)malloc(sizeof(struct bcp));
    strcpy(aloc_process->name, p_nome);
    aloc_process->state = ativado;
    aloc_process->contexto = cria_desc();
    newprocess(p_endereco, aloc_process->contexto);
    aloc_process->filaSemaforo = NULL;
    aloc_process->prox_bcp = NULL;

    /*Insercao no fim da fila de processos ativos*/

    /*Se a fila estiver vazia*/
    if(cabeca_fila_proc == NULL) {
        //Aponta para o processo
        aloc_process->prox_bcp = aloc_process;
        //Coloca processo na fila
        cabeca_fila_proc = aloc_process;
    } else { //Insere o elemento no fim da fila.
        PTR_BCP aux = cabeca_fila_proc;
        /* percorre a fila de processos */
        while(aux-> prox_bcp != cabeca_fila_proc) {
            aux = aux->prox_bcp;
        }
        //Insere o processo no fim da fila
        aux->prox_bcp = aloc_process;
        aloc_process->prox_bcp = cabeca_fila_proc;
    } 
}

void far terminaProcesso() {
    disable();  //Desabilita interrupcões.
    cabeca_fila_proc->state = finalizado; //Coloca o processo como estado finalizado.
    enable(); // Reabilita as interrupcões.
    while(1); // Gasta a fatia de tempo restante daquele processo.
}

PTR_BCP retornaProxAtivo() {
    /*Percorre a fila de processos ativos*/
    PTR_BCP prox_ativado = cabeca_fila_proc->prox_bcp; /*Inicia a partir do próximo elemento*/
    while(prox_ativado != cabeca_fila_proc) {
        // Se tiver um processo ativo:
        if(prox_ativado->state == ativado) {
            return prox_ativado;
        }
        prox_ativado = prox_ativado->prox_bcp;
    }
    // Caso nao existam processos ativos:
    return NULL;
}

void far escalonador() {
    // Valores iniciais para p_est
    p_est->p_origem = contexto_escal; // Escalonador
    p_est->p_destino = cabeca_fila_proc->contexto; // Próximo processo pronto a ser executado
    p_est->num_vetor = INTERRUPT; // Bit de interrupcao (8)

    _AH = 0x34; // Inicia registrador AH, associado a um endereco de flag da pilha de 'servicos' do DOS.
    _AL=0x00; //Inicia o registrador AL com 0.

    geninterrupt(0x21); // Gera uma interrupcao associada ao endereco 0x21

    //Salva registrador  ES
    crDOS.reg.es1 = _ES;
    //Salva registrador  BX
    crDOS.reg.bx1 = _BX;

    //Controle de mudanca de processos

    while(1) {
        iotransfer(); // Da uma fatia de tempo para o processo rodar
        disable(); // Desabilita interrupcões para mudar de processo

        //Verificar se o processo esteja na regiao crítica do DOS
        //Para isso, o valor de flag de servicos eh utilizado por "valor_reg"

        if(!(*crDOS.valor_reg)) {
            //Retorna o próximo processo ativo (caso exista)
            if((cabeca_fila_proc = retornaProxAtivo()) == NULL) {
                //Caso nao exista, retorna o controle para o DOS
                retornaDOS();
            }

            //Caso o processo exista, eh colocado na fila de execucao pelo escalonador
            p_est->p_destino = cabeca_fila_proc->contexto;
        }

        enable() // Ativa interrupção novamente para o novo processo.

    }
}

void far ativaEscalonador() {
    //Cria os Descritores
    PTR_DESC aux_ativo = cria_desc();
    contexto_escal = cria_desc() 

    // Inicia processo do escalador no descritor associado
    newprocess(escalonador, contexto_escal);

    //Transfere o controle atual pra o escalonador
    transfer(aux_ativo, contexto_escal);
}

/*Utilização de Semáforos*/

// Iniciar semáforo
void far iniciaSemaforo(SEMAFORO *semaforo_usuario, int tam_semaforo) {
    semaforo_usuario->valor = tam_semaforo; // Inicia o valor com o tamanho fornecido pelo usuario
    semaforo_usuario->fila_semaforo = NULL; // Inicai a fila de processos bloqueadoos como vazia
}

// Primitiva P (Down) decrementa o valor do semafro (se for zero, processo -> fila de bloqueados)
void far downSemaforo(SEMAFORO *semaforo_usuario) {
    disable(); // Desabilita interrupcoes para manipular os semaforos

    /*Condicoes da primitiva P*/

    //Verifica se esta usando a regiao critica
    if(semaforo_usuario->value > 0) {
        // Decrementa o semaforo e nao bloqueia o processo
        semaforo_usuario->value--;
    } else {
        //Auxiliar de ponteiro BCP
        PTR_BCP p_aux;

        //Muda o estado do processo ativo para bloqueado
        cabeca_fila_proc->state = bloqueado;

        //Bloqueia o processo e adiciona na fila de bloqueados
        //Verifica se a fila de bloqueados esta vazia
        if(semaforo_usuario->fila_bloqueados == NULL) {
            // Se estiver vazia, insere o processo
            semaforo_usuario->fila_bloqueados = cabeca_fila_proc; // Insere o processo na fila de bloqueados
        } else {
            //Cria BCP auxiliar para percorrer a fila de bloqueados
            PTR_PCB p_aux = semaforo_usuario->fila_bloqueados;
            p_aux = semaforo_usuario->fila_bloqueados;

            //varre fila de processos bloqueados
            while(p_aux->filaSemaforo != NULL) {
                p_aux = p_aux->filaSemaforo;
            }
        }
    }
}



int far main() {

}