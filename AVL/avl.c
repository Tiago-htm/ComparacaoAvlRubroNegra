#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avl.h"

typedef struct no {
    int chave;
    int fb;
    struct no *Fesq;
    struct no *Fdir;
    struct no *pai;
} no;

typedef struct avl {
    struct no *sentinela;
    int numElementos;
} avl;

int numRotacoesInsercao = 0; // Contador de rotações de inserção
int numRotacoesRemocao = 0;  // Contador de rotações de remoção
struct timespec tempoTotalInsercao = {0, 0}; // Variável para medir o tempo total de inserção
struct timespec tempoTotalRemocao = {0, 0}; // Variável para medir o tempo total de remoção

avl *criaArvore() {
    avl* arv = (avl*) malloc(sizeof(avl));
    if(arv != NULL) {
        arv->numElementos = 0;
        no* sent = (no*) malloc(sizeof(no));
        sent->chave = -1000;
        sent->Fdir = NULL;
        sent->Fesq = NULL;
        sent->pai = NULL;
        arv->sentinela = sent;
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

int insereNo(avl *arv, int valor) {
    struct timespec inicio, fim, duracao;
    clock_gettime(CLOCK_MONOTONIC, &inicio); // Início da medição do tempo

    if(arv == NULL)
        return 0;

    no *novoNo = (no*) malloc(sizeof(no));
    novoNo->chave = valor;
    novoNo->fb = 0;
    novoNo->Fdir = NULL;
    novoNo->Fesq = NULL;
    novoNo->pai = NULL;

    if(arv->sentinela->Fdir == NULL) {
        arv->sentinela->Fdir = novoNo;
        novoNo->pai = arv->sentinela;
        arv->numElementos++;
        clock_gettime(CLOCK_MONOTONIC, &fim);
        somaTempo(&tempoTotalInsercao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});

        return 1;
    } else {
        no *atual = arv->sentinela->Fdir;
        no *anterior = atual->pai;

        while(atual != NULL) {
            anterior = atual;
            if(valor < atual->chave) {
                atual = atual->Fesq;
            } else {
                atual = atual->Fdir;
            }
        }
        if(valor < anterior->chave) {
            anterior->Fesq = novoNo;
            novoNo->pai = anterior;
        } else {
            anterior->Fdir = novoNo;
            novoNo->pai = anterior;
        }
    }

    arv->numElementos++;
    atualizaFB_insercao(arv, novoNo);

    clock_gettime(CLOCK_MONOTONIC, &fim);
    somaTempo(&tempoTotalInsercao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});

    return 1;
}

int removeNo(avl *arv, int valor) {
    struct timespec inicio, fim, duracao;
    clock_gettime(CLOCK_MONOTONIC, &inicio); // Início da medição do tempo

    no *pai = NULL;
    no *atual = arv->sentinela;

    if (atual == NULL) {
        return 0;
    }
    while (atual != NULL && atual->chave != valor) {
        pai = atual;
        if (valor < atual->chave) {
            atual = atual->Fesq;
        } else {
            atual = atual->Fdir;
        }
    }
    if (atual == NULL) {
        clock_gettime(CLOCK_MONOTONIC, &fim);
        somaTempo(&tempoTotalRemocao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});
        return -1;
    }
    if (atual->Fesq == NULL && atual->Fdir == NULL) {
        if (pai == NULL) {
            arv->sentinela = NULL;
        } else if (atual == pai->Fesq) {
            pai->Fesq = NULL;
        } else {
            pai->Fdir = NULL;
        }
        atualizaFB_remocao(arv, pai, atual->chave);
        free(atual);
    } else if (atual->Fesq == NULL || atual->Fdir == NULL) {
        no *ramo;
        if (atual->Fesq == NULL) {
            ramo = atual->Fdir;
            ramo->pai = pai;
        } else {
            ramo = atual->Fesq;
            ramo->pai = pai;
        }
        if (pai == NULL) {
            arv->sentinela = ramo;
        } else if (atual == pai->Fesq) {
            pai->Fesq = ramo;
        } else {
            pai->Fdir = ramo;
        }
        atualizaFB_remocao(arv, pai, atual->chave);
        free(atual);
    } else {
        no *proximo = atual->Fdir;
        pai = atual;
        while (proximo->Fesq != NULL) {
            pai = proximo;
            proximo = proximo->Fesq;
        }
        atual->chave = proximo->chave;
        if (pai->Fesq == proximo) {
            pai->Fesq = proximo->Fdir;
            if (proximo->Fdir != NULL) {
                proximo->Fdir->pai = atual;
            }
        } else {
            pai->Fdir = proximo->Fdir;
            if (proximo->Fdir != NULL) {
                proximo->Fdir->pai = atual;
            }
        }

        atualizaFB_remocao(arv, proximo->pai, proximo->chave);
        free(proximo);
        proximo = NULL;
    }
    arv->numElementos--;

    clock_gettime(CLOCK_MONOTONIC, &fim);
    somaTempo(&tempoTotalRemocao, &(struct timespec){fim.tv_sec - inicio.tv_sec, fim.tv_nsec - inicio.tv_nsec});
    return 1;
}

