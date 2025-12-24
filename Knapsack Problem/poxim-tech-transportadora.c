#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Estruturas de Dados */
typedef struct {
    char placa[8]; // 7 chars + \0
    int cap_peso;
    int cap_vol;
} Veiculo;

typedef struct {
    char codigo[14]; // 13 chars + \0
    double valor;
    int peso;
    int vol;
    int foi_usado; // Flag para marcar se já foi carregado em algum caminhão
} Pacote;

/* * Função Principal da Lógica (Knapsack 2D com Recuperação de Caminho)
 */
void resolver_mochila_2d(Veiculo caminhao, Pacote* lista, int total, FILE* saida) {
    int P = caminhao.cap_peso;
    int V = caminhao.cap_vol;

    /* 1. Filtragem: Criar lista de indíces apenas com itens que cabem no veículo e não foram usados */
    int *idx = malloc((size_t)total * sizeof(int));
    if (!idx) {
        fprintf(stderr, "Erro de memoria (idx)\n");
        return;
    }

    int n = 0;
    for (int i = 0; i < total; i++) {
        if (!lista[i].foi_usado &&
            lista[i].peso <= P &&
            lista[i].vol  <= V) {
            idx[n++] = i;
        }
    }

    /* * 2. Alocação Dinâmica da Tabela DP (Linearizada)
     * Dimensões virtuais: [n+1][P+1][V+1]
     * Usamos calloc para iniciar tudo com 0.0
     */
    size_t sz_p = (size_t)(P + 1);
    size_t sz_v = (size_t)(V + 1);
    
    // Proteção contra veículos com capacidade 0 ou sem itens
    if (n == 0 || P == 0 || V == 0) {
        fprintf(saida, "[%s]R$0.00,0KG(0%%),0L(0%%)\n", caminhao.placa);
        free(idx);
        return;
    }

    double *dp = calloc((size_t)(n + 1) * sz_p * sz_v, sizeof(double));
    if (!dp) {
        fprintf(stderr, "Erro de memoria (dp)\n");
        free(idx);
        return;
    }

    /* Macro para acesso fácil: dp[k][p][v] */
    #define DP(k, p, v) dp[(k) * sz_p * sz_v + (p) * sz_v + (v)]

    /* 3. Preenchimento da Tabela (Bottom-Up) */
    for (int k = 1; k <= n; k++) {
        int real_i = idx[k-1]; 
        int w = lista[real_i].peso;
        int vol = lista[real_i].vol;
        double val = lista[real_i].valor;

        for (int p = 0; p <= P; p++) {
            for (int v = 0; v <= V; v++) {
                // Opção A: Não levar o item k (mantém o valor anterior)
                double nao_leva = DP(k - 1, p, v);
                
                // Opção B: Levar o item k (se couber)
                double leva = 0.0;
                if (p >= w && v >= vol) {
                    leva = DP(k - 1, p - w, v - vol) + val;
                }

                // Escolhe o melhor
                if (leva > nao_leva) {
                    DP(k, p, v) = leva;
                } else {
                    DP(k, p, v) = nao_leva;
                }
            }
        }
    }

    /* 4. Backtracking (Recuperação dos itens escolhidos) */
    int *usados = malloc((size_t)n * sizeof(int));
    if (!usados) {
        free(dp); free(idx); return;
    }
    
    int qtd = 0;
    int cur_p = P;
    int cur_v = V;

    // Percorre do último item analisado até o primeiro
    for (int k = n; k > 0; k--) {
        int real_i = idx[k-1];
        int w = lista[real_i].peso;
        int vol = lista[real_i].vol;
        double val = lista[real_i].valor;

        double val_com_item = DP(k, cur_p, cur_v);
        double val_sem_item = DP(k - 1, cur_p, cur_v);

        // Se o valor mudou ao considerar o item k, então ele foi escolhido
        // Usamos uma tolerância pequena para float (1e-6)
        if (fabs(val_com_item - val_sem_item) > 1e-6) {
            usados[qtd++] = real_i;
            lista[real_i].foi_usado = 1;
            cur_p -= w;
            cur_v -= vol;
        }
    }

    /* 5. Calcular totais e imprimir */
    int peso_oc = 0, vol_oc = 0;
    double valor_total = 0.0;

    for (int i = 0; i < qtd; i++) {
        peso_oc += lista[usados[i]].peso;
        vol_oc  += lista[usados[i]].vol;
        valor_total += lista[usados[i]].valor;
    }

    int perc_peso = (int)((peso_oc * 100.0 / caminhao.cap_peso) + 0.5);
    int perc_vol  = (int)((vol_oc  * 100.0 / caminhao.cap_vol ) + 0.5);

    fprintf(saida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)",
            caminhao.placa, valor_total,
            peso_oc, perc_peso,
            vol_oc, perc_vol);

    if (qtd > 0) {
        fprintf(saida, "->");
        /* * CORREÇÃO DA ORDEM DE IMPRESSÃO:
         * O backtracking insere na ordem (Último -> Primeiro).
         * Para imprimir na ordem (Primeiro -> Último), percorremos o array 'usados' de trás para frente.
         */
        for (int i = qtd - 1; i >= 0; i--) {
            fprintf(saida, "%s", lista[usados[i]].codigo);
            if (i > 0) fprintf(saida, ",");
        }
    }
    fprintf(saida, "\n");

    /* Limpeza */
    #undef DP
    free(usados);
    free(dp);
    free(idx);
}

