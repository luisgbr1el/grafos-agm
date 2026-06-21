#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define INF 1e9

typedef struct {
    int u;
    int v;
    double peso;
} Aresta;

typedef struct NoAdjacencia {
    int vertice;
    double peso;
    struct NoAdjacencia* proximo;
} NoAdjacencia;

typedef struct {
    NoAdjacencia* cabeca;
} ListaAdjacencia;

typedef struct {
    int num_vertices;
    int num_arestas;
    ListaAdjacencia* adj;
    Aresta* arestas;     
} Grafo;

typedef struct {
    int* pai;
    int* rank;
} UnionFind;

UnionFind* criar_union_find(int n) {
    UnionFind* uf = (UnionFind*)malloc(sizeof(UnionFind));
    uf->pai = (int*)malloc(n * sizeof(int));
    uf->rank = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        uf->pai[i] = i;
        uf->rank[i] = 0;
    }
    return uf;
}

int encontrar_uf(UnionFind* uf, int i) {
    if (uf->pai[i] != i) {
        uf->pai[i] = encontrar_uf(uf, uf->pai[i]);
    }
    return uf->pai[i];
}

bool uniao_uf(UnionFind* uf, int x, int y) {
    int raiz_x = encontrar_uf(uf, x);
    int raiz_y = encontrar_uf(uf, y);

    if (raiz_x != raiz_y) {
        if (uf->rank[raiz_x] < uf->rank[raiz_y]) {
            uf->pai[raiz_x] = raiz_y;
        } else if (uf->rank[raiz_x] > uf->rank[raiz_y]) {
            uf->pai[raiz_y] = raiz_x;
        } else {
            uf->pai[raiz_y] = raiz_x;
            uf->rank[raiz_x]++;
        }
        return true;
    }
    return false;
}

void liberar_union_find(UnionFind* uf) {
    free(uf->pai);
    free(uf->rank);
    free(uf);
}

typedef struct {
    int v;
    double chave;
} NoMinHeap;

typedef struct {
    int capacidade;
    int tamanho;
    int* posicao;
    NoMinHeap** array;
    NoMinHeap** todos_os_nos;
} MinHeap;

MinHeap* criar_min_heap(int capacidad) {
    MinHeap* min_heap = (MinHeap*)malloc(sizeof(MinHeap));
    min_heap->posicao = (int*)malloc(capacidad * sizeof(int));
    min_heap->tamanho = 0;
    min_heap->capacidade = capacidad;
    min_heap->array = (NoMinHeap**)malloc(capacidad * sizeof(NoMinHeap*));
    min_heap->todos_os_nos = (NoMinHeap**)malloc(capacidad * sizeof(NoMinHeap*));
    return min_heap;
}

void trocar_no_min_heap(NoMinHeap** a, NoMinHeap** b) {
    NoMinHeap* t = *a;
    *a = *b;
    *b = t;
}

void min_heapify(MinHeap* min_heap, int idx) {
    int menor = idx;
    int esquerda = 2 * idx + 1;
    int direita = 2 * idx + 2;

    if (esquerda < min_heap->tamanho && min_heap->array[esquerda]->chave < min_heap->array[menor]->chave) {
        menor = esquerda;
    }
    if (direita < min_heap->tamanho && min_heap->array[direita]->chave < min_heap->array[menor]->chave) {
        menor = direita;
    }

    if (menor != idx) {
        NoMinHeap* no_menor = min_heap->array[menor];
        NoMinHeap* no_atual = min_heap->array[idx];

        min_heap->posicao[no_menor->v] = idx;
        min_heap->posicao[no_atual->v] = menor;

        trocar_no_min_heap(&min_heap->array[menor], &min_heap->array[idx]);
        min_heapify(min_heap, menor);
    }
}

bool esta_vazia_min_heap(MinHeap* min_heap) {
    return min_heap->tamanho == 0;
}

NoMinHeap* extrair_min(MinHeap* min_heap) {
    if (esta_vazia_min_heap(min_heap)) return NULL;

    NoMinHeap* raiz = min_heap->array[0];
    NoMinHeap* ultimo_no = min_heap->array[min_heap->tamanho - 1];

    min_heap->array[0] = ultimo_no;
    min_heap->posicao[raiz->v] = min_heap->tamanho - 1;
    min_heap->posicao[ultimo_no->v] = 0;

    min_heap->tamanho--;
    min_heapify(min_heap, 0);

    return raiz;
}

