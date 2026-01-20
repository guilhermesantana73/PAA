#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  

typedef struct {
    char nome[4];
    int score;
} Resultado;

void trocar(int* a, int* b, int* cont_trocas);
int hoare(int* vetor, int index_esq, int index_dir, int* cont_trocas);
void aux_hoare_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas);
void aux_hoare_random(int* vetor, int index_esq, int index_dir, int* cont_trocas);
void quicksort_hoare_padrao(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas);
void quicksort_hoare_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas);
void quicksort_hoare_random(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas);
int lomuto(int* vetor, int index_esq, int index_dir, int* cont_trocas);
void aux_lomuto_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas);
void aux_lomuto_random(int* vetor, int index_esq, int index_dir, int* cont_trocas);
void quicksort_lomuto_padrao(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas);
void quicksort_lomuto_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas);
void quicksort_lomuto_random(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas);
void ordenar_resultados(Resultado* resultados);

int main(int argc, char *argv[]) {
    FILE *entrada;
    FILE *saida;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    entrada = fopen(argv[1], "r");
    if (entrada == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de entrada: %s\n", argv[1]);
        return 1;
    }

    saida = fopen(argv[2], "w");
    if (saida == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo de saida: %s\n", argv[2]);
        fclose(entrada);
        return 1;
    }

    int total_vetores;
    fscanf(entrada, "%d", &total_vetores);

    for (int i = 0; i < total_vetores; i++) {
        int n;
        fscanf(entrada, "%d", &n);

        int* vetor_original = (int*)malloc(n * sizeof(int));
        int* vetor_copia = (int*)malloc(n * sizeof(int));

        if (vetor_original == NULL || vetor_copia == NULL) {
            printf("Erro de alocacao de memoria.\n");
            return 1;
        }

        for (int j = 0; j < n; j++) {
            fscanf(entrada, "%d", &vetor_original[j]);
        }

        Resultado resultados[6];
        int cont_trocas, cont_chamadas;

        memcpy(vetor_copia, vetor_original, n * sizeof(int));
        cont_trocas = 0; cont_chamadas = 0;
        quicksort_lomuto_padrao(vetor_copia, 0, n - 1, &cont_trocas, &cont_chamadas);
        strcpy(resultados[0].nome, "LP");
        resultados[0].score = cont_trocas + cont_chamadas;

        memcpy(vetor_copia, vetor_original, n * sizeof(int));
        cont_trocas = 0; cont_chamadas = 0;
        quicksort_lomuto_mediana(vetor_copia, 0, n - 1, &cont_trocas, &cont_chamadas);
        strcpy(resultados[1].nome, "LM");
        resultados[1].score = cont_trocas + cont_chamadas;

        memcpy(vetor_copia, vetor_original, n * sizeof(int));
        cont_trocas = 0; cont_chamadas = 0;
        quicksort_lomuto_random(vetor_copia, 0, n - 1, &cont_trocas, &cont_chamadas);
        strcpy(resultados[2].nome, "LA");
        resultados[2].score = cont_trocas + cont_chamadas;

        memcpy(vetor_copia, vetor_original, n * sizeof(int));
        cont_trocas = 0; cont_chamadas = 0;
        quicksort_hoare_padrao(vetor_copia, 0, n - 1, &cont_trocas, &cont_chamadas);
        strcpy(resultados[3].nome, "HP");
        resultados[3].score = cont_trocas + cont_chamadas;

        memcpy(vetor_copia, vetor_original, n * sizeof(int));
        cont_trocas = 0; cont_chamadas = 0;
        quicksort_hoare_mediana(vetor_copia, 0, n - 1, &cont_trocas, &cont_chamadas);
        strcpy(resultados[4].nome, "HM");
        resultados[4].score = cont_trocas + cont_chamadas;

        memcpy(vetor_copia, vetor_original, n * sizeof(int));
        cont_trocas = 0; cont_chamadas = 0;
        quicksort_hoare_random(vetor_copia, 0, n - 1, &cont_trocas, &cont_chamadas);
        strcpy(resultados[5].nome, "HA");
        resultados[5].score = cont_trocas + cont_chamadas;

        ordenar_resultados(resultados);

        fprintf(saida, "[%d]:", n);
        for (int j = 0; j < 6; j++) {
            fprintf(saida, "%s(%d)", resultados[j].nome, resultados[j].score);
            if (j < 5) fprintf(saida, ",");
        }
        fprintf(saida, "\n");

        free(vetor_original);
        free(vetor_copia);
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}

void trocar(int* a, int* b, int* cont_trocas) {
    int temp = *a;
    *a = *b;
    *b = temp;
    (*cont_trocas)++;
}

int hoare(int* vetor, int index_esq, int index_dir, int* cont_trocas) {
  int pivo_hoare = vetor[index_esq], cont_esq = index_esq - 1, cont_dir = index_dir + 1;
  
  while (1) {
    while(vetor[--cont_dir] > pivo_hoare);
    while(vetor[++cont_esq] < pivo_hoare);

    if (cont_esq < cont_dir) {
      trocar(&vetor[cont_esq], &vetor[cont_dir], cont_trocas);
    } else {
      return cont_dir;
    }
  }
}

void aux_hoare_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas) {
  int n = index_dir - index_esq + 1;

  int index_pri = index_esq + (n * 1) / 4;
  int index_sec = index_esq + (n * 1) / 2;
  int index_ter = index_esq + (n * 3) / 4;

  int index_mediana;

  if (vetor[index_pri] > vetor[index_sec]) {
    if (vetor[index_sec] > vetor[index_ter]) index_mediana = index_sec;
    else if (vetor[index_pri] > vetor[index_ter]) index_mediana = index_ter;
    else index_mediana = index_pri;
  } else {
    if (vetor[index_sec] < vetor[index_ter]) index_mediana = index_sec;
    else if (vetor[index_pri] < vetor[index_ter]) index_mediana = index_ter;
    else index_mediana = index_pri;
  }

  trocar(&vetor[index_esq], &vetor[index_mediana], cont_trocas);
}

