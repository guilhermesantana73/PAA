#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char placa[8];
    int cap_peso;
    int cap_vol;
} Veiculo;

typedef struct {
    char codigo[14];
    double valor;
    int peso;
    int vol;
    int foi_usado;
} Pacote;

/* quicksort sobre um array de índices 'arr' referenciando 'lista' */
void quicksort_indices(Pacote* lista, int* arr, int left, int right) {
    if (left >= right) return;
    int i = left, j = right;
    int mid = (left + right) / 2;
    int pivot_idx = arr[mid];
    const char *pivot_code = lista[pivot_idx].codigo;

    while (i <= j) {
        while (strcmp(lista[arr[i]].codigo, pivot_code) < 0) i++;
        while (strcmp(lista[arr[j]].codigo, pivot_code) > 0) j--;
        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++; j--;
        }
    }
    if (left < j) quicksort_indices(lista, arr, left, j);
    if (i < right) quicksort_indices(lista, arr, i, right);
}

void resolver_mochila_2d(Veiculo caminhao, Pacote* lista, int total, FILE* saida) {
    int P = caminhao.cap_peso;
    int V = caminhao.cap_vol;

    /* alocação contígua para melhor desempenho */
    size_t cells = (size_t)(P + 1) * (size_t)(V + 1);
    double *DP = calloc(cells, sizeof(double));
    int *escolha = malloc(cells * sizeof(int));
    if (!DP || !escolha) {
        fprintf(stderr, "Erro de alocacao\n");
        free(DP); free(escolha);
        return;
    }
    for (size_t i = 0; i < cells; ++i) escolha[i] = -1;

    #define DP_AT(pp,vv) DP[(size_t)(pp) * (V + 1) + (vv)]
    #define ESCOLHA_AT(pp,vv) escolha[(size_t)(pp) * (V + 1) + (vv)]

    /* preencher DP */
    for (int i = 0; i < total; i++) {
        if (lista[i].foi_usado) continue;

        int peso = lista[i].peso;
        int vol  = lista[i].vol;
        double val = lista[i].valor;

        if (peso > P || vol > V) continue; /* item não cabe de jeito nenhum */

        for (int p = P; p >= peso; p--) {
            /* otimização: calcular linha base uma vez por p */
            for (int v = V; v >= vol; v--) {
                double novo = DP_AT(p - peso, v - vol) + val;
                if (novo > DP_AT(p, v)) {
                    DP_AT(p, v) = novo;
                    ESCOLHA_AT(p, v) = i;
                }
            }
        }
    }

    /* encontrar ponto inicial para backtracking:
       preferir (P,V) se tiver escolha; caso contrário, procurar o par (p,v) com maior DP */
    int p = P, v = V;
    if (ESCOLHA_AT(p, v) < 0) {
        double best = 0.0;
        int bp = 0, bv = 0;
        for (int pp = 0; pp <= P; ++pp) {
            for (int vv = 0; vv <= V; ++vv) {
                if (DP_AT(pp, vv) > best) {
                    best = DP_AT(pp, vv);
                    bp = pp; bv = vv;
                }
            }
        }
        p = bp; v = bv;
    }

    int *usados = malloc(total * sizeof(int));
    if (!usados) { free(DP); free(escolha); return; }
    int qtd = 0;

    /* backtracking seguro */
    while (p > 0 && v > 0) {
        int idx = ESCOLHA_AT(p, v);
        if (idx < 0) break;
        if (qtd >= total) break; /* proteção */

        int peso = lista[idx].peso;
        int vol  = lista[idx].vol;

        /* proteção: se subtrair tornaria p ou v negativos, aborta */
        if (p - peso < 0 || v - vol < 0) break;

        usados[qtd++] = idx;
        lista[idx].foi_usado = 1;

        p -= peso;
        v -= vol;
    }

    /* ordenar os códigos selecionados (se houver mais de 1) */
    if (qtd > 1) quicksort_indices(lista, usados, 0, qtd - 1);

    /* calcular ocupação e valor final */
    int peso_oc = 0, vol_oc = 0;
    for (int k = 0; k < qtd; k++) {
        peso_oc += lista[usados[k]].peso;
        vol_oc  += lista[usados[k]].vol;
    }

    /* valor final: DP[P][V] pode não ser o máximo se escolhemos outro (p,v) no backtracking,
       então recalculamos o valor final como soma dos valores dos usados (mais robusto) */
    double valor_final = 0.0;
    for (int k = 0; k < qtd; k++) valor_final += lista[usados[k]].valor;

    int perc_peso = (int)((peso_oc * 100.0 / caminhao.cap_peso) + 0.5);
    int perc_vol  = (int)((vol_oc  * 100.0 / caminhao.cap_vol ) + 0.5);

    fprintf(saida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)",
            caminhao.placa, valor_final,
            peso_oc, perc_peso,
            vol_oc, perc_vol);

    if (qtd > 0) {
        fprintf(saida, "->");
        for (int k = 0; k < qtd; k++) {
            fprintf(saida, "%s", lista[usados[k]].codigo);
            if (k < qtd - 1) fprintf(saida, ",");
        }
    }
    fprintf(saida, "\n");

    /* liberar memória */
    free(DP);
    free(escolha);
    free(usados);

    #undef DP_AT
    #undef ESCOLHA_AT
}

