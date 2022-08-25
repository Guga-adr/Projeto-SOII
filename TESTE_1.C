#include <kernel.h>
#include <stdio.h>

void far iniciaProcess() {
    int i;
    for (i = 0; i < 100; i++) {
        printf ("Testando");
        i++;
    }
    terminaProcesso();
}

void far main() {
    
}