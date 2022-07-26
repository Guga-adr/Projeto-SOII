#include <stdio.h>
#include <system.h>

PTR_DESC dA, dB, desc, dmain;

void far co_rotina_A() {
    while (1)
        printf("Co-rotina A ");
}

void far co_rotina_B() {
    while (1)
        printf("Co-rotina B ");
}

void far esc() {
    p_est->p_origem = desc;
    p_est->p_destino = dA;
    p_est->num_vetor = 8;
    while (1) {
        iotransfer();
        disable();
        if (p_est->p_destino == dA)
            p_est->p_destino = dB;
        else
            p_est->p_destino = dA;
        system("pause");
        enable();
    }
}

void far main() {
    dA = cria_desc();
    dB = cria_desc();
    desc = cria_desc();
    dmain = cria_desc();
    newprocess(co_rotina_A, dA);
    newprocess(co_rotina_B, dB);
    newprocess(esc, desc);
    transfer(dmain, desc);
    printf("\n--ACABOU--\n");
    system("pause");
}