#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CODIGO 12
#define MAX_CNPJ 20

typedef struct {
    char codigo[MAX_CODIGO];
    char cnpj[MAX_CNPJ];
    int peso;
} Registro;

void lerDadosEntrada(FILE* arquivo, Registro** lista1, int* tam1, Registro** lista2, int* tam2);
void ordenarLista(Registro lista[], int tamanho);
void mergeSort(Registro vetor[], Registro aux[], int index_inic, int index_fim);
void merge(Registro vetor[], Registro aux[], int index_inic, int index_mid, int index_fim);
void processarListas(FILE* saida_cnpj, FILE* saida_peso, 
                     Registro lista1[], int tam1, 
                     Registro lista2[], int tam2);
void copiarArquivo(FILE* destino, FILE* origem);

int main() {
    FILE* arquivo_entrada;
    FILE* arquivo_saida;
    FILE* saida_cnpj_tmp;
    FILE* saida_peso_tmp;

    Registro* minhaLista1 = NULL;
    Registro* minhaLista2 = NULL;
    int tamanho_lista1 = 0;
    int tamanho_lista2 = 0;

    arquivo_entrada = fopen("entrada.txt", "r");
    if (arquivo_entrada == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo 'entrada.txt'\n");
        return 1;
    }
    lerDadosEntrada(arquivo_entrada, &minhaLista1, &tamanho_lista1, &minhaLista2, &tamanho_lista2);
    fclose(arquivo_entrada);
    printf("Dados lidos com sucesso.\n");

    ordenarLista(minhaLista1, tamanho_lista1);
    ordenarLista(minhaLista2, tamanho_lista2);
    printf("Ambas as listas ordenadas por codigo.\n");

    saida_cnpj_tmp = fopen("saida_cnpj.tmp", "w");
    saida_peso_tmp = fopen("saida_peso.tmp", "w");
    if (saida_cnpj_tmp == NULL || saida_peso_tmp == NULL) {
        fprintf(stderr, "Erro ao criar arquivos temporarios.\n");
        exit(1);
    }
    
    processarListas(saida_cnpj_tmp, saida_peso_tmp, minhaLista1, tamanho_lista1, minhaLista2, tamanho_lista2);
    
    fclose(saida_cnpj_tmp);
    fclose(saida_peso_tmp);
    printf("Processamento concluido.\n");

    arquivo_saida = fopen("saida.txt", "w");
    if (arquivo_saida == NULL) {
        fprintf(stderr, "Erro ao criar 'saida.txt'\n"); exit(1);
    }

    saida_cnpj_tmp = fopen("saida_cnpj.tmp", "r");
    copiarArquivo(arquivo_saida, saida_cnpj_tmp);
    fclose(saida_cnpj_tmp);

    saida_peso_tmp = fopen("saida_peso.tmp", "r");
    copiarArquivo(arquivo_saida, saida_peso_tmp);
    fclose(saida_peso_tmp);
    
    fclose(arquivo_saida);

    remove("saida_cnpj.tmp");
    remove("saida_peso.tmp");
    
    printf("Arquivo 'saida.txt' montado com sucesso.\n");

    free(minhaLista1);
    free(minhaLista2);

    return 0;
}

void lerDadosEntrada(FILE* arquivo, Registro** lista1, int* tam1, Registro** lista2, int* tam2) {
    if (fscanf(arquivo, "%d", tam1) != 1) { fprintf(stderr, "Erro ao ler tam1\n"); exit(1); }
    *lista1 = malloc(*tam1 * sizeof(Registro));
    if (*lista1 == NULL) { fprintf(stderr, "Erro malloc lista1\n"); exit(1); }
    for (int i = 0; i < *tam1; i++) {
        if (fscanf(arquivo, "%s %s %d", (*lista1)[i].codigo, (*lista1)[i].cnpj, &(*lista1)[i].peso) != 3) { fprintf(stderr, "Erro ao ler dados lista1\n"); exit(1); }
    }
    
    if (fscanf(arquivo, "%d", tam2) != 1) { fprintf(stderr, "Erro ao ler tam2\n"); exit(1); }
    *lista2 = malloc(*tam2 * sizeof(Registro));
    if (*lista2 == NULL) { fprintf(stderr, "Erro malloc lista2\n"); exit(1); }
    for (int i = 0; i < *tam2; i++) {
        if (fscanf(arquivo, "%s %s %d", (*lista2)[i].codigo, (*lista2)[i].cnpj, &(*lista2)[i].peso) != 3) { fprintf(stderr, "Erro ao ler dados lista2\n"); exit(1); }
    }
}

