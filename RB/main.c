#include "rubroNegra.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    rb *arv;
    char nomeArquivo[30];

    arv = criaArvore();
    printf("Digite o nome do arquivo de inserção: ");
    scanf(" %s", nomeArquivo);
    processaCarga(arv, nomeArquivo, 1);
    imprimePreOrdem(arv, getRaiz(arv));
    printf("%d\n", getNumElementos(arv));

    /*------------------------------------------------------------------*/
    struct timespec inicioBusca, fimBusca;
    clock_gettime(CLOCK_MONOTONIC, &inicioBusca);

    for (int i = 0; i < 1000; i++) {
        int valor = rand() % 100000;
        buscaNo(arv, valor);
    }

    clock_gettime(CLOCK_MONOTONIC, &fimBusca);

    struct timespec tempoBusca = {
            fimBusca.tv_sec - inicioBusca.tv_sec,
            fimBusca.tv_nsec - inicioBusca.tv_nsec
    };

    if (tempoBusca.tv_nsec < 0) {
        tempoBusca.tv_sec -= 1;
        tempoBusca.tv_nsec += 1000000000;
    }
    /*------------------------------------------------------------------*/

    printf("Digite o nome do arquivo de remoção: ");
    scanf("%s", nomeArquivo);
    processaCarga(arv, nomeArquivo, 2);
    imprimePreOrdem(arv, getRaiz(arv));
    printf("%d\n", getNumElementos(arv));

    imprimeMetricas();
    printf("Tempo total para buscar 1000 valores aleatórios: %ld.%09ld segundos\n",tempoBusca.tv_sec, tempoBusca.tv_nsec);
    return 0;
}
