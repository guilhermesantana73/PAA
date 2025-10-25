#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CODIGO 12
#define CNPJ 20

typedef struct {
    char codigo[CODIGO];
    char cnpj[CNPJ];
    int peso;
} Registro;

/* INÍCIO DO MERGESORT DEFAULT */
void merge(int vetor[], int aux[], int index_inic, int index_mid, int index_fim) {
  int cont_prim, cont_sec, cont_terc; 
  int n = index_mid - index_inic + 1;
  int m = index_fim - index_mid;

  int* esquerda = aux;
  int* direita = aux + n;

  for(cont_prim = 0; cont_prim < n; cont_prim++) {
    esquerda[cont_prim] = vetor[index_inic + cont_prim];
  }
  for(cont_sec = 0; cont_sec < m; cont_sec++) {
    direita[cont_sec] = vetor[index_mid + 1 + cont_sec];
  }

  cont_prim = 0, cont_sec = 0, cont_terc = index_inic;

  while(cont_prim < n && cont_sec < m) {
    if(esquerda[cont_prim] <= direita[cont_sec]){
      vetor[cont_terc++] = esquerda[cont_prim++];
    } else {
      vetor[cont_terc++] = direita[cont_sec++];
    }
  }

  while(cont_prim < n) {
    vetor[cont_terc++] = esquerda[cont_prim++];
  }
  while(cont_sec < m) {
    vetor[cont_terc++] = direita[cont_sec++];
  }
}

void mergeSort(int vetor[], int aux[], int index_inic, int index_fim) {
  if (index_inic < index_fim) {
    int index_mid = index_inic + (index_fim - index_inic)/2;

    mergeSort(vetor, aux, index_inic, index_mid);
    mergeSort(vetor, aux, index_mid + 1, index_fim);
    
    merge(vetor, aux, index_inic, index_mid, index_fim);
  }
}
/* FIM DO MERGESORT DEFAULT */

/* LEITURA dos Dados */
void lerDadosEntrada(FILE* arquivo, Registro** lista1, int* tam1, Registro** lista2, int* tam2) {
    if (fscanf(arquivo, "%d", tam1) != 1) {
        fprintf(stderr, "Erro ao ler o tamanho da Lista 1.\n");
        exit(1);
    }

    *lista1 = malloc(*tam1 * sizeof(Registro));
    
    if (*lista1 == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para Lista 1.\n");
        exit(1);
    }

    for (int i = 0; i < *tam1; i++) {
        if (fscanf(arquivo, "%s %s %d", 
                   (*lista1)[i].codigo, 
                   (*lista1)[i].cnpj, 
                   &(*lista1)[i].peso) != 3) 
        {
            fprintf(stderr, "Erro ao ler dados do registro %d da Lista 1.\n", i);
            exit(1);
        }
    }
    
    if (fscanf(arquivo, "%d", tam2) != 1) {
        fprintf(stderr, "Erro ao ler o tamanho da Lista 2.\n");
        exit(1);
    }

    *lista2 = malloc(*tam2 * sizeof(Registro));
    if (*lista2 == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para Lista 2.\n");
        exit(1);
    }

    for (int i = 0; i < *tam2; i++) {
        if (fscanf(arquivo, "%s %s %d", 
                   (*lista2)[i].codigo, 
                   (*lista2)[i].cnpj, 
                   &(*lista2)[i].peso) != 3) 
        {
            fprintf(stderr, "Erro ao ler dados do registro %d da Lista 2.\n", i);
            exit(1);
        }
    }
}

/* MAIN */
int main() {
    FILE* arquivo_entrada;
    
    Registro* minhaLista1 = NULL;
    Registro* minhaLista2 = NULL;

    int tamanho_lista1 = 0;
    int tamanho_lista2 = 0;

    arquivo_entrada = fopen("entrada.txt", "r");
    if (arquivo_entrada == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo 'entrada.txt'\n");
        return 1;
    }

    lerDadosEntrada(arquivo_entrada, 
                    &minhaLista1, &tamanho_lista1, 
                    &minhaLista2, &tamanho_lista2);

    fclose(arquivo_entrada);

    printf("--- Lista 1 (%d itens) ---\n", tamanho_lista1);
    for (int i = 0; i < tamanho_lista1; i++) {
        printf("  %s, %s, %d\n", 
               minhaLista1[i].codigo, 
               minhaLista1[i].cnpj, 
               minhaLista1[i].peso);
    }

    printf("\n--- Lista 2 (%d itens) ---\n", tamanho_lista2);
    for (int i = 0; i < tamanho_lista2; i++) {
        printf("  %s, %s, %d\n", 
               minhaLista2[i].codigo, 
               minhaLista2[i].cnpj, 
               minhaLista2[i].peso);
    }
    
    free(minhaLista1);
    free(minhaLista2);

    return 0;
}