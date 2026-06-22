#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

#define NUM_EXECUCOES 10

const char *ALGORITMOS[] = {"prim", "kruskal"};

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

void capitalizar(char *dest, const char *src) {
    strcpy(dest, src);
    if (dest[0] >= 'a' && dest[0] <= 'z') {
        dest[0] -= 32;
    }
}

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *ent;

    const char *executavel = (argc > 1) ? argv[1] : "../main.exe";
    const char *pasta_instancias = (argc > 2) ? argv[2] : "../GeradorInstancia/instancias";

    if ((dir = opendir(pasta_instancias)) == NULL) {
        printf("Erro: A pasta de instancias '%s' nao foi encontrada.\n", pasta_instancias);
        return 1;
    }

    FILE *csv = fopen("resultados.csv", "w");
    if (csv == NULL) {
        printf("Erro ao criar o arquivo CSV.\n");
        closedir(dir);
        return 1;
    }

    fprintf(csv, "Tamanho_V;Nome_Arquivo;Algoritmo;Peso_AGM;Media_Tempo_Leitura_s;Media_Tempo_Algoritmo_s;Desvio_Padrao_Algoritmo_s\n");

    printf("==================================================================\n");
    printf(" Iniciando testes (%dx cada)\n", NUM_EXECUCOES);
    printf(" Executavel: %s\n", executavel);
    printf(" Pasta de instancias: %s\n", pasta_instancias);
    printf("==================================================================\n\n");

    while ((ent = readdir(dir)) != NULL) {
        const char *ext = strrchr(ent->d_name, '.');
        if (ext != NULL && strcmp(ext, ".txt") == 0) {

            char caminho_arquivo[1024];
            snprintf(caminho_arquivo, sizeof(caminho_arquivo), "%s/%s", pasta_instancias, ent->d_name);

            int v_tamanho = extrair_tamanho(ent->d_name);

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

                    snprintf(comando, sizeof(comando), "\"\"%s\" %s \"%s\"\"", executavel, alg, caminho_arquivo);

                    FILE *pipe = POPEN(comando, "r");
                    if (pipe == NULL) {
                        printf(" -> Erro ao invocar o executavel na rodada %d.\n", i + 1);
                        continue;
                    }

                    char linha_saida[512];
                    double tempo_leitura_detectado = -1.0;
                    double tempo_algoritmo_detectado = -1.0;
                    double peso_detectado = 0.0;

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

                    if (tempo_algoritmo_detectado >= 0.0 && tempo_leitura_detectado >= 0.0) {
                        tempos_algoritmo[rodadas_com_sucesso] = tempo_algoritmo_detectado;
                        soma_tempos_algoritmo += tempo_algoritmo_detectado;
                        soma_tempos_leitura += tempo_leitura_detectado;
                        peso_final_agm = peso_detectado;
                        rodadas_com_sucesso++;
                    }
                }

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
    printf("\nTestes realizados!\n");
    printf("Arquivo CSV gerado: resultados.csv\n");

    return 0;
}
