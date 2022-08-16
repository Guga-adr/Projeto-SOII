# include <system.h>
# include <stdio.h>

PTR_DESC dtic, dtac, dmain;
void far tic() {
     int i = 0;
     for(i = 0; i < 100; i++) {
         printf("%d tic ", i+1);
         transfer(dtic, dtac);
         i++;
     }
     transfer(dtic, dmain);
}
void far tac() {
     while(1) {
         printf("tac \t");
         transfer(dtac, dtic);
     }
}

void far main()  {
     dtic = cria_desc();
     dtac = cria_desc();
     dmain = cria_desc();
     newprocess(tic, dtic);
     newprocess(tac, dtac);
     transfer(dmain, dtic);
     printf("\n--ACABOU--\n");
     system("pause");
}