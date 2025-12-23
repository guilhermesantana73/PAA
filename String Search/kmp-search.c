#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  

// Cálculo da tabela de transição
void calcular_tabela(int* k, char* P){
  for(int i = 1, j = -1; i < strlen(P); i++){
    while(j >= 0 && P[j + 1] != P[i]){
      j = k[j];
    }

    if(P[j+1] == P[i]){
      j++;
    }

    k[i] = j;
  }
}

// Busca por KMP
void KMP(int* k, int* R, char* T, char* P) {
  //Pré-processamento
  int n = strlen(T), m = strlen(P);
    calcular_tabela(k, P);
  
  for(int i = 0, j = -1; i < n; i++) {
    while(j >= 0 && P[j + 1] != T[i]) {
      j = k[j];
    }

    if(P[j + 1] == T[i]) {
      j++;
    }

    if(j == m - 1) {
      inserir(R, i - m + 1);
      j = k[j];
    }
  }
}