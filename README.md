# Trabalho Prático – Árvore Geradora Mínima

- **Integrantes:** Cainã Farias, Gustavo Farias e Luis Gabriel

# Instruções
Você precisa ter instalado o **GCC (GNU Compiler Collection)** em sua máquina.

## 1. Compilação
- **No Linux:**
    ```bash
    gcc -O3 main.c -o main
    gcc -O3 gerador.c -o gerador -lm
    gcc -O3 calcular.c -o calcular -lm
    ```
    > **Nota:** A flag `-lm` é obrigatória no Linux para vincular a biblioteca matemática `<math.h>` usada no cálculo de distância euclidiana do gerador e no cálculo de desvio padrão das estatísticas.

- **No Windows (MinGW / GCC):**
    ```cmd
    gcc -O3 main.c -o main.exe
    gcc -O3 gerador.c -o gerador.exe
    gcc -O3 calcular.c -o calcular.exe
    ```

## 2. Geração das Instâncias
Caso necessite, você pode gerar instâncias de testes. O gerador criará automaticamente a pasta `instancias/` e fará a criação de 20 instâncias variadas.

* **No Linux:**
    ```bash
    ./gerador
    ```
* **No Windows:**
    ```cmd
    gerador.exe
    ```

## 3. Execução individual (testes específicos)
Se quiser apenas testar a saída de um algoritmo em um grafo específico e visualizar as métricas direto no terminal, passe o modo (`prim` ou `kruskal`) seguido do caminho do arquivo gerado:

* **No Linux:**
    ```bash
    ./main prim instancias/esparso_V100.txt
    ./main kruskal instancias/esparso_V100.txt
    ```

* **No Windows:**
    ```cmd
    main.exe prim instancias\esparso_V100.txt
    main.exe kruskal instancias\esparso_V100.txt
    ```

## 4. Execução dos testes em lote (medição de tempos)
Para automatizar a execução exigida pelo trabalho, o script `calcular` processará todos os arquivos encontrados na pasta `instancias/`. Ele executará cada algoritmo 10 vezes, registrará separadamente os tempos de leitura e os tempos de execução do algoritmo, e consolidará as médias e o desvio padrão em um arquivo chamado `resultados_reais_com_estatistica.csv`.

* **No Linux:**
    ```bash
    ./calcular ./main instancias
    ```

* **No Windows:**
    ```cmd
    calcular.exe main.exe instancias
    ```