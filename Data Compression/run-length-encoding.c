#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// O contador é limitado a 255.
int comprimir_RLE(const unsigned char* entrada, int tamanho_entrada, unsigned char* saida) {
    int i = 0;
    int j = 0;

    while (i < tamanho_entrada) {
        unsigned char simbolo_atual = entrada[i];
        int contador = 1;

        while ((i + 1 < tamanho_entrada) && (entrada[i + 1] == simbolo_atual) && (contador < 255)) {
            contador++;
            i++;
        }

        saida[j++] = (unsigned char)contador;
        saida[j++] = simbolo_atual;
        
        i++;
    }
    return j;
}

int main_rle() {

    printf("--- RLE: Teste 1 (Repeticao Longa) ---\n");
    unsigned char teste1[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}; // 10 bytes
    int tam1_orig = sizeof(teste1);
    unsigned char saida1[100];

    int tam1_comp = comprimir_RLE(teste1, tam1_orig, saida1);

    printf("Original: %d bytes\n", tam1_orig); // Esperado: 10
    printf("Comprimido: %d bytes\n", tam1_comp); // Esperado: 2 (um par [10][AA])
    printf("Taxa de Compressao: %.2f\n", (double)tam1_comp / tam1_orig); // Esperado: 0.20
    printf("Saida (Contador, Simbolo): [%d] [0x%02X]\n\n", saida1[0], saida1[1]);

    printf("--- RLE: Teste 2 (Sem Repeticao) ---\n");
    unsigned char teste2[] = {0x10, 0x20, 0x30, 0x40};
    int tam2_orig = sizeof(teste2);
    unsigned char saida2[100];

    int tam2_comp = comprimir_RLE(teste2, tam2_orig, saida2);

    printf("Original: %d bytes\n", tam2_orig); // Esperado: 4
    printf("Comprimido: %d bytes\n", tam2_comp); // Esperado: 8 (4 pares [1][Byte])
    printf("Taxa de Compressao: %.2f\n\n", (double)tam2_comp / tam2_orig); // Esperado: 2.00 (EXPANSAO)

    return 0;
}