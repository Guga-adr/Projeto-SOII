/* Implemetation of produtor/consumidor Problem, for Semaphore Test */

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ITERATIONS 1000
#define MAX_ITEMS 80

/* Definicao dos Semaforos (Kernel.H) */
SEMAFORO mutex; /* Define o acesso a regiao critica */
SEMAFORO empty; /* Definida para contar os buffers vazios */
SEMAFORO full;  /* Definida para contar os buffers com informacao */

/* Definicao do Arquivo de Saida (Resultados) */
FILE *filePC;

/* Variaveis globais */
int buffer[MAX_ITEMS]; /* Buffer do Produtor/Consumidor */
int posP, posC;        /* Posicao atual do Produtor e Consumidor */
int val = 1;           /* Auxiliar para armazenar no buffer */

/* Funcao auxiliar para criacao e inicializacao do arquivo de saida */
void far iniciaArquivo(){
  /* Abrindo apenas com leitura */
  if (!(filePC = fopen("TK-02.txt", "w"))){
    printf("Erro ao abrir/criar o arquivo.\n");
    system("Pause");
    exit(0);
  }
}

/* Funcao auxiliar do Produtor: produzir um item (aumentar o valor) */
int far produzItem(){
  val += 2;
  return val;
}

/* Funcao auxiliar do Produtor: inserir item produzido no buffer */
void far insereItem(int item1){
  /* Somente insere em uma posicao vazia do buffer */
  while (buffer[posP] != -1){
    posP++; /* Avanca a posicao */

    /* Se chegar no indice chegar ao final do buffer, ele volta para o slot inicial do buffer, pois se trata de uma lista circular */
    if (posP == MAX_ITEMS)
      posP = 0; /* Posicao do slot inicial do buffer */
  }
  buffer[posP++] = item1; /* Insere o item no buffer e avanca uma posicao */

  /* Se chegar no indice chegar ao final do buffer, ele volta para o slot inicial do buffer, pois se trata de uma lista circular */
  if (posP == MAX_ITEMS)
    posP = 0; /* Posicao do slot inicial do buffer */
}

/* Funcao auxiliar do Consumidor: retirar item do buffer */
int far consomeItem(){
  int item2; /* Auxiliar para resgatar item do buffer */

  while (buffer[posC] == -1){
    posC++; /* Avanca a posicao */

    /* Se chegar no indice do final do buffer, ele volta para o slot inicial do buffer, pois se trata de uma lista circular */
    if (posC == MAX_ITEMS)
      posC = 0; /* Posicao do slot inicial do buffer */
  }
  item2 = buffer[posC]; /* Retira item do buffer */

  buffer[posC++] = -1; /* Remove item do buffer e avanca uma posicao */

  /* Se chegar no indice do final do buffer, ele volta para o slot inicial do buffer, pois se trata de uma lista circular */
  if (posC == MAX_ITEMS)
    posC = 0; /* Posicao do slot inicial do buffer */

  return item2; /* Retorna o item consumido para mostrar na tela/arquivo */
}

/* Co-rotina do Produtor, com utilizacao dos semaforos e limitacao em iteracoes */
void far produtor(){
  /* Variaveis auxiliares: valor do item do Produtor e limitacao de iteracoes */
  int itemP, i;

  /* Inicializacao de variaveis locais */
  posP = 0; /* Posicao atual do Produtor no buffer */
  i = 0;

  while (i < MAX_ITERATIONS){
    itemP = produzItem(); /* Realiza a producao do item */

    downSemaforo(&empty); /* Verifica se o buffer possui slots vazios */
    downSemaforo(&mutex); /* Se sim o produtor entra na regiao critica para inserir o item no slot */

    insertItem(itemP); /* Insere o item no slot vazio do buffer */

    /* Insere no arquivo, de maneira formatada */
    fprintf(filePC, "\nProdutor depositou no buffer[%d] = %d", posP, itemP);

    /* Mostrando o resultado na tela */
    /*printf("\nP-buffer[%d] = %d", posP, itemP);*/

    upSemaforo(&mutex); /* Sair da regiao critica */
    upSemaforo(&full);  /* Verifica se existem slots com item. Se existirem desbloqueia o processo Consumidor caso ele esteja bloqueado.*/

    i++;
  }

  /* Terminando o Processo...*/
  terminaProcesso();
}

/* Co-rotina do Consumidor, com utilizacao dos semaforos e limitacao em iteracoes */
void far consumidor(){
  /* Variaveis auxiliares: valor do item do Consumidor e limitacao de iteracoes */
  int itemC, i;

  /* Inicializacao de variaveis locais */
  posC = 0; /* Posicao atual do Consumidor no buffer */
  i = 0;

  while (i < MAX_ITERATIONS){
    downSemaforo(&full);  /* Verifica se o buffer possui slots com conteudo */
    downSemaforo(&mutex); /* Se sim o consumidor entra na regiao critica para consumir o item do slot */

    itemC = consomeItem();

    /* Insere no arquivo, de maneira formatada */
    fprintf(filePC, "\nConsumidor retirou do buffer[%d] = %d", posC, itemC);

    /* Mostrando o resultado na tela */
    /*printf("\nC-buffer[%d] = %d", posC, itemC);*/

    upSemaforo(&mutex); /* Sair da regiao critica */
    upSemaforo(&empty); /* Verifica se existem slots vazios. Se existirem desbloqueia o processo Produtor caso ele esteja bloqueado. */

    i++;
  }

  /* Terminando o Processo...*/
  terminaProcesso();
}

main(){
  /* Iniciando a Fila de Processos como Vazia */
  iniciaFilaProc();

  /* Inicializacao dos Semaforos */
  iniciaSemaforo(&mutex, 1);
  iniciaSemaforo(&empty, MAX_ITEMS);
  iniciaSemaforo(&full, 0);

  /* Criacao dos Processos */
  criaProcessos(produtor, "Produtor");
  criaProcessos(consumidor, "Consumidor");

  /* Inicializacao do buffer com valor -1 (vazio) */
  memset(buffer, -1, sizeof(buffer));

  /* Realiza a abertura/criacao do arquivo texto para resultados finais */
  iniciaArquivo();

  /* Transferindo o controle para o Escalonador */
  activateScheduler();
}