#include "rubroNegra.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct no {
    int chave;
    char cor;
    struct no *esq;
    struct no *dir;
    struct no *pai;
};

struct rb {
    struct no *raiz;
    int numElementos;
};

int numRotacoesInsercao = 0; // Contador de rotações de inserção
int numRotacoesRemocao = 0;  // Contador de rotações de remoção
struct timespec tempoTotalInsercao = {0, 0}; // Variável para medir o tempo total de inserção
struct timespec tempoTotalRemocao = {0, 0}; // Variável para medir o tempo total de remoção

rb *criaArvore() {
    rb *arv = (rb *) malloc(sizeof(rb));
    if (arv != NULL) {
        arv->raiz = NULL;
        arv->numElementos = 0;
    }
    return arv;
}

void somaTempo(struct timespec *t1, struct timespec *t2) {
    t1->tv_sec += t2->tv_sec;
    t1->tv_nsec += t2->tv_nsec;
    if (t1->tv_nsec >= 1000000000) {
        t1->tv_sec += 1;
        t1->tv_nsec -= 1000000000;
    }
}

int insereNo(rb *arv, int valor) {
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    if (arv == NULL)
        return 0;

    no *novoNo = (no *) malloc(sizeof(no));
    if (novoNo == NULL)
        return 0;

    novoNo->chave = valor;
    novoNo->cor = 'V';
    novoNo->esq = NULL;
    novoNo->dir = NULL;
    novoNo->pai = NULL;

    if (arv->raiz == NULL) {
        novoNo->cor = 'P';
        arv->raiz = novoNo;
        arv->numElementos++;
        clock_gettime(CLOCK_MONOTONIC, &fim);
        somaTempo(&tempoTotalInsercao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});
        return 1;
    }

    no *atual = arv->raiz;
    no *ant = NULL;

    while (atual != NULL) {
        ant = atual;

        if (valor == atual->chave) {
            free(novoNo);
            clock_gettime(CLOCK_MONOTONIC, &fim);
            somaTempo(&tempoTotalInsercao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});
            return 0; // Valor já existe na árvore
        } else if (valor < atual->chave) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }

    novoNo->pai = ant;
    if (valor < ant->chave) {
        ant->esq = novoNo;
    } else {
        ant->dir = novoNo;
    }

    arv->numElementos++;
    balanceamentoInsercao(arv, novoNo);

    clock_gettime(CLOCK_MONOTONIC, &fim);
    somaTempo(&tempoTotalInsercao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});
    return 1;
}

void balanceamentoInsercao(rb *arv, no *noDesbal) {
    int tipo = 1;

    while (noDesbal->pai != NULL && noDesbal->pai->cor == 'V') {
        if (noDesbal->pai == noDesbal->pai->pai->esq) {
            no *tio = noDesbal->pai->pai->dir;

            if (tio != NULL && tio->cor == 'V') {
                noDesbal->pai->cor = 'P';
                tio->cor = 'P';
                noDesbal->pai->pai->cor = 'V';
                noDesbal = noDesbal->pai->pai;
            } else {
                if (noDesbal == noDesbal->pai->dir) {
                    noDesbal = noDesbal->pai;
                    rotacaoEsq(arv, noDesbal, tipo);
                }

                noDesbal->pai->cor = 'P';
                noDesbal->pai->pai->cor = 'V';
                rotacaoDir(arv, noDesbal->pai->pai, tipo);
            }
        } else {
            no *tio = noDesbal->pai->pai->esq;

            if (tio != NULL && tio->cor == 'V') {
                noDesbal->pai->cor = 'P';
                tio->cor = 'P';
                noDesbal->pai->pai->cor = 'V';
                noDesbal = noDesbal->pai->pai;
            } else {
                if (noDesbal == noDesbal->pai->esq) {
                    noDesbal = noDesbal->pai;
                    rotacaoDir(arv, noDesbal, tipo);
                }

                noDesbal->pai->cor = 'P';
                noDesbal->pai->pai->cor = 'V';
                rotacaoEsq(arv, noDesbal->pai->pai, tipo);
            }
        }
    }

    arv->raiz->cor = 'P';
}

