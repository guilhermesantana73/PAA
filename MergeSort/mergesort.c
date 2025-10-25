#include <stdio.h>
#include <stdlib.h>

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

int main() {
  int lista[] = {23, 32, 54, 92, 74, 23, 1, 43, 63, 12};
  int n = sizeof(lista) / sizeof(int);

  printf("Vetor original:\n");
  for (int i = 0; i < n; i++) {
    printf("%d ", lista[i]);
  }
  printf("\n");

  int* aux = malloc(n * sizeof(int));

  mergeSort(lista, aux, 0, n - 1);

  printf("Vetor ordenado:\n");
  for (int i = 0; i < n; i++) {
    printf("%d ", lista[i]);
  }
  printf("\n");

  free(aux);

  return 0;
}