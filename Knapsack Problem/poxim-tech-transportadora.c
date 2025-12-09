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

double max_double(double a, double b) {
    return (a > b) ? a : b;
}

void resolver_mochila_3d(Veiculo caminhao, Pacote* lista_pacotes, int total_pacotes, FILE* saida) {
    
    int max_peso = caminhao.cap_peso;
    int max_vol = caminhao.cap_vol;

    int tamanho_tabela = (total_pacotes + 1) * (max_peso + 1) * (max_vol + 1);
    double* tabela = (double*)calloc(tamanho_tabela, sizeof(double));

    if (tabela == NULL) {
        fprintf(stderr, "Erro: Memoria insuficiente.\n");
        return;
    }

    #define DP(i, p, v) tabela[(i) * (max_peso + 1) * (max_vol + 1) + (p) * (max_vol + 1) + (v)]

    for (int i = 1; i <= total_pacotes; i++) {
        int idx = i - 1; 
        
        int p_item = lista_pacotes[idx].peso;
        int v_item = lista_pacotes[idx].vol;
        double val_item = lista_pacotes[idx].valor;
        int usado = lista_pacotes[idx].foi_usado;

        for (int p = 0; p <= max_peso; p++) {
            for (int v = 0; v <= max_vol; v++) {
                
                if (usado) {
                    DP(i, p, v) = DP(i - 1, p, v);
                    continue;
                }

                if (p_item <= p && v_item <= v) {
                    double inclui = val_item + DP(i - 1, p - p_item, v - v_item);
                    double n_inclui = DP(i - 1, p, v);
                    DP(i, p, v) = max_double(inclui, n_inclui);
                } else {
                    DP(i, p, v) = DP(i - 1, p, v);
                }
            }
        }
    }

    int p_atual = max_peso;
    int v_atual = max_vol;
    double valor_final = DP(total_pacotes, max_peso, max_vol);
    
    int peso_oc = 0;
    int vol_oc = 0;

    int* escolhidos = (int*)malloc(total_pacotes * sizeof(int));
    int qtd = 0;

    for (int i = total_pacotes; i > 0; i--) {
        int idx = i - 1;
        if (!lista_pacotes[idx].foi_usado && 
            DP(i, p_atual, v_atual) != DP(i - 1, p_atual, v_atual)) {
            
            escolhidos[qtd++] = idx;
            lista_pacotes[idx].foi_usado = 1; 
            
            peso_oc += lista_pacotes[idx].peso;
            vol_oc += lista_pacotes[idx].vol;
            
            p_atual -= lista_pacotes[idx].peso;
            v_atual -= lista_pacotes[idx].vol;
        }
    }

    for (int i = 0; i < qtd - 1; i++) {
        for (int j = 0; j < qtd - i - 1; j++) {
            if (strcmp(lista_pacotes[escolhidos[j]].codigo, lista_pacotes[escolhidos[j+1]].codigo) > 0) {
                int temp = escolhidos[j];
                escolhidos[j] = escolhidos[j+1];
                escolhidos[j+1] = temp;
            }
        }
    }

    int perc_peso = (int)(((peso_oc * 100.0) / caminhao.cap_peso) + 0.5);
    int perc_vol = (int)(((vol_oc * 100.0) / caminhao.cap_vol) + 0.5);

    fprintf(saida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)", 
            caminhao.placa, 
            valor_final, 
            peso_oc, perc_peso, 
            vol_oc, perc_vol);

    if (qtd > 0) {
        fprintf(saida, "->");
        for (int k = 0; k < qtd; k++) {
            fprintf(saida, "%s", lista_pacotes[escolhidos[k]].codigo);
            if (k < qtd - 1) fprintf(saida, ",");
        }
    }
    fprintf(saida, "\n");

    free(tabela);
    free(escolhidos);
    #undef DP
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
    fscanf(entrada, "%d", &num_veiculos);
    Veiculo* frota = (Veiculo*)malloc(num_veiculos * sizeof(Veiculo));
    for(int i=0; i<num_veiculos; i++){
        fscanf(entrada, "%s %d %d", frota[i].placa, &frota[i].cap_peso, &frota[i].cap_vol);
    }

    int num_pacotes;
    fscanf(entrada, "%d", &num_pacotes);
    Pacote* estoque = (Pacote*)malloc(num_pacotes * sizeof(Pacote));
    for(int i=0; i<num_pacotes; i++){
        fscanf(entrada, "%s %lf %d %d", 
               estoque[i].codigo, &estoque[i].valor, &estoque[i].peso, &estoque[i].vol);
        estoque[i].foi_usado = 0;
    }

    for(int i=0; i<num_veiculos; i++){
        resolver_mochila_3d(frota[i], estoque, num_pacotes, saida);
    }

    double val_pend = 0;
    int peso_pend = 0;
    int vol_pend = 0;
    int qtd_pend = 0;
    
    int* pendentes_indices = (int*)malloc(num_pacotes * sizeof(int));

    for(int i=0; i<num_pacotes; i++){
        if(!estoque[i].foi_usado){
            val_pend += estoque[i].valor;
            peso_pend += estoque[i].peso;
            vol_pend += estoque[i].vol;
            pendentes_indices[qtd_pend++] = i;
        }
    }

    for (int i = 0; i < qtd_pend - 1; i++) {
        for (int j = 0; j < qtd_pend - i - 1; j++) {
            if (strcmp(estoque[pendentes_indices[j]].codigo, estoque[pendentes_indices[j+1]].codigo) > 0) {
                int temp = pendentes_indices[j];
                pendentes_indices[j] = pendentes_indices[j+1];
                pendentes_indices[j+1] = temp;
            }
        }
    }

    if (qtd_pend > 0) {
        fprintf(saida, "PENDENTE:R$%.2f,%dKG,%dL->", val_pend, peso_pend, vol_pend);
        for(int i=0; i<qtd_pend; i++){
            fprintf(saida, "%s", estoque[pendentes_indices[i]].codigo);
            if(i < qtd_pend - 1) fprintf(saida, ",");
        }
        fprintf(saida, "\n");
    }

    free(frota);
    free(estoque);
    free(pendentes_indices);
    fclose(entrada);
    fclose(saida);

    return 0;
}