void rotacaoDir(rb *arv, no *noDesbal, int tipo) {
    if (tipo == 1) {
        numRotacoesInsercao++; // Incrementa o contador de rotações de inserção
    }else if(tipo == 2){
        numRotacoesRemocao++; // Incrementa o contador de rotações de remoção
    }

    no *filhoEsq = noDesbal->esq;
    noDesbal->esq = filhoEsq->dir;

    if (filhoEsq->dir != NULL) {
        filhoEsq->dir->pai = noDesbal;
    }

    filhoEsq->pai = noDesbal->pai;

    if (noDesbal->pai == NULL) {
        arv->raiz = filhoEsq;
    } else if (noDesbal == noDesbal->pai->esq) {
        noDesbal->pai->esq = filhoEsq;
    } else {
        noDesbal->pai->dir = filhoEsq;
    }

    filhoEsq->dir = noDesbal;
    noDesbal->pai = filhoEsq;
}

void rotacaoEsq(rb *arv, no *noDesbal, int tipo) {
    if (tipo == 1) {
        numRotacoesInsercao++; // Incrementa o contador de rotações de inserção
    }else if(tipo == 2){
        numRotacoesRemocao++; // Incrementa o contador de rotações de remoção
    }

    no *filhoDir = noDesbal->dir;
    noDesbal->dir = filhoDir->esq;

    if (filhoDir->esq != NULL) {
        filhoDir->esq->pai = noDesbal;
    }

    filhoDir->pai = noDesbal->pai;

    if (noDesbal->pai == NULL) {
        arv->raiz = filhoDir;
    } else if (noDesbal == noDesbal->pai->esq) {
        noDesbal->pai->esq = filhoDir;
    } else {
        noDesbal->pai->dir = filhoDir;
    }

    filhoDir->esq = noDesbal;
    noDesbal->pai = filhoDir;
}

int removeNo(rb *arv, int valor) {
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    if (arv == NULL || arv->raiz == NULL)
        return 0;

    no *noRemover = arv->raiz;
    while (noRemover != NULL && noRemover->chave != valor) {
        if (valor < noRemover->chave)
            noRemover = noRemover->esq;
        else
            noRemover = noRemover->dir;
    }

    if (noRemover == NULL) {
        clock_gettime(CLOCK_MONOTONIC, &fim);
        somaTempo(&tempoTotalRemocao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});
        return -1; // Valor não encontrado
    }

    if (noRemover->esq != NULL && noRemover->dir != NULL) {
        no *sucessor = noRemover->dir;
        while (sucessor->esq != NULL)
            sucessor = sucessor->esq;

        noRemover->chave = sucessor->chave;
        noRemover = sucessor;
    }

    no *filho = (noRemover->esq != NULL) ? noRemover->esq : noRemover->dir;

    if (noRemover->cor == 'P') {
        if (filho == NULL || filho->cor == 'P') {
            balanceamentoRemocao(arv, noRemover, noRemover->pai, valor);
        } else {
            filho->cor = 'P';
        }
    }

    if (noRemover->pai == NULL) {
        arv->raiz = filho;
        if (filho != NULL) {
            filho->pai = NULL;
        }
    } else {
        if (noRemover == noRemover->pai->esq)
            noRemover->pai->esq = filho;
        else
            noRemover->pai->dir = filho;

        if (filho != NULL)
            filho->pai = noRemover->pai;
    }

    free(noRemover);
    arv->numElementos--;

    clock_gettime(CLOCK_MONOTONIC, &fim);
    somaTempo(&tempoTotalRemocao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});
    return 1;
}