void aux_hoare_random(int* vetor, int index_esq, int index_dir, int* cont_trocas) {
  int tamanho = index_dir - index_esq + 1;
  int idx_pivo = index_esq + (abs(vetor[index_esq]) % tamanho);

  trocar(&vetor[index_esq], &vetor[idx_pivo], cont_trocas);
}

void quicksort_hoare_padrao(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas) {
  (*cont_chamadas)++;
  if (index_esq < index_dir) {
    int particao = hoare(vetor, index_esq, index_dir, cont_trocas);
    quicksort_hoare_padrao(vetor, index_esq, particao, cont_trocas, cont_chamadas);
    quicksort_hoare_padrao(vetor, particao + 1, index_dir, cont_trocas, cont_chamadas);
  }
}

void quicksort_hoare_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas) {
  (*cont_chamadas)++;
  if (index_esq < index_dir) {
    aux_hoare_mediana(vetor, index_esq, index_dir, cont_trocas);
    int particao = hoare(vetor, index_esq, index_dir, cont_trocas);
    quicksort_hoare_mediana(vetor, index_esq, particao, cont_trocas, cont_chamadas);
    quicksort_hoare_mediana(vetor, particao + 1, index_dir, cont_trocas, cont_chamadas);
  }
}

void quicksort_hoare_random(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas) {
  (*cont_chamadas)++;
  if (index_esq < index_dir) {
    aux_hoare_random(vetor, index_esq, index_dir, cont_trocas);
    int particao = hoare(vetor, index_esq, index_dir, cont_trocas);
    quicksort_hoare_random(vetor, index_esq, particao, cont_trocas, cont_chamadas);
    quicksort_hoare_random(vetor, particao + 1, index_dir, cont_trocas, cont_chamadas);
  }
}

int lomuto(int* vetor, int index_esq, int index_dir, int* cont_trocas) {
  int pivo_lomuto = vetor[index_dir];
  int cont_esq = index_esq - 1; 

  for(int cont_dir = index_esq; cont_dir < index_dir; cont_dir++) {
    if (vetor[cont_dir] <= pivo_lomuto) {
      cont_esq++;
      trocar(&vetor[cont_esq], &vetor[cont_dir], cont_trocas);
    }
  }
  trocar(&vetor[cont_esq + 1], &vetor[index_dir], cont_trocas);
  return cont_esq + 1;
}

void aux_lomuto_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas) {
  int n = index_dir - index_esq + 1;

  int index_pri = index_esq + (n * 1) / 4;
  int index_sec = index_esq + (n * 1) / 2;
  int index_ter = index_esq + (n * 3) / 4;

  int index_mediana;

  if (vetor[index_pri] > vetor[index_sec]) {
    if (vetor[index_sec] > vetor[index_ter]) index_mediana = index_sec;
    else if (vetor[index_pri] > vetor[index_ter]) index_mediana = index_ter;
    else index_mediana = index_pri;
  } else {
    if (vetor[index_sec] < vetor[index_ter]) index_mediana = index_sec;
    else if (vetor[index_pri] < vetor[index_ter]) index_mediana = index_ter;
    else index_mediana = index_pri;
  }

  trocar(&vetor[index_dir], &vetor[index_mediana], cont_trocas);
}

void aux_lomuto_random(int* vetor, int index_esq, int index_dir, int* cont_trocas) {
  int tamanho = index_dir - index_esq + 1;
  int idx_pivo = index_esq + (abs(vetor[index_esq]) % tamanho);

  trocar(&vetor[index_dir], &vetor[idx_pivo], cont_trocas);
}

void quicksort_lomuto_padrao(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas) {
  (*cont_chamadas)++;
  if (index_esq < index_dir) {
    int particao = lomuto(vetor, index_esq, index_dir, cont_trocas);
    quicksort_lomuto_padrao(vetor, index_esq, particao - 1, cont_trocas, cont_chamadas);
    quicksort_lomuto_padrao(vetor, particao + 1, index_dir, cont_trocas, cont_chamadas);
  }
}

void quicksort_lomuto_mediana(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas) {
  (*cont_chamadas)++;
  if (index_esq < index_dir) {
    aux_lomuto_mediana(vetor, index_esq, index_dir, cont_trocas);
    int particao = lomuto(vetor, index_esq, index_dir, cont_trocas);
    quicksort_lomuto_mediana(vetor, index_esq, particao - 1, cont_trocas, cont_chamadas);
    quicksort_lomuto_mediana(vetor, particao + 1, index_dir, cont_trocas, cont_chamadas);
  }
}

void quicksort_lomuto_random(int* vetor, int index_esq, int index_dir, int* cont_trocas, int* cont_chamadas) {
  (*cont_chamadas)++;
  if (index_esq < index_dir) {
    aux_lomuto_random(vetor, index_esq, index_dir, cont_trocas);
    int particao = lomuto(vetor, index_esq, index_dir, cont_trocas);
    quicksort_lomuto_random(vetor, index_esq, particao - 1, cont_trocas, cont_chamadas);
    quicksort_lomuto_random(vetor, particao + 1, index_dir, cont_trocas, cont_chamadas);
  }
}

void ordenar_resultados(Resultado* resultados) {
    for (int i = 1; i < 6; i++) {
        Resultado chave = resultados[i];
        int j = i - 1;
        
        while (j >= 0 && resultados[j].score > chave.score) {
            resultados[j + 1] = resultados[j];
            j = j - 1;
        }
        resultados[j + 1] = chave;
    }
}