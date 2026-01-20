#include <stdio.h>
#include <stdlib.h> 

void trocar(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int lomuto(int* vetor, int index_esq, int index_dir) {
  int pivo_lomuto = vetor[index_dir], cont_esq = index_esq - 1, cont_dir = index_dir + 1;

  for(cont_dir = index_esq; cont_dir < index_dir; cont_dir++) {
    if (vetor[cont_dir] <= pivo_lomuto) {
      trocar(&vetor[++cont_esq], &vetor[cont_dir]);
    }
  }

  trocar(&vetor[++cont_esq], &vetor[index_dir]);

  return cont_esq;
}

void aux_lomuto_mediana(int* vetor, int index_esq, int index_dir) {
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

  trocar(&vetor[index_dir], &vetor[index_mediana]);
}

void aux_lomuto_random(int* vetor, int index_esq, int index_dir) {
  int index_random = index_esq + rand() % (index_dir - index_esq + 1);

  trocar(&vetor[index_dir], &vetor[index_random]);
}

void quicksort_lomuto_padrao(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    int particao = lomuto(vetor, index_esq, index_dir);

    quicksort_lomuto_padrao(vetor, index_esq, particao - 1);
    quicksort_lomuto_padrao(vetor, particao + 1, index_dir);
  }
}

void quicksort_lomuto_mediana(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    aux_lomuto_mediana(vetor, index_esq, index_dir);
    
    int particao = lomuto(vetor, index_esq, index_dir);

    quicksort_lomuto_mediana(vetor, index_esq, particao - 1);
    quicksort_lomuto_mediana(vetor, particao + 1, index_dir);
  }
}

void quicksort_lomuto_random(int* vetor, int index_esq, int index_dir) {
  if (index_esq < index_dir) {
    aux_lomuto_random(vetor, index_esq, index_dir);
    
    int particao = lomuto(vetor, index_esq, index_dir);

    quicksort_lomuto_random(vetor, index_esq, particao - 1);
    quicksort_lomuto_random(vetor, particao + 1, index_dir);
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

  quicksort_lomuto_random(vetor, 0, n - 1);

  printf("Vetor ordenado: ");
  for (int i = 0; i < n; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

  return 0;
}