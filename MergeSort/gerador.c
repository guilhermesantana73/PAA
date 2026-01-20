#include <stdio.h>    // Para printf, fopen, fclose, fprintf, stderr
#include <stdlib.h>   // Para rand, srand, exit, NULL, EXIT_FAILURE
#include <time.h>     // Para time()

// Definimos constantes para "números mágicos", 
// o que torna o código mais legível.
#define NOME_ARQUIVO "entrada.txt"
#define TOTAL_NUMEROS 2000000

/*
 * Função para gerar um número aleatório de 64 bits (long long int).
 * A função rand() do C padrão geralmente retorna um int de 32 bits.
 * Para criar um número maior, "juntamos" duas chamadas de rand().
 * (long long)rand() << 32: Pega o primeiro número, converte para long long
 * e o "desloca" 32 bits para a esquerda.
 * | rand():              Faz um OU bit-a-bit com o segundo número,
 * preenchendo os 32 bits da direita.
 */
long long int gerarNumero64bit() {
    // Nota: Esta é uma forma simples. Para aleatoriedade de alta qualidade, 
    // seriam necessárias bibliotecas mais complexas, mas para este
    // projeto, isso é mais do que suficiente.
    return ((long long)rand() << 32) | rand();
}

int main() {
    FILE* arquivo_saida; // 1. Declara o ponteiro de Arquivo

    // 2. "Semeia" o gerador de números aleatórios.
    //    Usamos time(NULL) para garantir que, a cada execução,
    //    a sequência de números aleatórios seja diferente.
    srand(time(NULL));

    printf("Iniciando a geracao do arquivo %s...\n", NOME_ARQUIVO);

    // 3. Abre o arquivo 'entrada.txt' no modo "w" (escrita).
    //    Se o arquivo já existir, será sobrescrito.
    arquivo_saida = fopen(NOME_ARQUIVO, "w");

    // 4. (Robustez) Sempre verifique se o arquivo abriu com sucesso.
    //    fopen retorna NULL se falhar (ex: disco cheio, sem permissão).
    if (arquivo_saida == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo %s para escrita.\n", NOME_ARQUIVO);
        exit(EXIT_FAILURE); // Encerra o programa indicando erro
    }

    // 5. Loop principal para gerar e escrever os números.
    for (long i = 0; i < TOTAL_NUMEROS; i++) {
        long long int numero_aleatorio = gerarNumero64bit();

        // 6. Escreve o número no arquivo.
        //    fprintf funciona como o printf, mas para arquivos.
        //    "%lld" é o formato para long long int.
        //    "\n" garante que cada número fique em sua própria linha.
        fprintf(arquivo_saida, "%lld\n", numero_aleatorio);
    }

    // 7. (Crucial) Fecha o arquivo.
    //    Isso "salva" quaisquer dados restantes na memória para o disco
    //    e libera os recursos do sistema operacional.
    fclose(arquivo_saida);

    printf("Arquivo %s gerado com sucesso com %d numeros.\n", NOME_ARQUIVO, TOTAL_NUMEROS);

    return 0; // Sucesso
}