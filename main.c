#include <stdio.h>
#include <stdlib.h>
#include "jogo.h"
#define MAX_NOME 60
#define INIMIGOS 4
#define N_LINHA 11
#define N_COLUNA 27


int main(){
    time_t t;
    //Inicializamos a semente aqui
    srand((unsigned) time(&t));
    controlador_jogo();
    return 0;
}
