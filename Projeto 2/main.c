#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 100
#define INF 999999

typedef struct Aresta {
    int destino;
    int peso;
    struct Aresta* prox;
} Aresta;

typedef struct {
    char nome[50];
    Aresta* listaAdj;
} Cidade;

Cidade cidades[MAX];
int totalCidades = 0;

// ---------- Manipulacao de Arquivos ----------

void salvarCidadeNoArquivo(char* nome) {
    FILE* arq = fopen("cidades.txt", "a");
    if (arq == NULL) return;
    fprintf(arq, "%s\n", nome);
    fclose(arq);
}

void salvarRotaNoArquivo(int origem, int destino, int peso) {
    FILE* arq = fopen("rotas.txt", "a");
    if (arq == NULL) return;
    fprintf(arq, "%d %d %d\n", origem, destino, peso);
    fclose(arq);
}

void carregarCidades() {
    FILE* arq = fopen("cidades.txt", "r");
    if (arq == NULL) return;
    char linha[50];
    while (fgets(linha, sizeof(linha), arq)) {
        linha[strcspn(linha, "\n")] = '\0';
        strcpy(cidades[totalCidades].nome, linha);
        cidades[totalCidades].listaAdj = NULL;
        totalCidades++;
    }
    fclose(arq);
}

void adicionarAresta(int origem, int destino, int peso) {
    Aresta* nova = malloc(sizeof(Aresta));
    nova->destino = destino;
    nova->peso = peso;
    nova->prox = cidades[origem].listaAdj;
    cidades[origem].listaAdj = nova;
}

void carregarRotas() {
    FILE* arq = fopen("rotas.txt", "r");
    if (arq == NULL) return;
    int origem, destino, peso;
    while (fscanf(arq, "%d %d %d", &origem, &destino, &peso) == 3) {
        adicionarAresta(origem, destino, peso);
        adicionarAresta(destino, origem, peso);
    }
    fclose(arq);
}

// ---------- Funcionalidades ----------

void adicionarCidade(char* nome) {
    strcpy(cidades[totalCidades].nome, nome);
    cidades[totalCidades].listaAdj = NULL;
    salvarCidadeNoArquivo(nome);
    printf("Cidade '%s' adicionada com ID %d.\n", nome, totalCidades);
    totalCidades++;
}

void adicionarRota(int origem, int destino, int peso) {
    adicionarAresta(origem, destino, peso);
    adicionarAresta(destino, origem, peso);
    salvarRotaNoArquivo(origem, destino, peso);
    printf("Rota criada entre '%s' e '%s' com distancia %d.\n",
           cidades[origem].nome, cidades[destino].nome, peso);
}

void dijkstra(int origem, int destino) {
    int dist[MAX], visitado[MAX], anterior[MAX];
    for (int i = 0; i < totalCidades; i++) {
        dist[i] = INF;
        visitado[i] = 0;
        anterior[i] = -1;
    }

    dist[origem] = 0;

    for (int i = 0; i < totalCidades; i++) {
        int minDist = INF, u = -1;
        for (int j = 0; j < totalCidades; j++) {
            if (!visitado[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1) break;
        visitado[u] = 1;

        Aresta* a = cidades[u].listaAdj;
        while (a != NULL) {
            int v = a->destino;
            if (!visitado[v] && dist[u] + a->peso < dist[v]) {
                dist[v] = dist[u] + a->peso;
                anterior[v] = u;
            }
            a = a->prox;
        }
    }

    if (dist[destino] == INF) {
        printf("Nao ha caminho entre '%s' e '%s'.\n", cidades[origem].nome, cidades[destino].nome);
        return;
    }

    int caminho[MAX], tam = 0;
    for (int at = destino; at != -1; at = anterior[at])
        caminho[tam++] = at;

    printf("Menor caminho de '%s' para '%s' (distancia %d):\n",
           cidades[origem].nome, cidades[destino].nome, dist[destino]);

    for (int i = tam - 1; i >= 0; i--) {
        printf("%s", cidades[caminho[i]].nome);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

void listarCidades() {
    for (int i = 0; i < totalCidades; i++) {
        printf("ID %d - %s\n", i, cidades[i].nome);
        Aresta* a = cidades[i].listaAdj;
        while (a != NULL) {
            printf("  -> %s (distancia %d)\n", cidades[a->destino].nome, a->peso);
            a = a->prox;
        }
    }
}

// ---------- Menu Principal ----------

void menu() {
    int op;
    char nome[50];
    int id1, id2, peso;

    do {
        printf("\n===== MENU =====\n");
        printf("1. Adicionar cidade\n");
        printf("2. Adicionar rota\n");
        printf("3. Calcular menor caminho (Dijkstra)\n");
        printf("4. Listar cidades e rotas\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &op);

        switch (op) {
            case 1:
                printf("Nome da cidade: ");
                scanf(" %[^\n]", nome);
                adicionarCidade(nome);
                break;
            case 2:
                listarCidades();
                printf("ID da cidade origem: ");
                scanf("%d", &id1);
                printf("ID da cidade destino: ");
                scanf("%d", &id2);
                printf("Distancia entre elas: ");
                scanf("%d", &peso);
                adicionarRota(id1, id2, peso);
                break;
            case 3:
                listarCidades();
                printf("ID da cidade origem: ");
                scanf("%d", &id1);
                printf("ID da cidade destino: ");
                scanf("%d", &id2);
                dijkstra(id1, id2);
                break;
            case 4:
                listarCidades();
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (op != 0);
}

int main() {
    carregarCidades();
    carregarRotas();
    menu();
    return 0;
}
