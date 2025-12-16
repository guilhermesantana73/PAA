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

int comparar_codigos(Pacote* lista, int a, int b) {
    return strcmp(lista[a].codigo, lista[b].codigo);
}

void quicksort_indices(Pacote* lista, int* arr, int left, int right) {
    int i = left, j = right;
    int pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (comparar_codigos(lista, arr[i], pivot) < 0) i++;
        while (comparar_codigos(lista, arr[j], pivot) > 0) j--;
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

    double** DP = malloc((P + 1) * sizeof(double*));
    int** escolha = malloc((P + 1) * sizeof(int*));

    for (int p = 0; p <= P; p++) {
        DP[p] = calloc(V + 1, sizeof(double));
        escolha[p] = malloc((V + 1) * sizeof(int));
        for (int v = 0; v <= V; v++) escolha[p][v] = -1;
    }

    for (int i = 0; i < total; i++) {
        if (lista[i].foi_usado) continue;

        int peso = lista[i].peso;
        int vol  = lista[i].vol;
        double val = lista[i].valor;

        for (int p = P; p >= peso; p--) {
            for (int v = V; v >= vol; v--) {
                double novo = DP[p - peso][v - vol] + val;
                if (novo > DP[p][v]) {
                    DP[p][v] = novo;
                    escolha[p][v] = i;
                }
            }
        }
    }

    int p = P, v = V;
    int* usados = malloc(total * sizeof(int));
    int qtd = 0;

    while (p >= 0 && v >= 0) {
        int idx = escolha[p][v];
        if (idx < 0) break;

        usados[qtd++] = idx;
        lista[idx].foi_usado = 1;

        p -= lista[idx].peso;
        v -= lista[idx].vol;
    }

    if (qtd > 1) quicksort_indices(lista, usados, 0, qtd - 1);

    int peso_oc = 0, vol_oc = 0;
    double valor_final = DP[P][V];

    for (int k = 0; k < qtd; k++) {
        peso_oc += lista[usados[k]].peso;
        vol_oc  += lista[usados[k]].vol;
    }

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

    for (int p2 = 0; p2 <= P; p2++) {
        free(DP[p2]);
        free(escolha[p2]);
    }
    free(DP);
    free(escolha);
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
    fscanf(entrada, "%d", &num_veiculos);
    Veiculo* frota = malloc(num_veiculos * sizeof(Veiculo));
    for(int i=0; i<num_veiculos; i++){
        fscanf(entrada, "%s %d %d", frota[i].placa, &frota[i].cap_peso, &frota[i].cap_vol);
    }

    int num_pacotes;
    fscanf(entrada, "%d", &num_pacotes);
    Pacote* estoque = malloc(num_pacotes * sizeof(Pacote));
    for(int i=0; i<num_pacotes; i++){
        fscanf(entrada, "%s %lf %d %d", 
               estoque[i].codigo, &estoque[i].valor, &estoque[i].peso, &estoque[i].vol);
        estoque[i].foi_usado = 0;
    }

    for(int i=0; i<num_veiculos; i++){
        resolver_mochila_2d(frota[i], estoque, num_pacotes, saida);
    }

    int* pendentes = malloc(num_pacotes * sizeof(int));
    int qtd_pend = 0;
    double val_pend = 0;
    int peso_pend = 0, vol_pend = 0;

    for(int i=0; i<num_pacotes; i++){
        if(!estoque[i].foi_usado){
            pendentes[qtd_pend++] = i;
            val_pend += estoque[i].valor;
            peso_pend += estoque[i].peso;
            vol_pend += estoque[i].vol;
        }
    }

    if (qtd_pend > 1) quicksort_indices(estoque, pendentes, 0, qtd_pend - 1);

    if (qtd_pend > 0) {
        fprintf(saida, "PENDENTE:R$%.2f,%dKG,%dL->", val_pend, peso_pend, vol_pend);
        for(int i=0; i<qtd_pend; i++){
            fprintf(saida, "%s", estoque[pendentes[i]].codigo);
            if(i < qtd_pend - 1) fprintf(saida, ",");
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