int main(int argc, char *argv[]) {
    /* Checagem de Argumentos */
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <entrada.txt> <saida.txt>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if (!entrada) { 
        perror("Erro ao abrir arquivo de entrada"); 
        return 1; 
    }

    FILE *saida = fopen(argv[2], "w");
    if (!saida) { 
        perror("Erro ao criar arquivo de saida"); 
        fclose(entrada); 
        return 1; 
    }

    /* Leitura da Frota */
    int num_veiculos;
    if (fscanf(entrada, "%d", &num_veiculos) != 1) {
        fprintf(stderr, "Erro: Arquivo de entrada vazio ou formato invalido (num_veiculos).\n");
        fclose(entrada); fclose(saida); return 1;
    }

    Veiculo* frota = malloc((size_t)num_veiculos * sizeof(Veiculo));
    if (!frota) {
        fprintf(stderr, "Erro fatal: Falha de alocacao (frota).\n");
        fclose(entrada); fclose(saida); return 1;
    }

    for (int i = 0; i < num_veiculos; i++) {
        if (fscanf(entrada, "%7s %d %d", frota[i].placa, &frota[i].cap_peso, &frota[i].cap_vol) != 3) {
            fprintf(stderr, "Erro: Falha ao ler dados do veiculo %d.\n", i+1);
            free(frota); fclose(entrada); fclose(saida); return 1;
        }
    }

    /* Leitura do Estoque */
    int num_pacotes;
    if (fscanf(entrada, "%d", &num_pacotes) != 1) {
        fprintf(stderr, "Erro: Falha ao ler numero de pacotes.\n");
        free(frota); fclose(entrada); fclose(saida); return 1;
    }

    Pacote* estoque = malloc((size_t)num_pacotes * sizeof(Pacote));
    if (!estoque) {
        fprintf(stderr, "Erro fatal: Falha de alocacao (estoque).\n");
        free(frota); fclose(entrada); fclose(saida); return 1;
    }

    for (int i = 0; i < num_pacotes; i++) {
        if (fscanf(entrada, "%13s %lf %d %d",
                   estoque[i].codigo, &estoque[i].valor, &estoque[i].peso, &estoque[i].vol) != 4) {
            fprintf(stderr, "Erro: Falha ao ler pacote %d.\n", i+1);
            free(frota); free(estoque); fclose(entrada); fclose(saida); return 1;
        }
        estoque[i].foi_usado = 0;
    }

    /* Processamento */
    printf("Iniciando processamento de %d veiculos e %d pacotes...\n", num_veiculos, num_pacotes);
    
    for (int i = 0; i < num_veiculos; i++) {
        resolver_mochila_2d(frota[i], estoque, num_pacotes, saida);
    }

    /* Relatório de Pendentes */
    int* pendentes = malloc((size_t)num_pacotes * sizeof(int));
    if (pendentes) {
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
        free(pendentes);
    } else {
        fprintf(stderr, "Aviso: Falha ao alocar memoria para relatorio de pendentes.\n");
    }

    /* Finalização */
    printf("Concluido! Verifique o arquivo: %s\n", argv[2]);

    free(frota);
    free(estoque);
    fclose(entrada);
    fclose(saida);

    return 0;
}