int main(int argc, char *argv[]) {
    FILE *entrada;
    FILE *saida;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }

    entrada = fopen(argv[1], "r");
    if (!entrada) { perror("Erro entrada"); return 1; }

    saida = fopen(argv[2], "w");
    if (!saida) { perror("Erro saida"); fclose(entrada); return 1; }

    int num_veiculos;
    if (fscanf(entrada, "%d", &num_veiculos) != 1) { fclose(entrada); fclose(saida); return 1; }
    Veiculo* frota = malloc((size_t)num_veiculos * sizeof(Veiculo));
    if (!frota) { fclose(entrada); fclose(saida); return 1; }

    for (int i = 0; i < num_veiculos; i++) {
        /* leitura segura: placa tem 7 chars + '\0' */
        if (fscanf(entrada, "%7s %d %d", frota[i].placa, &frota[i].cap_peso, &frota[i].cap_vol) != 3) {
            free(frota); fclose(entrada); fclose(saida); return 1;
        }
    }

    int num_pacotes;
    if (fscanf(entrada, "%d", &num_pacotes) != 1) { free(frota); fclose(entrada); fclose(saida); return 1; }
    Pacote* estoque = malloc((size_t)num_pacotes * sizeof(Pacote));
    if (!estoque) { free(frota); fclose(entrada); fclose(saida); return 1; }

    for (int i = 0; i < num_pacotes; i++) {
        /* leitura segura: codigo tem 13 chars + '\0' */
        if (fscanf(entrada, "%13s %lf %d %d",
                   estoque[i].codigo, &estoque[i].valor, &estoque[i].peso, &estoque[i].vol) != 4) {
            free(frota); free(estoque); fclose(entrada); fclose(saida); return 1;
        }
        estoque[i].foi_usado = 0;
    }

    for (int i = 0; i < num_veiculos; i++) {
        resolver_mochila_2d(frota[i], estoque, num_pacotes, saida);
    }

    /* pendentes */
    int* pendentes = malloc((size_t)num_pacotes * sizeof(int));
    if (!pendentes) { free(frota); free(estoque); fclose(entrada); fclose(saida); return 1; }
    int qtd_pend = 0;
    double val_pend = 0.0;
    int peso_pend = 0, vol_pend = 0;

    for (int i = 0; i < num_pacotes; i++) {
        if (!estoque[i].foi_usado) {
            pendentes[qtd_pend++] = i;
            val_pend += estoque[i].valor;
            peso_pend += estoque[i].peso;
            vol_pend += estoque[i].vol;
        }
    }

    if (qtd_pend > 1) quicksort_indices(estoque, pendentes, 0, qtd_pend - 1);

    if (qtd_pend > 0) {
        fprintf(saida, "PENDENTE:R$%.2f,%dKG,%dL->", val_pend, peso_pend, vol_pend);
        for (int i = 0; i < qtd_pend; i++) {
            fprintf(saida, "%s", estoque[pendentes[i]].codigo);
            if (i < qtd_pend - 1) fprintf(saida, ",");
        }
        fprintf(saida, "\n");
    }

    free(frota);
    free(estoque);
    free(pendentes);
    fclose(entrada);
    fclose(saida);

    return 0;
}
