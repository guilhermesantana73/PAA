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

void aux_hoare_mediana(int* vetor, int index_esq, int index_dir) {
  int index_pri = index_esq + (index_dir - index_esq) / 4;
  int index_sec = (index_esq + index_dir) / 2;
  int index_ter = index_esq + (index_dir - index_esq) * 3/4;

  int index_mediana;

  if (vetor[index_pri] > vetor[index_sec]) {
    if (vetor[index_sec] > vetor[index_ter]) {
      index_mediana = index_sec;
    } else if (vetor[index_pri] > vetor[index_ter]) {
      index_mediana = index_ter;
    } else {
      index_mediana = index_pri;
    }
  } else {
    if (vetor[index_sec] < vetor[index_ter]) {
      index_mediana = index_sec;
    } else if (vetor[index_pri] < vetor[index_ter]) {
      index_mediana = index_ter;
    } else {
      index_mediana = index_pri;
    }
  }

  trocar(&vetor[index_esq], &vetor[index_mediana]);
}

void aux_hoare_random(int* vetor, int index_esq, int index_dir) {
  int index_random = index_esq + rand() % (index_dir - index_esq + 1);

  trocar(&vetor[index_esq], &vetor[index_random]);
}

void quicksort_hoare_padrao(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    int particao = hoare(vetor, index_esq, index_dir);

    quicksort_hoare_padrao(vetor, index_esq, particao);
    quicksort_hoare_padrao(vetor, particao + 1, index_dir);
  }
}

void quicksort_hoare_mediana(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    aux_hoare_mediana(vetor, index_esq, index_dir);
    
    int particao = hoare(vetor, index_esq, index_dir);

    quicksort_hoare_mediana(vetor, index_esq, particao);
    quicksort_hoare_mediana(vetor, particao + 1, index_dir);
  }
}

void quicksort_hoare_random(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    aux_hoare_random(vetor, index_esq, index_dir);
    
    int particao = hoare(vetor, index_esq, index_dir);

    quicksort_hoare_random(vetor, index_esq, particao);
    quicksort_hoare_random(vetor, particao + 1, index_dir);
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

  quicksort_hoare_random(vetor, 0, n - 1);

  printf("Vetor ordenado: ");
  for (int i = 0; i < n; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

  return 0;
}