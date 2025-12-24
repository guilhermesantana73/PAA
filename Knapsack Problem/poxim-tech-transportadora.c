#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

typedef struct {
    int idx;          /* índice do pacote */
    double densidade; /* valor / (peso + volume) */
} ItemOrdenado;

void resolver_mochila_2d(Veiculo caminhao, Pacote* lista, int total, FILE* saida) {
    int P = caminhao.cap_peso;
    int V = caminhao.cap_vol;

    /* 1. Filtragem: Criar lista apenas com itens que cabem no caminhão */
    /* Alocação dinâmica para evitar estouro de buffer se total > 1000 */
    int *idx = malloc(total * sizeof(int));
    if (!idx) return; // Erro de alocação

    int n = 0;
    for (int i = 0; i < total; i++) {
        if (!lista[i].foi_usado &&
            lista[i].peso <= P &&
            lista[i].vol  <= V) {
            idx[n++] = i;
        }
    }

    /* 2. Tabela DP 3D Linearizada 
       Tamanho: (n+1) * (P+1) * (V+1)
       Usamos calloc para garantir que tudo inicie com 0.0
       Representa: dp[item_i][peso_j][vol_k]
    */
    size_t sz_p = (size_t)(P + 1);
    size_t sz_v = (size_t)(V + 1);
    
    double *dp = calloc((n + 1) * sz_p * sz_v, sizeof(double));
    if (!dp) { free(idx); return; }

    /* Macro para acessar o array 1D como se fosse 3D: dp[k][p][v] */
    #define DP(k, p, v) dp[(k) * sz_p * sz_v + (p) * sz_v + (v)]

    /* 3. Preenchimento da DP */
    for (int k = 1; k <= n; k++) {
        int i = idx[k-1]; // Índice real no array 'lista'
        int w = lista[i].peso;
        int vol = lista[i].vol;
        double val = lista[i].valor;

        for (int p = 0; p <= P; p++) {
            for (int v = 0; v <= V; v++) {
                // Opção 1: Não levar o item k (copia o valor do estado anterior k-1)
                double nao_leva = DP(k - 1, p, v);
                
                // Opção 2: Levar o item k (se couber)
                double leva = 0.0;
                if (p >= w && v >= vol) {
                    leva = DP(k - 1, p - w, v - vol) + val;
                }

                // Armazena o melhor dos dois
                if (leva > nao_leva) {
                    DP(k, p, v) = leva;
                } else {
                    DP(k, p, v) = nao_leva;
                }
            }
        }
    }

    /* 4. Encontrar o melhor ponto final (pode não ser a capacidade máxima exata) */
    // Na mochila 0/1 clássica, o valor ótimo está sempre em DP(n, P, V) se os pesos forem exatos,
    // mas vamos varrer para garantir o maior valor global dentro dos limites.
    double best_val = -1.0;
    int cur_p = P;
    int cur_v = V;
    
    // O valor em DP(n, P, V) já contém o máximo acumulado considerando limites P e V.
    best_val = DP(n, P, V);

    /* Otimização de Backtracking:
       Em vez de varrer tudo para achar o 'best_val', sabemos que DP[n][P][V] contém 
       o melhor valor possível dado o espaço disponível, pois a lógica de "copiar o anterior"
       propaga os melhores valores para frente.
    */

    /* 5. Backtracking (Recuperar os itens) */
    int *usados = malloc(n * sizeof(int)); // Lista temporária de usados
    int qtd = 0;

    // Começamos do último item (n) e da capacidade máxima (P, V)
    // E vamos voltando
    for (int k = n; k > 0; k--) {
        int i = idx[k-1];
        int w = lista[i].peso;
        int vol = lista[i].vol;
        double val = lista[i].valor;

        // Verifica se o valor mudou em relação a não ter o item (k-1)
        // Se DP(k, p, v) != DP(k-1, p, v), significa que o item k foi adicionado
        double val_sem = DP(k - 1, cur_p, cur_v);
        double val_com = DP(k, cur_p, cur_v);

        // Usamos uma pequena margem de erro para double
        if (fabs(val_com - val_sem) > 1e-6) {
            // Item foi usado
            usados[qtd++] = i;
            lista[i].foi_usado = 1;
            cur_p -= w;
            cur_v -= vol;
        }
    }

    /* 6. Preparar Resultados para Impressão */
    int peso_oc = 0, vol_oc = 0;
    double valor_total = 0.0;

    for (int i = 0; i < qtd; i++) {
        peso_oc += lista[usados[i]].peso;
        vol_oc  += lista[usados[i]].vol;
        valor_total += lista[usados[i]].valor;
    }

    // Calcular percentuais
    int perc_peso = (caminhao.cap_peso > 0) ? (int)((peso_oc * 100.0 / caminhao.cap_peso) + 0.5) : 0;
    int perc_vol  = (caminhao.cap_vol > 0)  ? (int)((vol_oc  * 100.0 / caminhao.cap_vol ) + 0.5) : 0;

    fprintf(saida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)",
            caminhao.placa, valor_total,
            peso_oc, perc_peso,
            vol_oc, perc_vol);

    if (qtd > 0) {
        fprintf(saida, "->");
        // O array 'usados' foi preenchido de trás para frente (do item n ao 1)
        // Mas a ordem de inserção na lista 'usados' seguiu o backtracking.
        // O output esperado parece listar em ordem de processamento ou inversa.
        // Vamos manter a ordem que você tinha (iterando inverso do preenchimento).
        for (int i = 0; i < qtd; i++) {
            fprintf(saida, "%s", lista[usados[i]].codigo);
            if (i < qtd - 1) fprintf(saida, ",");
        }
    }
    fprintf(saida, "\n");

    /* Limpeza */
    #undef DP
    free(dp);
    free(idx);
    free(usados);
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
