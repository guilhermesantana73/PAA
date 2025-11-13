#include <stdint.h>
#include <stdio.h>

void trocar(int* a, int* b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void heapify(int* vetor, int tamanho, int index) {
  int pai = index; 
  int no_esq = 2 * index + 1;
  int no_dir = 2 * index + 2;

  if (no_esq < tamanho && vetor[no_esq] > vetor[no_dir]) {
    pai = no_esq;
  }

  if (no_dir < tamanho && vetor[no_dir] > vetor[no_esq]) {
    pai = no_dir;
  }

  if (pai != index) {
    trocar(&vetor[index], &vetor[pai]);
    heapify(vetor, tamanho, pai);
  }
}

void heapsort(int vetor[], int n) {
  for(int j = n/2 - 1; j >= 0; j--) {
    heapify(vetor, n, j);
  }

  for(int i = n - 1; i > 0; i--) {
    trocar(&vetor[0], &vetor[n]);

    heapify(vetor, i, 0);
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

  heapsort(vetor, n);

  printf("Vetor ordenado: ");
  for (int i = 0; i < n; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

  return 0;
}