void balanceamentoRemocao(rb *arv, no *noDesbal, no *pai, int valor) {
    int tipo = 2;

    while (noDesbal != arv->raiz && (noDesbal == NULL || noDesbal->cor == 'P')) {
        if (noDesbal == pai->esq) {
            no *irmao = pai->dir;

            if (irmao->cor == 'V') {
                irmao->cor = 'P';
                pai->cor = 'V';
                rotacaoEsq(arv, pai, tipo);
                irmao = pai->dir;
            }

            if ((irmao->esq == NULL || irmao->esq->cor == 'P') && (irmao->dir == NULL || irmao->dir->cor == 'P')) {
                irmao->cor = 'V';
                noDesbal = pai;
                pai = noDesbal->pai;
            } else {
                if (irmao->dir == NULL || irmao->dir->cor == 'P') {
                    irmao->esq->cor = 'P';
                    irmao->cor = 'V';
                    rotacaoDir(arv, irmao, tipo);
                    irmao = pai->dir;
                }

                irmao->cor = pai->cor;
                pai->cor = 'P';
                if (irmao->dir != NULL) {
                    irmao->dir->cor = 'P';
                }
                rotacaoEsq(arv, pai, tipo);
                noDesbal = arv->raiz;
            }
        } else {
            no *irmao = pai->esq;

            if (irmao->cor == 'V') {
                irmao->cor = 'P';
                pai->cor = 'V';
                rotacaoDir(arv, pai, tipo);
                irmao = pai->esq;
            }

            if ((irmao->dir == NULL || irmao->dir->cor == 'P') && (irmao->esq == NULL || irmao->esq->cor == 'P')) {
                irmao->cor = 'V';
                noDesbal = pai;
                pai = noDesbal->pai;
            } else {
                if (irmao->esq == NULL || irmao->esq->cor == 'P') {
                    irmao->dir->cor = 'P';
                    irmao->cor = 'V';
                    rotacaoEsq(arv, irmao, tipo);
                    irmao = pai->esq;
                }

                irmao->cor = pai->cor;
                pai->cor = 'P';
                if (irmao->esq != NULL) {
                    irmao->esq->cor = 'P';
                }
                rotacaoDir(arv, pai, tipo);
                noDesbal = arv->raiz;
            }
        }
    }

    if (noDesbal != NULL) {
        noDesbal->cor = 'P';
    }
}


void imprimeOrdem(no *raiz) {
    if (raiz != NULL) {
        imprimeOrdem(raiz->esq);
        printf("%d: %c -- %d\n", raiz->chave, raiz->cor, raiz->pai ? raiz->pai->chave : -1);
        imprimeOrdem(raiz->dir);
    }
}

void imprimePreOrdem(rb *arv, no *raiz) {
    if (raiz != NULL) {
        printf("%d: %c -- %d\n", raiz->chave, raiz->cor, raiz->pai != NULL ? raiz->pai->chave : -1000);
        imprimePreOrdem(arv, raiz->esq);
        imprimePreOrdem(arv, raiz->dir);
    }
}

no *getRaiz(rb *arv) {
    if (arv != NULL)
        return arv->raiz;
    return NULL;
}

int getNumElementos(rb *arv) {
    if (arv != NULL)
        return arv->numElementos;
    return 0;
}

int processaCarga(rb *arv, char *nomeArquivo, int tipo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL)
        return -2;

    int valor;
    int resultado = 0;

    if (tipo == 1) {
        while (fscanf(arquivo, "%d", &valor) != EOF) {
            resultado = insereNo(arv, valor);
            if (resultado == 0)
                break;
        }
    } else if (tipo == 2) {
        while (fscanf(arquivo, "%d", &valor) != EOF) {
            resultado = removeNo(arv, valor);
            if (resultado == -1)
                break;
        }
    }

    fclose(arquivo);
    return resultado;
}

no *buscaNo(rb *arv, int valor) {
    no *atual = arv->raiz;
    while (atual != NULL) {
        if (valor == atual->chave) {
            return atual;
        } else if (valor < atual->chave) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }
    return NULL; // Valor não encontrado
}


void imprimeMetricas() {
    printf("Número total de rotações (inserção): %d\n", numRotacoesInsercao);
    printf("Número total de rotações (remoção): %d\n", numRotacoesRemocao);
    printf("Tempo total gasto em inserções: %ld.%09ld segundos\n", tempoTotalInsercao.tv_sec, tempoTotalInsercao.tv_nsec);
    printf("Tempo total gasto em remoções: %ld.%09ld segundos\n", tempoTotalRemocao.tv_sec, tempoTotalRemocao.tv_nsec);
}