void imprimeOrdem(no *raiz) {
    if(raiz == NULL)
        return;

    if(raiz != NULL) {
        printf("%d - %d - %d\n", raiz->chave, raiz->pai->chave, raiz->fb);
        imprimeOrdem(raiz->Fesq);
        imprimeOrdem(raiz->Fdir);
    }
}

void imprimePreOrdem(no *raiz) {
    if(raiz == NULL)
        return;

    if(raiz->pai == NULL) {
        imprimePreOrdem(raiz->Fesq);
        imprimePreOrdem(raiz->Fdir);
    } else if(raiz->pai != NULL) {
        printf("%d - %d - %d\n", raiz->chave, raiz->pai->chave, raiz->fb);
        imprimePreOrdem(raiz->Fesq);
        imprimePreOrdem(raiz->Fdir);
    }
}

no *getRaiz(avl *arv) {
    return arv->sentinela;
}

int getNumElementos(avl *arv) {
    return arv->numElementos;
}

int processaCarga(avl *arv, char *nomeArquivo, int tipo) {
    FILE *arquivo;
    int insere;

    arquivo = fopen(nomeArquivo, "r");

    if(arquivo == NULL) {
        return -2;
    }
    if (tipo == 1) {
        if(arv == NULL) {
            fclose(arquivo); // Fechar o arquivo se a árvore for NULL
            return 0;
        }

        while (fscanf(arquivo, "%d", &insere) == 1) {
            insereNo(arv, insere);
        }
    } else {
        while (fscanf(arquivo, "%d", &insere) == 1) {
            removeNo(arv, insere);
        }
    }

    fclose(arquivo); // Fechar o arquivo após a leitura
    return 1;
}

void atualizaFB_insercao(avl *arv, no *novoNo) {
    no *aux = novoNo->pai;

    while(aux != arv->sentinela) {
        if(novoNo->chave < aux->chave) {
            aux->fb--;
        } else {
            aux->fb++;
        }
        if(aux->fb == 0) {
            return;
        }
        if(aux->fb == -2 || aux->fb == 2) {
            balanceamento(arv, aux, 1); // Indica que é uma inserção
            return;
        }
        aux = aux->pai;
    }
}

void balanceamento(avl *arv, no *noDesbalanceado, int tipo) {
    no *filho;

    if(noDesbalanceado->fb == -2) {
        filho = noDesbalanceado->Fesq;
        if(filho->fb == 1) {
            int fbNeto = filho->Fdir->fb;
            rotacaoEsq(filho, tipo);
            rotacaoDir(noDesbalanceado, tipo);

            if(fbNeto == 0) {
                noDesbalanceado->fb = 0;
                filho->fb = 0;
            } else if(fbNeto == 1) {
                noDesbalanceado->fb = 0;
                filho->fb = -1;
                noDesbalanceado->pai->fb = 0;
            } else {
                noDesbalanceado->fb = 0;
                filho->fb = 1;
                noDesbalanceado->pai->fb = 0;
            }
        } else {
            int fbFilho = filho->fb;
            rotacaoDir(noDesbalanceado, tipo);
            if(fbFilho != 0) {
                filho->fb = 0;
                noDesbalanceado->fb = 0;
            } else {
                filho->fb = 1;
                noDesbalanceado->fb = -1;
            }
        }
    }

    if(noDesbalanceado->fb == 2) {
        filho = noDesbalanceado->Fdir;
        if(filho->fb == -1) {
            int fbNeto = filho->Fesq->fb;
            rotacaoDir(filho, tipo);
            rotacaoEsq(noDesbalanceado, tipo);

            if(fbNeto == 0) {
                noDesbalanceado->fb = 0;
                filho->fb = 0;
            } else if(fbNeto == -1) {
                noDesbalanceado->fb = 0;
                filho->fb = 1;
                noDesbalanceado->pai->fb = 0;
            } else {
                noDesbalanceado->fb = -1;
                filho->fb = 0;
                noDesbalanceado->pai->fb = 0;
            }
        } else {
            int fbFilho = filho->fb;
            rotacaoEsq(noDesbalanceado, tipo);
            if(fbFilho != 0) {
                filho->fb = 0;
                noDesbalanceado->fb = 0;
            } else {
                filho->fb = -1;
                noDesbalanceado->fb = 1;
            }
        }
    }
}

