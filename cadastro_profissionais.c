#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "profissionais.csv"

typedef struct {
    int matricula;
    char nome[50];
    char cargo[30];
    char unidade[30];
    int cargaHoraria;
} Profissional;

typedef struct No {
    Profissional dados;
    struct No *esq;
    struct No *dir;
} No;

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int lerNumero(void) {
    int valor;
    int lidos = scanf("%d", &valor);
    if (lidos == EOF) {
        return 0;
    }
    if (lidos != 1) {
        limparBuffer();
        return -1;
    }
    limparBuffer();
    return valor;
}

int textoValido(char texto[]) {
    if (strchr(texto, ';') != NULL) {
        return 0;
    }
    return 1;
}

No* criarNo(Profissional p) {
    No *novo = (No*) malloc(sizeof(No));
    if (novo == NULL) {
        printf("erro: sem memoria para criar o no\n");
        exit(1);
    }
    novo->dados = p;
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

No* inserirNo(No *raiz, Profissional p, int *inseriu) {
    if (raiz == NULL) {
        *inseriu = 1;
        return criarNo(p);
    }
    if (p.matricula < raiz->dados.matricula) {
        raiz->esq = inserirNo(raiz->esq, p, inseriu);
    } else if (p.matricula > raiz->dados.matricula) {
        raiz->dir = inserirNo(raiz->dir, p, inseriu);
    } else {
        *inseriu = 0;
    }
    return raiz;
}

No* buscarNo(No *raiz, int matricula) {
    if (raiz == NULL) {
        return NULL;
    }
    if (matricula == raiz->dados.matricula) {
        return raiz;
    }
    if (matricula < raiz->dados.matricula) {
        return buscarNo(raiz->esq, matricula);
    }
    return buscarNo(raiz->dir, matricula);
}

void mostrarProfissional(Profissional p) {
    printf("Matricula: %d\n", p.matricula);
    printf("Nome: %s\n", p.nome);
    printf("Cargo: %s\n", p.cargo);
    printf("Unidade: %s\n", p.unidade);
    printf("Carga horaria: %d horas semanais\n\n", p.cargaHoraria);
}

void listarEmOrdem(No *raiz) {
    if (raiz == NULL) {
        return;
    }
    listarEmOrdem(raiz->esq);
    mostrarProfissional(raiz->dados);
    listarEmOrdem(raiz->dir);
}

void exibirArvore(No *raiz, int nivel, char lado) {
    int i;
    if (raiz == NULL) {
        return;
    }
    for (i = 0; i < nivel; i++) {
        printf("    ");
    }
    if (lado == 'R') {
        printf("Raiz: ");
    } else if (lado == 'E') {
        printf("E: ");
    } else {
        printf("D: ");
    }
    printf("%d - %s\n", raiz->dados.matricula, raiz->dados.nome);
    exibirArvore(raiz->esq, nivel + 1, 'E');
    exibirArvore(raiz->dir, nivel + 1, 'D');
}

int contarNos(No *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return 1 + contarNos(raiz->esq) + contarNos(raiz->dir);
}

No* removerNo(No *raiz, int matricula, int *removeu) {
    No *aux;
    No *sucessor;

    if (raiz == NULL) {
        return NULL;
    }
    if (matricula < raiz->dados.matricula) {
        raiz->esq = removerNo(raiz->esq, matricula, removeu);
    } else if (matricula > raiz->dados.matricula) {
        raiz->dir = removerNo(raiz->dir, matricula, removeu);
    } else {
        *removeu = 1;
        if (raiz->esq == NULL && raiz->dir == NULL) {
            free(raiz);
            return NULL;
        }
        if (raiz->esq == NULL) {
            aux = raiz->dir;
            free(raiz);
            return aux;
        }
        if (raiz->dir == NULL) {
            aux = raiz->esq;
            free(raiz);
            return aux;
        }
        sucessor = raiz->dir;
        while (sucessor->esq != NULL) {
            sucessor = sucessor->esq;
        }
        raiz->dados = sucessor->dados;
        raiz->dir = removerNo(raiz->dir, sucessor->dados.matricula, removeu);
    }
    return raiz;
}

void liberarArvore(No *raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarArvore(raiz->esq);
    liberarArvore(raiz->dir);
    free(raiz);
}

void gravarNos(No *raiz, FILE *arquivo) {
    if (raiz == NULL) {
        return;
    }
    fprintf(arquivo, "%d;%s;%s;%s;%d\n",
            raiz->dados.matricula, raiz->dados.nome, raiz->dados.cargo,
            raiz->dados.unidade, raiz->dados.cargaHoraria);
    gravarNos(raiz->esq, arquivo);
    gravarNos(raiz->dir, arquivo);
}

void gravarArquivo(No *raiz) {
    FILE *arquivo = fopen(ARQUIVO, "w");
    if (arquivo == NULL) {
        printf("erro: nao foi possivel abrir o arquivo para gravar\n");
        return;
    }
    gravarNos(raiz, arquivo);
    fclose(arquivo);
}

No* lerArquivo(void) {
    No *raiz = NULL;
    Profissional p;
    int inseriu;
    FILE *arquivo = fopen(ARQUIVO, "r");

    if (arquivo == NULL) {
        return NULL;
    }
    while (fscanf(arquivo, "%d;%49[^;];%29[^;];%29[^;];%d\n",
                  &p.matricula, p.nome, p.cargo, p.unidade, &p.cargaHoraria) == 5) {
        raiz = inserirNo(raiz, p, &inseriu);
    }
    fclose(arquivo);
    return raiz;
}



No* cadastrar(No *raiz) {
    Profissional p;
    int inseriu;

    printf("Matricula (numero maior que zero): ");
    p.matricula = lerNumero();
    if (p.matricula <= 0) {
        printf("Matricula invalida.\n");
        return raiz;
    }

    printf("Nome: ");
    if (scanf(" %49[^\n]", p.nome) != 1) {
        printf("Nome invalido. Cadastro cancelado.\n");
        return raiz;
    }
    limparBuffer();

    printf("Cargo: ");
    if (scanf(" %29[^\n]", p.cargo) != 1) {
        printf("Cargo invalido. Cadastro cancelado.\n");
        return raiz;
    }
    limparBuffer();

    printf("Unidade: ");
    if (scanf(" %29[^\n]", p.unidade) != 1) {
        printf("Unidade invalida. Cadastro cancelado.\n");
        return raiz;
    }
    limparBuffer();

    if (!textoValido(p.nome) || !textoValido(p.cargo) || !textoValido(p.unidade)) {
        printf("Nao use ponto e virgula nos textos. Cadastro cancelado.\n");
        return raiz;
    }

    printf("Carga horaria semanal: ");
    p.cargaHoraria = lerNumero();
    if (p.cargaHoraria <= 0) {
        printf("Carga horaria invalida. Cadastro cancelado.\n");
        return raiz;
    }

    raiz = inserirNo(raiz, p, &inseriu);
    if (inseriu) {
        printf("Profissional cadastrado!\n");
    } else {
        printf("Ja existe um profissional com essa matricula. Cadastro recusado.\n");
    }
    return raiz;
}

void buscarProfissional(No *raiz) {
    int matricula;
    No *achado;

    printf("Matricula a buscar: ");
    matricula = lerNumero();
    achado = buscarNo(raiz, matricula);
    if (achado == NULL) {
        printf("Matricula nao encontrada.\n");
    } else {
        mostrarProfissional(achado->dados);
    }
}

No* removerProfissional(No *raiz) {
    int matricula;
    int removeu = 0;

    printf("Matricula a remover: ");
    matricula = lerNumero();
    raiz = removerNo(raiz, matricula, &removeu);
    if (removeu) {
        printf("Profissional removido.\n");
    } else {
        printf("Matricula nao encontrada.\n");
    }
    return raiz;
}

void mostrarMenu(void) {
    printf("\n CADASTRO DE PROFISSIONAIS \n");
    printf("1 - Cadastrar profissional\n");
    printf("2 - Buscar profissional\n");
    printf("3 - Listar todos\n");
    printf("4 - Exibir a arvore\n");
    printf("5 - Remover profissional\n");
    printf("6 - Contar profissionais\n");
    printf("0 - Sair\n");
    printf("Escolha uma opcao: ");
}

int main(void) {
    No *raiz;
    int opcao;

    raiz = lerArquivo();

    do {
        mostrarMenu();
        opcao = lerNumero();
        printf("\n");

        switch (opcao) {
            case 1:
                raiz = cadastrar(raiz);
                break;
            case 2:
                buscarProfissional(raiz);
                break;
            case 3:
                if (raiz == NULL) {
                    printf("Nenhum profissional cadastrado.\n");
                } else {
                    listarEmOrdem(raiz);
                }
                break;
            case 4:
                if (raiz == NULL) {
                    printf("A arvore esta vazia.\n");
                } else {
                    exibirArvore(raiz, 0, 'R');
                }
                break;
            case 5:
                raiz = removerProfissional(raiz);
                break;
            case 6:
                printf("Total de profissionais: %d\n", contarNos(raiz));
                break;
            case 0:
                printf("Gravando dados e saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    gravarArquivo(raiz);
    liberarArvore(raiz);
    return 0;
}
