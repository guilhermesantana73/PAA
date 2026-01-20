// Problema da Mochila com 2 Parâmetros (Peso e Valor) - Modelo binário (0/1)

#include <stdio.h>

int max(int a, int b) {
    return (a > b) ? a : b;
}

int knapsack(int cap_max, int pesos[], int valores[], int total) {
    int K[total + 1][cap_max + 1];

    for (int i = 0; i <= total; i++) {
        for (int w = 0; w <= cap_max; w++) {
            
            if (i == 0 || w == 0)
                K[i][w] = 0;
            
            else if (pesos[i - 1] <= w) {
                K[i][w] = max(valores[i - 1] + K[i - 1][w - pesos[i - 1]], 
                              K[i - 1][w]);
            } 
            else {
                K[i][w] = K[i - 1][w];
            }
        }
    }

    return K[total][cap_max];
}

int main() {
    int valores[] = {12, 10, 20, 15};
    int pesos[] = {2, 1, 3, 2};
    int W = 5;
    
    int n = sizeof(valores) / sizeof(valores[0]);

    printf("Valor maximo possivel: %d\n", knapsack(W, pesos, valores, n));

    return 0;
}
