#include <stdio.h>
#include <stdlib.h> 

void trocar(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* LOMUTO PADRÃO */
int lomuto_padrao(int* vetor, int index_esq, int index_dir) {
  int pivo_lomuto = vetor[index_dir], cont_esq = index_esq - 1, cont_dir = index_dir + 1;

  for(cont_dir = index_esq; cont_dir < index_dir; cont_dir++) {
    if (vetor[cont_dir] <= pivo_lomuto) {
      trocar(&vetor[++cont_esq], &vetor[cont_dir]);
    }
  }

  trocar(&vetor[++cont_esq], &vetor[index_dir]);

  return cont_esq;
}

void quicksort_lomuto_padrao(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    int particao = lomuto_padrao(vetor, index_esq, index_dir);

    quicksort_lomuto_padrao(vetor, index_esq, particao - 1);
    quicksort_lomuto_padrao(vetor, particao + 1, index_dir);
  }
}
/* LOMUTO PADRÃO */

int main() {
  int vetor[10] = {23, 32, 54, 92, 74, 23, 1, 43, 63, 12};

  int n = 10;

  printf("Vetor original: ");
  for (int i = 0; i < n; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

  quicksort_lomuto_padrao(vetor, 0, n - 1);

  printf("Vetor ordenado: ");
  for (int i = 0; i < n; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

  return 0;
}