void diminuir_chave(MinHeap* min_heap, int v, double chave) {
    int i = min_heap->posicao[v];
    min_heap->array[i]->chave = chave;

    while (i && min_heap->array[i]->chave < min_heap->array[(i - 1) / 2]->chave) {
        min_heap->posicao[min_heap->array[i]->v] = (i - 1) / 2;
        min_heap->posicao[min_heap->array[(i - 1) / 2]->v] = i;
        trocar_no_min_heap(&min_heap->array[i], &min_heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

bool esta_no_min_heap(MinHeap* min_heap, int v) {
    if (min_heap->posicao[v] < min_heap->tamanho) return true;
    return false;
}

void liberar_min_heap(MinHeap* min_heap) {
    free(min_heap->posicao);

    for (int i = 0; i < min_heap->capacidade; i++) {
        free(min_heap->todos_os_nos[i]);
    }

    free(min_heap->array);
    free(min_heap->todos_os_nos);
    free(min_heap);
}

Grafo* criar_grafo(int num_vertices, int num_arestas) {
    Grafo* grafo = (Grafo*)malloc(sizeof(Grafo));
    grafo->num_vertices = num_vertices;
    grafo->num_arestas = num_arestas;

    grafo->adj = (ListaAdjacencia*)calloc(num_vertices, sizeof(ListaAdjacencia));
    for (int i = 0; i < num_vertices; i++) {
        grafo->adj[i].cabeca = NULL;
    }

    grafo->arestas = (Aresta*)malloc(num_arestas * sizeof(Aresta));
    return grafo;
}

void adicionar_aresta(Grafo* grafo, int indice_aresta, int u, int v, double peso) {
    grafo->arestas[indice_aresta].u = u;
    grafo->arestas[indice_aresta].v = v;
    grafo->arestas[indice_aresta].peso = peso;

    NoAdjacencia* novo_no = (NoAdjacencia*)malloc(sizeof(NoAdjacencia));
    novo_no->vertice = v;
    novo_no->peso = peso;
    novo_no->proximo = grafo->adj[u].cabeca;
    grafo->adj[u].cabeca = novo_no;

    novo_no = (NoAdjacencia*)malloc(sizeof(NoAdjacencia));
    novo_no->vertice = u;
    novo_no->peso = peso;
    novo_no->proximo = grafo->adj[v].cabeca;
    grafo->adj[v].cabeca = novo_no;
}

void liberar_grafo(Grafo* grafo) {
    for (int i = 0; i < grafo->num_vertices; i++) {
        NoAdjacencia* atual = grafo->adj[i].cabeca;
        while (atual != NULL) {
            NoAdjacencia* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
    free(grafo->adj);
    free(grafo->arestas);
    free(grafo);
}

int comparar_arestas(const void* a, const void* b) {
    Aresta* a_elem = (Aresta*)a;
    Aresta* b_elem = (Aresta*)b;
    if (a_elem->peso < b_elem->peso) return -1;
    if (a_elem->peso > b_elem->peso) return 1;
    return 0;
}

double algoritmo_kruskal(Grafo* grafo, Aresta* resultado_agm) {
    int V = grafo->num_vertices;
    int E = grafo->num_arestas;
    double peso_total = 0;
    int e_agm = 0;
    int i = 0;

    qsort(grafo->arestas, E, sizeof(Aresta), comparar_arestas);

    UnionFind* uf = criar_union_find(V);

    while (e_agm < V - 1 && i < E) {
        Aresta proxima_aresta = grafo->arestas[i++];

        if (uniao_uf(uf, proxima_aresta.u, proxima_aresta.v)) {
            resultado_agm[e_agm++] = proxima_aresta;
            peso_total += proxima_aresta.peso;
        }
    }

    liberar_union_find(uf);
    return peso_total;
}

double algoritmo_prim(Grafo* grafo, Aresta* resultado_agm) {
    int V = grafo->num_vertices;
    int* pai = (int*)malloc(V * sizeof(int));
    double* chave = (double*)malloc(V * sizeof(double));
    
    MinHeap* min_heap = criar_min_heap(V);

    for (int v = 0; v < V; v++) {
        pai[v] = -1;
        chave[v] = INF;
        min_heap->array[v] = (NoMinHeap*)malloc(sizeof(NoMinHeap));
        min_heap->array[v]->v = v;
        min_heap->array[v]->chave = chave[v];
        min_heap->posicao[v] = v;
        min_heap->todos_os_nos[v] = min_heap->array[v];
    }

    chave[0] = 0;
    min_heap->array[0]->chave = 0;
    min_heap->tamanho = V;

    double peso_total = 0;
    int e_agm = 0;

    while (!esta_vazia_min_heap(min_heap)) {
        NoMinHeap* no_min = extrair_min(min_heap);
        int u = no_min->v;
        
        if (pai[u] != -1) {
            resultado_agm[e_agm].u = pai[u];
            resultado_agm[e_agm].v = u;
            resultado_agm[e_agm].peso = no_min->chave;
            peso_total += no_min->chave;
            e_agm++;
        }

        NoAdjacencia* rastejador = grafo->adj[u].cabeca;
        while (rastejador != NULL) {
            int v = rastejador->vertice;

            if (esta_no_min_heap(min_heap, v) && rastejador->peso < chave[v]) {
                chave[v] = rastejador->peso;
                pai[v] = u;
                diminuir_chave(min_heap, v, chave[v]);
            }
            rastejador = rastejador->proximo;
        }
    }

    liberar_min_heap(min_heap);
    free(pai);
    free(chave);

    return peso_total;
}

Grafo* ler_grafo_de_arquivo(const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return NULL;
    }

    int V, E;
    if (fscanf(arquivo, "%d %d", &V, &E) != 2) {
        fclose(arquivo);
        return NULL;
    }

    Grafo* grafo = criar_grafo(V, E);

    int u, v;
    double peso;
    for (int i = 0; i < E; i++) {
        if (fscanf(arquivo, "%d %d %lf", &u, &v, &peso) == 3) {
            adicionar_aresta(grafo, i, u, v, peso);
        }
    }

    fclose(arquivo);
    return grafo;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso correto: %s <modo: prim|kruskal> <arquivo_instancia.txt>\n", argv[0]);
        return 1;
    }

    char* modo = argv[1];
    char* arquivo_instancia = argv[2];

    clock_t inicio_leitura = clock();
    Grafo* grafo = ler_grafo_de_arquivo(arquivo_instancia);
    clock_t fim_leitura = clock();
    
    if (!grafo) return 1;

    double tempo_leitura = (double)(fim_leitura - inicio_leitura) / CLOCKS_PER_SEC;

    Aresta* agm = (Aresta*)malloc((grafo->num_vertices - 1) * sizeof(Aresta));
    double peso_total = 0;

    clock_t inicio_algoritmo = clock();
    if (strcmp(modo, "kruskal") == 0) {
        peso_total = algoritmo_kruskal(grafo, agm);
    } else if (strcmp(modo, "prim") == 0) {
        peso_total = algoritmo_prim(grafo, agm);
    } else {
        printf("Modo inválido. Escolha 'prim' ou 'kruskal'.\n");
        free(agm);
        liberar_grafo(grafo);
        return 1;
    }
    clock_t fim_algoritmo = clock();
    double tempo_algoritmo = (double)(fim_algoritmo - inicio_algoritmo) / CLOCKS_PER_SEC;

    printf("=== METRICAS DE DESEMPENHO ===\n");
    printf("Tempo de Leitura/Construcao: %.6f segundos\n", tempo_leitura);
    printf("Tempo de Execucao (%s): %.6f segundos\n", modo, tempo_algoritmo);
    printf("\n=== RESULTADO DA SOLUCAO ===\n");
    printf("Peso Total da AGM: %.4f\n", peso_total);
       
    // só pra debug
    printf("Arestas pertencentes a AGM:\n");
    for (int i = 0; i < grafo->num_vertices - 1; i++) {
        printf("(%d, %d) -> Peso: %.4f\n", agm[i].u, agm[i].v, agm[i].peso);
    }
    
    free(agm);
    liberar_grafo(grafo);

    return 0;
}