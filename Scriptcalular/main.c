#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>   /* Para varrer a pasta de instâncias */

// Garante compatibilidade do Pipe tanto no Windows quanto no Linux
#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

// ==============================================================================
// CONFIGURAÇÕES REVISADAS (FLUXO UFC - GRAFOS)
// ==============================================================================
// 1. Caminho do executável de grafos (usando barras invertidas duplas para o sistema)
#define EXECUTAVEL "C:/Users/faria/OneDrive/Desktop/UFC/Grafos/Trabalho/grafos-agm/main.exe"

// 2. Pasta onde o Scriptcalcular vai ler as instâncias (barras normais '/' evitam erros no C)
#define PASTA_INSTANCIAS "C:/Users/faria/OneDrive/Desktop/UFC/Grafos/Trabalho/GeradorInstancia/instancias"

// 3. Quantidade de vezes que cada algoritmo roda por arquivo para tirar a média
#define NUM_EXECUCOES 10

const char *ALGORITMOS[] = {"prim", "kruskal"};

/* ── Função para extrair o V (vértices) automaticamente do nome do arquivo ── */
int extrair_tamanho(const char *nome_arquivo) {
    const char *p = strstr(nome_arquivo, "_V");
    if (p != NULL) {
        int v;
        if (sscanf(p + 2, "%d", &v) == 1) {
            return v;
        }
    }
    return -1;
}

/* ── Capitaliza a primeira letra do algoritmo (ex: prim -> Prim) ── */
void capitalizar(char *dest, const char *src) {
    strcpy(dest, src);
    if (dest[0] >= 'a' && dest[0] <= 'z') {
        dest[0] -= 32;
    }
}

