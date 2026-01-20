#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DNA 50000
#define MAX_DOENCAS 200
#define MAX_GENES 100
#define MAX_GENE_LEN 1100
#define MAX_CODE 10

#define HASH_SIZE 131071

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

static Node *hash_table[HASH_SIZE];

unsigned long hash(const char *s) {
    unsigned long h = 5381;
    while (*s)
        h = ((h << 5) + h) + (unsigned char)*s++;
    return h % HASH_SIZE;
}

void hash_insert(const char *s) {
    unsigned long h = hash(s);
    Node *n = hash_table[h];

    while (n) {
        if (strcmp(n->str, s) == 0)
            return;
        n = n->next;
    }

    n = malloc(sizeof(Node));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    n->str = strdup(s);
    if (!n->str) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    n->next = hash_table[h];
    hash_table[h] = n;
}

int hash_contains(const char *s) {
    unsigned long h = hash(s);
    Node *n = hash_table[h];

    while (n) {
        if (strcmp(n->str, s) == 0)
            return 1;
        n = n->next;
    }
    return 0;
}

typedef struct {
    char codigo[MAX_CODE];
    int num_genes;
    char genes[MAX_GENES][MAX_GENE_LEN];
} Doenca;

typedef struct {
    char codigo[MAX_CODE];
    int prob;
    int ordem;
} Resultado;

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr,
            "Uso: %s <arquivo_entrada> <arquivo_saida>\n",
            argv[0]);
        return EXIT_FAILURE;
    }

    FILE *fin = fopen(argv[1], "r");
    if (!fin) {
        perror("Erro ao abrir arquivo de entrada");
        return EXIT_FAILURE;
    }

    FILE *fout = fopen(argv[2], "w");
    if (!fout) {
        perror("Erro ao abrir arquivo de saída");
        fclose(fin);
        return EXIT_FAILURE;
    }

    int k, M;
    char DNA[MAX_DNA];

    fscanf(fin, "%d", &k);
    fscanf(fin, "%s", DNA);
    fscanf(fin, "%d", &M);

    if (M > MAX_DOENCAS) {
        fprintf(stderr, "Erro: numero de doencas excede limite\n");
        fclose(fin);
        fclose(fout);
        return EXIT_FAILURE;
    }

    int nDNA = strlen(DNA);

    Doenca *doencas = malloc(sizeof(Doenca) * M);
    if (!doencas) {
        perror("malloc");
        fclose(fin);
        fclose(fout);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < M; i++) {
        fscanf(fin, "%s %d",
               doencas[i].codigo,
               &doencas[i].num_genes);

        if (doencas[i].num_genes > MAX_GENES) {
            fprintf(stderr, "Erro: numero de genes excede MAX_GENES\n");
            free(doencas);
            fclose(fin);
            fclose(fout);
            return EXIT_FAILURE;
        }

        for (int j = 0; j < doencas[i].num_genes; j++) {
            fscanf(fin, "%s", doencas[i].genes[j]);
        }
    }

    for (int i = 0; i <= nDNA - k; i++) {
        char sub[k + 1];
        memcpy(sub, DNA + i, k);
        sub[k] = '\0';
        hash_insert(sub);
    }

    Resultado resultados[MAX_DOENCAS];
    int total_resultados = 0;

    for (int i = 0; i < M; i++) {
        int genes_detectados = 0;

        for (int g = 0; g < doencas[i].num_genes; g++) {
            char *gene = doencas[i].genes[g];
            int len = strlen(gene);

            if (len < k)
                continue;

            int total_sub = len - k + 1;
            int hits = 0;
            int limite = (int)ceil(0.9 * total_sub);

            for (int p = 0; p <= len - k; p++) {
                if (hits + (total_sub - p) < limite)
                    break;

                char sub[k + 1];
                memcpy(sub, gene + p, k);
                sub[k] = '\0';

                if (hash_contains(sub))
                    hits++;
            }

            if ((double)hits / total_sub >= 0.9)
                genes_detectados++;
        }

        if (genes_detectados > 0) {
            int prob = (int)round(
                (double)genes_detectados * 100.0 /
                doencas[i].num_genes
            );

            if (prob > 0) {
                strcpy(resultados[total_resultados].codigo,
                    doencas[i].codigo);
                resultados[total_resultados].prob = prob;
                resultados[total_resultados].ordem = i;
                total_resultados++;
            }
        }

    }

    for (int i = 0; i < total_resultados; i++) {
        for (int j = i + 1; j < total_resultados; j++) {
            if (resultados[j].prob > resultados[i].prob ||
            (resultados[j].prob == resultados[i].prob &&
                resultados[j].ordem < resultados[i].ordem)) {

                Resultado tmp = resultados[i];
                resultados[i] = resultados[j];
                resultados[j] = tmp;
            }
        }
    }

    for (int i = 0; i < total_resultados; i++) {
        fprintf(fout, "%s->%d%%\n",
                resultados[i].codigo,
                resultados[i].prob);
    }

    free(doencas);
    fclose(fin);
    fclose(fout);

    return EXIT_SUCCESS;
}