void rotacaoEsq(no *noDesbal, int tipo) {
    if(tipo == 1) {
        numRotacoesInsercao++; // Incrementa o contador de rotações de inserção
    } else {
        numRotacoesRemocao++; // Incrementa o contador de rotações de remoção
    }

    no *y = noDesbal->Fdir;
    noDesbal->Fdir = y->Fesq;
    if(y->Fesq != NULL) {
        y->Fesq->pai = noDesbal;
    }
    y->pai = noDesbal->pai;
    if (noDesbal == noDesbal->pai->Fesq) {
        noDesbal->pai->Fesq = y;
    } else {
        noDesbal->pai->Fdir = y;
    }
    y->Fesq = noDesbal;
    noDesbal->pai = y;
}

void rotacaoDir(no *noDesbal, int tipo) {
    if(tipo == 1) {
        numRotacoesInsercao++; // Incrementa o contador de rotações de inserção
    } else {
        numRotacoesRemocao++; // Incrementa o contador de rotações de remoção
    }

    no *x = noDesbal->Fesq;
    noDesbal->Fesq = x->Fdir;
    if(x->Fdir != NULL) {
        x->Fdir->pai = noDesbal;
    }
    x->pai = noDesbal->pai;
    if (noDesbal == noDesbal->pai->Fdir) {
        noDesbal->pai->Fdir = x;
    } else {
        noDesbal->pai->Fesq = x;
    }
    x->Fdir = noDesbal;
    noDesbal->pai = x;
}

void atualizaFB_remocao(avl *arv, no *pai, int chaveRemovida) {
    if(pai == arv->sentinela) return;

    no *aux = pai;

    if(chaveRemovida < aux->chave) {
        aux->fb++;
    } else{
        aux->fb--;
    }

    while(aux != arv->sentinela->Fdir && aux->fb == 0){
        aux = aux->pai;

        if(chaveRemovida < aux->chave){
            aux->fb++;
        } else {
            aux->fb--;
        }
    }

    if(aux->fb == 2 || aux->fb == -2){
        balanceamento(arv, aux, 2); // Indica que é uma remoção
        if((aux->pai != arv->sentinela) && (aux->pai->fb == 0)){
            atualizaFB_remocao(arv, aux->pai->pai, chaveRemovida);
        }
    }
}

no *buscaNo(avl *arv, int valor) {
    no *atual = arv->sentinela->Fdir;
    while (atual != NULL) {
        if (valor == atual->chave) {
            return atual;
        } else if (valor < atual->chave) {
            atual = atual->Fesq;
        } else {
            atual = atual->Fdir;
        }
    }
    return NULL; // Valor não encontrado
}

// Função para imprimir os resultados das métricas
void imprimeMetricas() {
    printf("Número total de rotações (inserção): %d\n", numRotacoesInsercao);
    printf("Número total de rotações (remoção): %d\n", numRotacoesRemocao);
    printf("Tempo total gasto em inserções: %ld.%09ld segundos\n", tempoTotalInsercao.tv_sec, tempoTotalInsercao.tv_nsec);
    printf("Tempo total gasto em remoções: %ld.%09ld segundos\n", tempoTotalRemocao.tv_sec, tempoTotalRemocao.tv_nsec);
}
