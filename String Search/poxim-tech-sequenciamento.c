#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DNA 100000
#define MAX_GENE 1100
#define MAX_DOENCAS 1000
#define MAX_CODIGO 16

typedef struct {
    char codigo[MAX_CODIGO];
    int prob;
    int ordem;
} Doenca;

/* ================= KMP ================= */

void calcular_tabela(int *k, const char *P, int m) {
    k[0] = -1;
    for (int i = 1, j = -1; i < m; i++) {
        while (j >= 0 && P[j + 1] != P[i])
            j = k[j];
        if (P[j + 1] == P[i])
            j++;
        k[i] = j;
    }
}

int KMP_existe(const char *T, int n, const char *P, int m) {
    int k[MAX_GENE];
    calcular_tabela(k, P, m);

    for (int i = 0, j = -1; i < n; i++) {
        while (j >= 0 && P[j + 1] != T[i])
            j = k[j];
        if (P[j + 1] == T[i])
            j++;
        if (j == m - 1)
            return 1;
    }
    return 0;
}

/* ================= ORDENAÇÃO ESTÁVEL ================= */

void ordenar_estavel(Doenca *v, int n) {
    for (int i = 1; i < n; i++) {
        Doenca atual = v[i];
        int j = i - 1;
        while (j >= 0 &&
              (v[j].prob < atual.prob ||
              (v[j].prob == atual.prob &&
               v[j].ordem > atual.ordem))) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = atual;
    }
}

/* ================= MAIN ================= */

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <entrada.txt> <saida.txt>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        perror("Erro ao abrir arquivo de saída");
        fclose(in);
        return 1;
    }

    int k;
    static char DNA[MAX_DNA];

    fscanf(in, "%d", &k);
    fscanf(in, "%s", DNA);
    int n_dna = strlen(DNA);

    int numDoencas;
    fscanf(in, "%d", &numDoencas);

    Doenca doencas[MAX_DOENCAS];

    for (int d = 0; d < numDoencas; d++) {
        int numGenes;
        fscanf(in, "%s %d", doencas[d].codigo, &numGenes);
        doencas[d].ordem = d;

        int genes_detectados = 0;

        for (int g = 0; g < numGenes; g++) {
            char gene[MAX_GENE];
            fscanf(in, "%s", gene);
            int m = strlen(gene);

            if (k > m) continue;

            char coberto[MAX_GENE] = {0};

            for (int i = 0; i <= m - k; i++) {
                char sub[MAX_GENE];
                memcpy(sub, gene + i, k);
                sub[k] = '\0';

                if (KMP_existe(DNA, n_dna, sub, k)) {
                    for (int x = 0; x < k; x++)
                        coberto[i + x] = 1;
                }
            }

            int cont = 0;
            for (int i = 0; i < m; i++)
                cont += coberto[i];

            if ((double)cont / m >= 0.9)
                genes_detectados++;
        }

        if (numGenes == 0)
            doencas[d].prob = 0;
        else
            doencas[d].prob =
                (int)((double)genes_detectados / numGenes * 100 + 0.5);
    }

    ordenar_estavel(doencas, numDoencas);

    for (int i = 0; i < numDoencas; i++)
        fprintf(out, "%s->%d%%\n", doencas[i].codigo, doencas[i].prob);

    fclose(in);
    fclose(out);
    return 0;
}
