#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif


static unsigned long long rng = 12345;

void seed(unsigned long long s) { rng = s ? s : 1; }

double randf() {
    rng ^= rng << 13; rng ^= rng >> 7; rng ^= rng << 17;
    return (double)(rng >> 11) / (double)(1ULL << 53);
}

int randi(int n) { return (int)(randf() * n); }

double uniform(double lo, double hi) { return lo + randf() * (hi - lo); }

/* ── Salvar grafo em arquivo ──────────────────────────────── */

void salvar(const char *nome, int V, int *us, int *vs, double *ws, int E) {
    char path[128];
    snprintf(path, sizeof(path), "instancias/%s", nome);
    FILE *f = fopen(path, "w");
    fprintf(f, "%d %d\n", V, E);
    for (int i = 0; i < E; i++)
        fprintf(f, "%d %d %.4f\n", us[i], vs[i], ws[i]);
    fclose(f);
    printf("  %-35s %5d vertices  %7d arestas\n", nome, V, E);
}



int spanning_tree(int V, int *us, int *vs) {
    int *nos = malloc(V * sizeof(int));
    for (int i = 0; i < V; i++) nos[i] = i;
    for (int i = V-1; i > 0; i--) { int j=randi(i+1), t=nos[i]; nos[i]=nos[j]; nos[j]=t; }
    for (int i = 1; i < V; i++) { us[i-1]=nos[randi(i)]; vs[i-1]=nos[i]; }
    free(nos);
    return V - 1;
}

/* ── Verifica se par (u,v) já existe (busca linear) ───────── */

int existe(int *us, int *vs, int E, int u, int v) {
    int lo = u<v?u:v, hi = u<v?v:u;
    for (int i = 0; i < E; i++)
        if ((us[i]==lo && vs[i]==hi) || (us[i]==hi && vs[i]==lo)) return 1;
    return 0;
}

/* ── Insere arestas extras aleatórias ─────────────────────── */

int extras(int V, int *us, int *vs, int base, int qtd) {
    int ins=0, tries=0;
    while (ins < qtd && tries < V*20) {
        int u=randi(V), v=randi(V);
        if (u!=v && !existe(us, vs, base+ins, u, v)) {
            us[base+ins]=u; vs[base+ins]=v; ins++;
        }
        tries++;
    }
    return ins;
}

/* ════════════════════════════════════════════════════════════
   TIPOS DE GRAFO
   ════════════════════════════════════════════════════════════ */

/* Esparso: E ≈ 1.5V, pesos [1, 100] */
void esparso(int V, int s) {
    seed(s);
    int cap = V*2;
    int *us=malloc(cap*sizeof(int)), *vs=malloc(cap*sizeof(int));
    double *ws=malloc(cap*sizeof(double));
    int E = spanning_tree(V, us, vs);
    E += extras(V, us, vs, E, V/2);
    for (int i=0; i<E; i++) ws[i]=uniform(1,100);
    char nome[64]; snprintf(nome,sizeof(nome),"esparso_V%d.txt",V);
    salvar(nome, V, us, vs, ws, E);
    free(us); free(vs); free(ws);
}

/* Denso: E ≈ V²/4 (máx 2M), pesos [1, 100] */
void denso(int V, int s) {
    seed(s+1);
    int qtd = V*(V-1)/4; if (qtd>2000000) qtd=2000000;
    int cap = (V-1)+qtd+10;
    int *us=malloc(cap*sizeof(int)), *vs=malloc(cap*sizeof(int));
    double *ws=malloc(cap*sizeof(double));
    int E = spanning_tree(V, us, vs);
    E += extras(V, us, vs, E, qtd);
    for (int i=0; i<E; i++) ws[i]=uniform(1,100);
    char nome[64]; snprintf(nome,sizeof(nome),"denso_V%d.txt",V);
    salvar(nome, V, us, vs, ws, E);
    free(us); free(vs); free(ws);
}

/* Pesos aleatórios: E ≈ 2V, pesos [0.001, 9999] */
void pesos_aleatorios(int V, int s) {
    seed(s+2);
    int cap = V*3;
    int *us=malloc(cap*sizeof(int)), *vs=malloc(cap*sizeof(int));
    double *ws=malloc(cap*sizeof(double));
    int E = spanning_tree(V, us, vs);
    E += extras(V, us, vs, E, V);
    for (int i=0; i<E; i++) ws[i]=uniform(0.001,9999);
    char nome[64]; snprintf(nome,sizeof(nome),"pesos_aleatorios_V%d.txt",V);
    salvar(nome, V, us, vs, ws, E);
    free(us); free(vs); free(ws);
}

/* Geométrico: peso = distância euclidiana entre pontos 2D */
void geometrico(int V, int s) {
    seed(s+3);
    double *px=malloc(V*sizeof(double)), *py=malloc(V*sizeof(double));
    for (int i=0; i<V; i++) { px[i]=uniform(0,1000); py[i]=uniform(0,1000); }

    double raio = sqrt(log((double)V)*1e6/(M_PI*V))*2.5;
    if (raio < 50) raio = 50;

    int cap = V*50+V; /* estimativa generosa */
    int *us=malloc(cap*sizeof(int)), *vs=malloc(cap*sizeof(int));
    double *ws=malloc(cap*sizeof(double));
    int E=0, lim=V>500?V/2:V;

    for (int i=0; i<V && E<cap-V; i++)
        for (int j=i+1; j<i+1+lim && j<V; j++) {
            double d=sqrt((px[i]-px[j])*(px[i]-px[j])+(py[i]-py[j])*(py[i]-py[j]));
            if (d<=raio) { us[E]=i; vs[E]=j; ws[E]=d; E++; }
        }

    /* fallback: garante conexidade */
    int *nos=malloc(V*sizeof(int));
    for (int i=0; i<V; i++) nos[i]=i;
    for (int i=V-1; i>0; i--) { int j=randi(i+1),t=nos[i]; nos[i]=nos[j]; nos[j]=t; }
    for (int i=1; i<V && E<cap; i++) {
        int u=nos[randi(i)], v=nos[i];
        if (!existe(us,vs,E,u,v)) {
            double d=sqrt((px[u]-px[v])*(px[u]-px[v])+(py[u]-py[v])*(py[u]-py[v]));
            us[E]=u; vs[E]=v; ws[E]=d; E++;
        }
    }
    free(nos);

    char nome[64]; snprintf(nome,sizeof(nome),"geometrico_V%d.txt",V);
    salvar(nome, V, us, vs, ws, E);
    free(us); free(vs); free(ws); free(px); free(py);
}

/* ════════════════════════════════════════════════════════════
   MAIN
   ════════════════════════════════════════════════════════════ */

int main(void) {
    #ifdef _WIN32
    _mkdir("instancias");
    #else
    mkdir("instancias", 0755);
    #endif

    int tamanhos[] = {100, 500, 1000, 5000, 10000};
    int n = sizeof(tamanhos)/sizeof(tamanhos[0]);

    printf("=================================================\n");
    printf(" GERADOR DE INSTANCIAS DE GRAFOS\n");
    printf("=================================================\n");

    for (int t = 0; t < n; t++) {
        int V = tamanhos[t];
        printf("\n[V = %d]\n", V);
        esparso         (V, 42+V);
        denso           (V, 42+V);
        pesos_aleatorios(V, 42+V);
        geometrico      (V, 42+V);
    }

    printf("\n 20 instancias geradas em instancias/\n");
    return 0;
}

