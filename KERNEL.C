#include <system.h>

#define MAX_PROCESS_NAME 30;
#define INTERRUPT 8;

typedef struct bcp {
    char name[MAX_PROCESS_NAME];
    enum {
        ativado,
        bloqueado,
        finalizado
    } state;
    PTR_DESC contexto;
    struct bcp *FilaSemaforo;
    struct bcp *prox_bcp;
} BCP;

int main() {

}