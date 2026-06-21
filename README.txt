=== PROJETO GRAFOS (AGM) ===

Instrucoes de Compilacao e Execucao:

1. Algoritmo Principal (Prim e Kruskal)
   Compilar: gcc main.c -O3 -o main
   Executar: ./main <algoritmo> <caminho_instancia>
   Exemplo: ./main prim GeradorInstancia/instancias/denso_V100.txt

2. Script de Testes (Scriptcalular)
   Compilar: gcc Scriptcalular/main.c -o Scriptcalular
   Executar: ./Scriptcalular ./main GeradorInstancia/instancias
   * O script gerara um arquivo CSV chamado 'resultados_reais_com_estatistica.csv' contendo os resultados consolidados.
