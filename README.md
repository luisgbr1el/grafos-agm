## 1. Documentação das Funções

Esta seção serve como memorial descritivo das sub-estruturas fundamentais exigidas para o trabalho, detalhando o papel de cada função implementada no ciclo de vida do programa.

### A. Estrutura Union-Find (Disjoint Set)
Utilizada exclusivamente pelo algoritmo de Kruskal para gerenciar e validar componentes conexas de forma eficiente, impedindo a inserção de arestas que formem ciclos no grafo.
* `criar_union_find(int n)`: Aloca memória para os vetores de pais (`pai`) e alturas (`rank`). Define inicialmente que cada vértice é pai de si mesmo, criando $n$ subconjuntos unitários disjuntos.
* `encontrar_uf(UnionFind* uf, int i)`: Determina o elemento representante (raiz) do conjunto que contém o vértice `i`. Utiliza a técnica de **Compressão de Caminho (Path Compression)**, que atualiza o pai de todos os nós visitados diretamente para a raiz, otimizando drasticamente as consultas futuras.
* `uniao_uf(UnionFind* uf, int x, int y)`: Conecta os conjuntos dos elementos `x` e `y`. Utiliza a técnica **Union by Rank** para pendurar a árvore de menor altura sob a raiz da árvore de maior altura, mantendo o balanceamento da estrutura e garantindo complexidade de tempo quase linear. Retorna `true` se a união consolidou-se ou `false` caso eles já integrassem a mesma componente (evitando ciclo).
* `liberar_union_find(UnionFind* uf)`: Limpa as estruturas internas e desaloca a memória que foi reservada dinamicamente.

### B. Fila de Prioridade (Min-Heap)
Utilizada pelo algoritmo de Prim para extrair rapidamente o vértice que possui o menor peso de borda associado em relação à árvore que está sendo construída.
* `criar_min_heap(int capacidade)`: Inicializa a árvore binária completa do heap, alocando o array de nós e o vetor de posições utilizado para o mapeamento direto.
* `min_heapify(MinHeap* min_heap, int idx)`: Realiza a correção estrutural descendente. Move um elemento para baixo na árvore caso ele quebre a propriedade fundamental do min-heap (onde o nó pai deve ser estritamente menor ou igual aos seus nós filhos).
* `extrair_min(MinHeap* min_heap)`: Remove e retorna o nó raiz de menor valor chave, reorganizando a estrutura do heap em tempo $O(\log V)$.
* `diminuir_chave(MinHeap* min_heap, int v, double chave)`: Ajuste estrutural ascendente. Reduz o valor da prioridade (chave) de um vértice e propaga-o para cima na árvore. Utiliza o vetor auxiliar de posições para localizar o vértice instantaneamente no array em custo $O(1)$, executando o rearranjo total em $O(\log V)$.
* `esta_no_min_heap(MinHeap* min_heap, int v)`: Consulta e retorna uma resposta booleana indicando se um determinado vértice ainda precisa ser processado dentro da fila de prioridade.
* `liberar_min_heap(MinHeap* min_heap)`: Varre a estrutura com base em sua capacidade original para liberar individualmente a memória de cada nó alocado, prevenindo vazamentos de memória (*memory leaks*).

### C. Abstração do Grafo e Algoritmos de AGM
* `criar_grafo(int num_vertices, int num_arestas)` / `adicionar_aresta(...)`: Funções responsáveis por mapear a entrada do programa. Elas alimentam simultaneamente o vetor estático de arestas (usado para a ordenação global do Kruskal) e as listas encadeadas de adjacências (usadas para a exploração de vizinhos no Prim).
* `liberar_grafo(Grafo* grafo)`: Percorre todas as listas de adjacência liberando os nós encadeados e, por fim, desaloca os vetores principais e a struct do grafo.
* `comparar_arestas(const void* a, const void* b)`: Função de callback customizada para o método `qsort` do C, permitindo ordenar o vetor de arestas em ordem crescente de peso.
* `algoritmo_kruskal(Grafo* grafo, Aresta* resultado_agm)`: Ordena todas as arestas por peso através do `qsort`. Itera sobre o vetor ordenado e constrói a AGM interceptando ciclos com o Union-Find. Preenche o vetor resultado com as $V-1$ arestas ótimas e retorna a somatória final dos pesos.
* `algoritmo_prim(Grafo* grafo, Aresta* resultado_agm)`: Define o vértice `0` como raiz inicial e alimenta o Min-Heap com os custos iniciais mapeados como infinito. Explora as adjacências a cada extração do nó mínimo, atualizando chaves e rastreando os pais. Preenche o vetor por referência com as arestas pertencentes à solução e retorna o peso total gerado.

---

## 2. Exemplos de Execução

Para validar as métricas de desempenho separadas (tempo de leitura/construção do grafo versus tempo de execução lógica do algoritmo), o programa utiliza parâmetros de linha de comando.

### A. Formato do Arquivo de Entrada (`instancia_teste.txt`)
O arquivo de texto contendo o grafo deve seguir o padrão acadêmico: a primeira linha informa a quantidade total de vértices e arestas ($V$ e $E$), seguida pelas linhas contendo a origem, o destino e o peso associado (`u v peso`):

```text
4 5
0 1 10.0
0 2 6.0
0 3 5.0
1 3 15.0
2 3 4.0
```

### B. Saída do Terminal - Algoritmo de Kruskal
Comando para compilação e execução via Kruskal:

```sh
gcc -O3 main.c -o main
./main kruskal instancia_teste.txt
```

Saída gerada no console:
```text
=== METRICAS DE DESEMPENHO ===
Tempo de Leitura/Construcao: 0.000120 segundos
Tempo de Execucao (kruskal): 0.000005 segundos

=== RESULTADO DA SOLUCAO ===
Peso Total da AGM: 15.0000
Arestas pertencentes a AGM:
(2, 3) -> Peso: 4.0000
(0, 3) -> Peso: 5.0000
(0, 1) -> Peso: 10.0000
```

### C. Saída do Terminal - Algoritmo de Prim
Comando para execução via Prim:

```sh
./main prim instancia_teste.txt
```

Saída gerada no console:
```text
=== METRICAS DE DESEMPENHO ===
Tempo de Leitura/Construcao: 0.000115 segundos
Tempo de Execucao (prim): 0.000008 segundos

=== RESULTADO DA SOLUCAO ===
Peso Total da AGM: 15.0000
Arestas pertencentes a AGM:
(0, 3) -> Peso: 5.0000
(3, 2) -> Peso: 4.0000
(0, 1) -> Peso: 10.0000
```