#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ===================== LIMITES ===================== */

#define MAX_DNA 50000
#define MAX_DOENCAS 200
#define MAX_GENES 100
#define MAX_GENE_LEN 1100
#define MAX_CODE 10

/* ===================== HASH TABLE ===================== */

#define HASH_SIZE 131071  /* primo > 2 * 45000 */

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

/* ===================== ESTRUTURAS ===================== */

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

/* ===================== MAIN ===================== */

int main(void) {
    int k;
    char DNA[MAX_DNA];
    int M;

    if (scanf("%d", &k) != 1) return 1;
    if (scanf("%s", DNA) != 1) return 1;
    if (scanf("%d", &M) != 1) return 1;

    if (M > MAX_DOENCAS) {
        fprintf(stderr, "Erro: numero de doencas excede MAX_DOENCAS\n");
        return 1;
    }

    int nDNA = strlen(DNA);

    /* ===== ALOCAÇÃO NO HEAP (CRÍTICA) ===== */

    Doenca *doencas = malloc(sizeof(Doenca) * M);
    if (!doencas) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < M; i++) {
        scanf("%s %d", doencas[i].codigo, &doencas[i].num_genes);

        if (doencas[i].num_genes > MAX_GENES) {
            fprintf(stderr, "Erro: numero de genes excede MAX_GENES\n");
            free(doencas);
            return 1;
        }

        for (int j = 0; j < doencas[i].num_genes; j++) {
            scanf("%s", doencas[i].genes[j]);
        }
    }

    /* ===================== PRÉ-PROCESSAMENTO DO DNA ===================== */

    for (int i = 0; i <= nDNA - k; i++) {
        char sub[k + 1];
        memcpy(sub, DNA + i, k);
        sub[k] = '\0';
        hash_insert(sub);
    }

    Resultado resultados[MAX_DOENCAS];
    int total_resultados = 0;

    /* ===================== PROCESSAMENTO ===================== */

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
                (double)genes_detectados / doencas[i].num_genes * 100.0
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

    /* ===================== ORDENAÇÃO ESTÁVEL ===================== */

    for (int i = 0; i < total_resultados; i++) {
        for (int j = i + 1; j < total_resultados; j++) {
            if (resultados[j].prob > resultados[i].prob) {
                Resultado tmp = resultados[i];
                resultados[i] = resultados[j];
                resultados[j] = tmp;
            }
        }
    }

    /* ===================== SAÍDA ===================== */

    for (int i = 0; i < total_resultados; i++) {
        printf("%s->%d%%\n",
               resultados[i].codigo,
               resultados[i].prob);
    }

    /* ===================== LIMPEZA ===================== */

    free(doencas);
    /* (hash não precisa ser liberada para o programa terminar corretamente) */

    return 0;
}