int main(void) {
    DIR *dir;
    struct dirent *ent;

    // Tenta abrir a pasta do GeradorInstancia
    if ((dir = opendir(PASTA_INSTANCIAS)) == NULL) {
        printf("Erro: A pasta de instancias '%s' nao foi encontrada.\n", PASTA_INSTANCIAS);
        return 1;
    }

    // Cria o arquivo CSV com os resultados na mesma pasta do Scriptcalcular
    FILE *csv = fopen("resultados_reais_com_estatistica.csv", "w");
    if (csv == NULL) {
        printf("Erro ao criar o arquivo CSV.\n");
        closedir(dir);
        return 1;
    }

    // Cabeçalho das colunas do CSV
    fprintf(csv, "Tamanho_V;Nome_Arquivo;Algoritmo;Peso_AGM;Media_Tempo_Leitura_s;Media_Tempo_Algoritmo_s;Desvio_Padrao_Algoritmo_s\n");

    printf("==================================================================\n");
    printf(" Iniciando Bateria de Testes Integrada (%dx cada)\n", NUM_EXECUCOES);
    printf(" Executavel: %s\n", EXECUTAVEL);
    printf("==================================================================\n\n");

    // Varre a pasta procurando os arquivos .txt das instâncias
    while ((ent = readdir(dir)) != NULL) {
        const char *ext = strrchr(ent->d_name, '.');
        if (ext != NULL && strcmp(ext, ".txt") == 0) {

            // Monta o caminho completo da instância usando barras normais
            char caminho_arquivo[1024];
            snprintf(caminho_arquivo, sizeof(caminho_arquivo), "%s/%s", PASTA_INSTANCIAS, ent->d_name);

            int v_tamanho = extrair_tamanho(ent->d_name);

            // Roda o algoritmo "prim" e depois o "kruskal"
            for (int a = 0; a < 2; a++) {
                const char *alg = ALGORITMOS[a];
                printf("Executando [%s] na instancia '%s'...\n", alg, ent->d_name);

                double tempos_algoritmo[NUM_EXECUCOES];
                double soma_tempos_algoritmo = 0.0;
                double soma_tempos_leitura = 0.0;
                double peso_final_agm = 0.0;
                int rodadas_com_sucesso = 0;

                for (int i = 0; i < NUM_EXECUCOES; i++) {
                    char comando[2048];

                    // Monta a chamada envolvendo os caminhos em aspas duplas de forma segura para o Windows
                    snprintf(comando, sizeof(comando), "\"\"%s\" %s \"%s\"\"", EXECUTAVEL, alg, caminho_arquivo);

                    // Executa o grafos-agm via Pipe e coleta os textos de saída (printfs)
                    FILE *pipe = POPEN(comando, "r");
                    if (pipe == NULL) {
                        printf(" -> Erro ao invocar o executavel na rodada %d.\n", i + 1);
                        continue;
                    }

                    char linha_saida[512];
                    double tempo_leitura_detectado = -1.0;
                    double tempo_algoritmo_detectado = -1.0;
                    double peso_detectado = 0.0;

                    // Captura e filtra os resultados gerados pelo grafos-agm
                    while (fgets(linha_saida, sizeof(linha_saida), pipe) != NULL) {
                        if (strstr(linha_saida, "Tempo de Leitura/Construcao") != NULL) {
                            char *pos = strchr(linha_saida, ':');
                            if (pos) sscanf(pos + 1, "%lf", &tempo_leitura_detectado);
                        }
                        if (strstr(linha_saida, "Tempo de Execucao") != NULL) {
                            char *pos = strchr(linha_saida, ':');
                            if (pos) sscanf(pos + 1, "%lf", &tempo_algoritmo_detectado);
                        }
                        if (strstr(linha_saida, "Peso Total da AGM") != NULL) {
                            char *pos = strchr(linha_saida, ':');
                            if (pos) sscanf(pos + 1, "%lf", &peso_detectado);
                        }
                    }
                    PCLOSE(pipe);

                    // Armazena se a coleta funcionou
                    if (tempo_algoritmo_detectado >= 0.0 && tempo_leitura_detectado >= 0.0) {
                        tempos_algoritmo[rodadas_com_sucesso] = tempo_algoritmo_detectado;
                        soma_tempos_algoritmo += tempo_algoritmo_detectado;
                        soma_tempos_leitura += tempo_leitura_detectado;
                        peso_final_agm = peso_detectado;
                        rodadas_com_sucesso++;
                    }
                }

                // Se coletou dados com sucesso, calcula as estatísticas e joga no CSV
                if (rodadas_com_sucesso > 0) {
                    double media_algoritmo = soma_tempos_algoritmo / rodadas_com_sucesso;
                    double media_leitura = soma_tempos_leitura / rodadas_com_sucesso;

                    double soma_variancia = 0.0;
                    for (int i = 0; i < rodadas_com_sucesso; i++) {
                        soma_variancia += (tempos_algoritmo[i] - media_algoritmo) * (tempos_algoritmo[i] - media_algoritmo);
                    }
                    double desvio_padrao = (rodadas_com_sucesso > 1) ? sqrt(soma_variancia / (rodadas_com_sucesso - 1)) : 0.0;

                    char alg_cap[32];
                    capitalizar(alg_cap, alg);

                    if (v_tamanho != -1) {
                        fprintf(csv, "%d;%s;%s;%.4f;%.6f;%.6f;%.6f\n",
                                v_tamanho, ent->d_name, alg_cap, peso_final_agm, media_leitura, media_algoritmo, desvio_padrao);
                    } else {
                        fprintf(csv, "Desconhecido;%s;%s;%.4f;%.6f;%.6f;%.6f\n",
                                ent->d_name, alg_cap, peso_final_agm, media_leitura, media_algoritmo, desvio_padrao);
                    }
                }
            }
        }
    }

    closedir(dir);
    fclose(csv);
    printf("\n[SUCESSO] Processamento concluido com as pastas da UFC!\n");
    printf("Arquivo CSV gerado: resultados_reais_com_estatistica.csv\n");

    return 0;
}
