#include <stdio.h>
#include <stdlib.h> 

void trocar(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int hoare(int* vetor, int index_esq, int index_dir) {
  int pivo_hoare = vetor[index_esq], cont_esq = index_esq - 1, cont_dir = index_dir + 1;
  
  while (1) {
    while(vetor[--cont_dir] > pivo_hoare);
    while(vetor[++cont_esq] < pivo_hoare);

    if (cont_esq < cont_dir) {
      trocar(&vetor[cont_esq], &vetor[cont_dir]);
    } else {
      return cont_dir;
    }
  }
}

void quicksort_hoare(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    int pivo = hoare(vetor, index_esq, index_dir);

    quicksort_hoare(vetor, index_esq, pivo);
    quicksort_hoare(vetor, pivo + 1, index_dir);
  }
}

int main() {
  int vetor[10] = {23, 32, 54, 92, 74, 23, 1, 43, 63, 12};

  int n = 10;

  printf("Vetor original: ");
  for (int i = 0; i < n; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

  quicksort_hoare(vetor, 0, n - 1);

  printf("Vetor ordenado: ");
  for (int i = 0; i < n; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

  return 0;
}