void ordenarLista(Registro lista[], int tamanho) {
    if (tamanho <= 1) return;
    Registro* aux = malloc(tamanho * sizeof(Registro));
    if (aux == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria no mergeSort.\n");
        exit(1);
    }
    mergeSort(lista, aux, 0, tamanho - 1);
    free(aux);
}

void mergeSort(Registro vetor[], Registro aux[], int index_inic, int index_fim) {
    if (index_inic < index_fim) {
        int index_mid = index_inic + (index_fim - index_inic) / 2;
        mergeSort(vetor, aux, index_inic, index_mid);
        mergeSort(vetor, aux, index_mid + 1, index_fim);
        merge(vetor, aux, index_inic, index_mid, index_fim);
    }
}

void merge(Registro vetor[], Registro aux[], int index_inic, int index_mid, int index_fim) {
    int cont_prim, cont_sec, cont_terc;
    int n = index_mid - index_inic + 1;
    int m = index_fim - index_mid;
    Registro* esquerda = aux;
    Registro* direita = aux + n;

    for (cont_prim = 0; cont_prim < n; cont_prim++) {
        esquerda[cont_prim] = vetor[index_inic + cont_prim];
    }
    for (cont_sec = 0; cont_sec < m; cont_sec++) {
        direita[cont_sec] = vetor[index_mid + 1 + cont_sec];
    }

    cont_prim = 0; cont_sec = 0; cont_terc = index_inic;

    while (cont_prim < n && cont_sec < m) {
        if (strcmp(esquerda[cont_prim].codigo, direita[cont_sec].codigo) <= 0) {
            vetor[cont_terc++] = esquerda[cont_prim++];
        } else {
            vetor[cont_terc++] = direita[cont_sec++];
        }
    }
    while (cont_prim < n) {
        vetor[cont_terc++] = esquerda[cont_prim++];
    }
    while (cont_sec < m) {
        vetor[cont_terc++] = direita[cont_sec++];
    }
}

void copiarArquivo(FILE* destino, FILE* origem) {
    char buffer[4096];
    size_t bytes_lidos;

    rewind(origem); 

    while ((bytes_lidos = fread(buffer, 1, sizeof(buffer), origem)) > 0) {
        fwrite(buffer, 1, bytes_lidos, destino);
    }
}

void processarListas(FILE* saida_cnpj, FILE* saida_peso, 
                     Registro lista1[], int tam1, 
                     Registro lista2[], int tam2) {
    int i = 0;
    int j = 0;

    while (i < tam1 && j < tam2) {
        int cmp = strcmp(lista1[i].codigo, lista2[j].codigo);

        if (cmp == 0) {
            Registro* item_mestre = &lista1[i];
            Registro* item_inspecionado = &lista2[j];

            if (strcmp(item_mestre->cnpj, item_inspecionado->cnpj) != 0) {
                fprintf(saida_cnpj, "%s:%s<->%s\n", 
                        item_mestre->codigo, 
                        item_mestre->cnpj, 
                        item_inspecionado->cnpj);
            }
            else {
                int peso_original = item_mestre->peso;
                int diff_peso_abs = abs(item_inspecionado->peso - peso_original);
                
                if (peso_original == 0) {
                    if (diff_peso_abs > 0) {
                         fprintf(saida_peso, "%s:%dkg(100%%+)\n", 
                                 item_mestre->codigo, 
                                 diff_peso_abs);
                    }
                } else {
                    double percent_diff = ((double)diff_peso_abs / peso_original) * 100.0;
                    if (percent_diff > 10.0) {
                        fprintf(saida_peso, "%s:%dkg(%d%%)\n", 
                                item_mestre->codigo, 
                                diff_peso_abs, 
                                (int)round(percent_diff));
                    }
                }
            }
            i++;
            j++;
        } else if (cmp < 0) {
            i++;
        } else {
            j++;
        }
    }
}