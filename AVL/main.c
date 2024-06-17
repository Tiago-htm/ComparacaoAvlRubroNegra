#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avl.h"

int main() {
    avl *arv;
    char nomeArquivo[30];
    int res;
    printf("Digite o nome do arquivo de inserção: ");
    scanf("%s", nomeArquivo);
    arv = criaArvore();
    res = processaCarga(arv, nomeArquivo, 1);
    if(res == -2)
    {
        printf("Erro ao abrir o arquivo de insercao");
        return 0;
    }
    if(res == 0)
    {
        printf("Erro na insercao");
        return 0;
    }
    imprimePreOrdem(getRaiz(arv));
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
    res = processaCarga(arv, nomeArquivo, 2);
    if(res == -2)
    {
        printf("Erro ao abrir o arquivo de remocao");
        return 0;
    }
    if(res == 0)
    {
        printf("Erro na remocao");
        return 0;
    }

    imprimePreOrdem(getRaiz(arv));
    printf("%d", getNumElementos(arv));
    imprimeMetricas();
    printf("Tempo total para buscar 1000 valores aleatórios: %ld.%09ld segundos\n",tempoBusca.tv_sec, tempoBusca.tv_nsec);
    free(arv);
    return 1;
}
