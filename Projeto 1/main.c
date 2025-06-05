#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct No {
    int id;
    struct No* prox;
} No;

typedef struct {
    char nome[50];
    No* listaAdj;
    int visitado;
} Usuario;

Usuario usuarios[MAX];
int totalUsuarios = 0;

// Funcoes para lista de adjacencia
No* criarNo(int id) {
    No* novo = (No*)malloc(sizeof(No));
    novo->id = id;
    novo->prox = NULL;
    return novo;
}

void adicionarAresta(int origem, int destino) {
    No* novo = criarNo(destino);
    novo->prox = usuarios[origem].listaAdj;
    usuarios[origem].listaAdj = novo;

    novo = criarNo(origem);
    novo->prox = usuarios[destino].listaAdj;
    usuarios[destino].listaAdj = novo;
}

// Arquivo
void salvarUsuariosEmArquivo() {
    FILE* fp = fopen("usuarios.txt", "w");
    if (!fp) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return;
    }
    for (int i = 0; i < totalUsuarios; i++) {
        fprintf(fp, "%s\n", usuarios[i].nome);
    }
    fclose(fp);
}

void carregarUsuariosDeArquivo() {
    FILE* fp = fopen("usuarios.txt", "r");
    if (!fp) {
        return;
    }
    char linha[50];
    while (fgets(linha, sizeof(linha), fp)) {
        linha[strcspn(linha, "\n")] = 0; // remove \n
        strcpy(usuarios[totalUsuarios].nome, linha);
        usuarios[totalUsuarios].listaAdj = NULL;
        usuarios[totalUsuarios].visitado = 0;
        totalUsuarios++;
    }
    fclose(fp);
}

// Funcoes do sistema
void adicionarUsuario(char nome[]) {
    if (totalUsuarios >= MAX) {
        printf("Limite de usuarios atingido.\n");
        return;
    }
    strcpy(usuarios[totalUsuarios].nome, nome);
    usuarios[totalUsuarios].listaAdj = NULL;
    usuarios[totalUsuarios].visitado = 0;
    totalUsuarios++;
    salvarUsuariosEmArquivo();
    printf("Usuario adicionado com sucesso!\n");
}

// Lista todos os usuarios com IDs
void listarUsuarios() {
    printf("\nUsuarios cadastrados:\n");
    if (totalUsuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        return;
    }
    for (int i = 0; i < totalUsuarios; i++) {
        printf("ID %d: %s\n", i, usuarios[i].nome);
    }
}

void criarConexao(int id1, int id2) {
    if (id1 >= totalUsuarios || id2 >= totalUsuarios || id1 == id2) {
        printf("IDs invalidos.\n");
        return;
    }

    // Verifica se ja existe conexao
    No* atual = usuarios[id1].listaAdj;
    while (atual != NULL) {
        if (atual->id == id2) {
            printf("Conexao ja existe.\n");
            return;
        }
        atual = atual->prox;
    }

    adicionarAresta(id1, id2);
    printf("Conexao criada com sucesso!\n");
}

void mostrarAmigos(int id) {
    if (id >= totalUsuarios) {
        printf("ID invalido.\n");
        return;
    }

    printf("Amigos de %s (ID %d):\n", usuarios[id].nome, id);
    No* atual = usuarios[id].listaAdj;

    if (atual == NULL) {
        printf("- Este usuario ainda nao tem nenhum amigo.\n");
        return;
    }

    while (atual != NULL) {
        printf("- ID %d: %s\n", atual->id, usuarios[atual->id].nome);
        atual = atual->prox;
    }
}

// BFS
void bfs(int inicio) {
    int fila[MAX], frente = 0, tras = 0;
    for (int i = 0; i < totalUsuarios; i++)
        usuarios[i].visitado = 0;

    usuarios[inicio].visitado = 1;
    fila[tras++] = inicio;

    printf("BFS a partir de %s:\n", usuarios[inicio].nome);

    while (frente < tras) {
        int atual = fila[frente++];
        printf("- %s\n", usuarios[atual].nome);
        No* adj = usuarios[atual].listaAdj;
        while (adj != NULL) {
            if (!usuarios[adj->id].visitado) {
                usuarios[adj->id].visitado = 1;
                fila[tras++] = adj->id;
            }
            adj = adj->prox;
        }
    }
}

// DFS
void dfsUtil(int id) {
    usuarios[id].visitado = 1;
    printf("- %s\n", usuarios[id].nome);
    No* adj = usuarios[id].listaAdj;
    while (adj != NULL) {
        if (!usuarios[adj->id].visitado) {
            dfsUtil(adj->id);
        }
        adj = adj->prox;
    }
}

void dfs(int inicio) {
    for (int i = 0; i < totalUsuarios; i++)
        usuarios[i].visitado = 0;

    printf("DFS a partir de %s:\n", usuarios[inicio].nome);
    dfsUtil(inicio);
}

// Sugestao de amigos
void sugerirAmigos(int id) {
    if (id >= totalUsuarios) {
        printf("ID invalido.\n");
        return;
    }

    int amigosDiretos[MAX] = {0};
    int sugeridos[MAX] = {0};

    No* adj = usuarios[id].listaAdj;
    while (adj != NULL) {
        amigosDiretos[adj->id] = 1;
        adj = adj->prox;
    }

    adj = usuarios[id].listaAdj;
    while (adj != NULL) {
        No* adj2 = usuarios[adj->id].listaAdj;
        while (adj2 != NULL) {
            if (adj2->id != id && !amigosDiretos[adj2->id])
                sugeridos[adj2->id] = 1;
            adj2 = adj2->prox;
        }
        adj = adj->prox;
    }

    printf("Sugestoes de amizade para %s (ID %d):\n", usuarios[id].nome, id);
    int encontrou = 0;
    for (int i = 0; i < totalUsuarios; i++) {
        if (sugeridos[i]) {
            printf("- ID %d: %s\n", i, usuarios[i].nome);
            encontrou = 1;
        }
    }

    if (!encontrou)
        printf("Sem sugestoes no momento.\n");
}

// Menu
void menu() {
    int opcao;
    do {
        printf("\n======= MENU =======\n");
        printf("1. Adicionar usuario\n");
        printf("2. Criar conexao\n");
        printf("3. Visualizar rede de amizades\n");
        printf("4. BFS\n");
        printf("5. DFS\n");
        printf("6. Sugerir amigos\n");
        printf("7. Listar todos os usuarios\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        int id1, id2;
        char nome[50];

        switch (opcao) {
            case 1:
                printf("Nome do usuario: ");
                scanf(" %[^\n]", nome);
                adicionarUsuario(nome);
                break;
            case 2:
                printf("ID do usuario 1: ");
                scanf("%d", &id1);
                printf("ID do usuario 2: ");
                scanf("%d", &id2);
                criarConexao(id1, id2);
                break;
            case 3:
                printf("ID do usuario: ");
                scanf("%d", &id1);
                mostrarAmigos(id1);
                break;
            case 4:
                printf("ID de inicio para BFS: ");
                scanf("%d", &id1);
                bfs(id1);
                break;
            case 5:
                printf("ID de inicio para DFS: ");
                scanf("%d", &id1);
                dfs(id1);
                break;
            case 6:
                printf("ID do usuario: ");
                scanf("%d", &id1);
                sugerirAmigos(id1);
                break;
            case 7:
                listarUsuarios();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}

// Main
int main() {
    carregarUsuariosDeArquivo();
    menu();
    return 0;
}
