#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Estruturas ---
typedef struct {
    int prioridade;
    int tamanho;
    int id_chegada; // Para garantir estabilidade se necessário (desempate)
    char** dados;   // Array de strings para guardar os bytes "0A", "0B", etc.
} Pacote;

// --- Protótipos ---
void trocar(Pacote* a, Pacote* b);
void max_heapify(Pacote* vetor, int tamanho_heap, int i);
void build_max_heap(Pacote* vetor, int n);
void heapsort(Pacote* vetor, int n);
void liberar_pacote(Pacote* p);
void imprimir_buffer(FILE* saida, Pacote* buffer, int n);

// --- Função Main ---
int main(int argc, char *argv[]) {
    FILE *entrada;
    FILE *saida;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <entrada.txt> <saida.txt>\n", argv[0]);
        return 1;
    }

    entrada = fopen(argv[1], "r");
    if (entrada == NULL) {
        fprintf(stderr, "Erro ao abrir entrada: %s\n", argv[1]);
        return 1;
    }

    saida = fopen(argv[2], "w");
    if (saida == NULL) {
        fprintf(stderr, "Erro ao abrir saida: %s\n", argv[2]);
        fclose(entrada);
        return 1;
    }

    int total_pacotes, capacidade_maxima;
    if (fscanf(entrada, "%d %d", &total_pacotes, &capacidade_maxima) != 2) {
        return 1;
    }

    // Buffer estático para armazenar os pacotes (tamanho máximo seguro)
    // Assumindo que o buffer nunca terá mais pacotes que o total de entrada
    Pacote* buffer = (Pacote*)malloc(total_pacotes * sizeof(Pacote));
    int qtd_no_buffer = 0;
    int carga_atual = 0;
    int contador_chegada = 0;

    for (int i = 0; i < total_pacotes; i++) {
        Pacote novo_pacote;
        novo_pacote.id_chegada = contador_chegada++;
        
        fscanf(entrada, "%d %d", &novo_pacote.prioridade, &novo_pacote.tamanho);

        // Aloca memória para os bytes do pacote
        novo_pacote.dados = (char**)malloc(novo_pacote.tamanho * sizeof(char*));
        for (int j = 0; j < novo_pacote.tamanho; j++) {
            novo_pacote.dados[j] = (char*)malloc(4 * sizeof(char)); // "FF" + \0
            fscanf(entrada, "%s", novo_pacote.dados[j]);
        }

        // Verifica se cabe no buffer
        if (carga_atual + novo_pacote.tamanho > capacidade_maxima) {
            // Buffer cheio: Ordena e Imprime
            heapsort(buffer, qtd_no_buffer);
            imprimir_buffer(saida, buffer, qtd_no_buffer);

            // Limpa o buffer (libera memória interna dos pacotes antigos)
            for (int k = 0; k < qtd_no_buffer; k++) {
                liberar_pacote(&buffer[k]);
            }
            qtd_no_buffer = 0;
            carga_atual = 0;
        }

        // Adiciona o novo pacote ao buffer
        buffer[qtd_no_buffer] = novo_pacote;
        qtd_no_buffer++;
        carga_atual += novo_pacote.tamanho;
    }

    // Processa o restante do buffer se houver algo
    if (qtd_no_buffer > 0) {
        heapsort(buffer, qtd_no_buffer);
        imprimir_buffer(saida, buffer, qtd_no_buffer);
        for (int k = 0; k < qtd_no_buffer; k++) {
            liberar_pacote(&buffer[k]);
        }
    }

    free(buffer);
    fclose(entrada);
    fclose(saida);

    return 0;
}

// --- Implementações ---

void trocar(Pacote* a, Pacote* b) {
    Pacote temp = *a;
    *a = *b;
    *b = temp;
}

// Adaptação do algoritmo da aula para a struct Pacote
// Compara baseando-se na PRIORIDADE
void max_heapify(Pacote* vetor, int tamanho_heap, int i) {
    int maior = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    // Compara prioridade. Se prioridade for igual, usa ordem de chegada (estabilidade)
    // Queremos construir um Max-Heap (maior prioridade no topo)
    
    if (esq < tamanho_heap) {
        if (vetor[esq].prioridade > vetor[maior].prioridade) {
            maior = esq;
        }
    }

    if (dir < tamanho_heap) {
        if (vetor[dir].prioridade > vetor[maior].prioridade) {
            maior = dir;
        }
    }

    if (maior != i) {
        trocar(&vetor[i], &vetor[maior]);
        max_heapify(vetor, tamanho_heap, maior);
    }
}

void build_max_heap(Pacote* vetor, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        max_heapify(vetor, n, i);
    }
}

void heapsort(Pacote* vetor, int n) {
    build_max_heap(vetor, n);

    for (int i = n - 1; i > 0; i--) {
        trocar(&vetor[0], &vetor[i]);
        max_heapify(vetor, i, 0);
    }
}

void liberar_pacote(Pacote* p) {
    for (int i = 0; i < p->tamanho; i++) {
        free(p->dados[i]);
    }
    free(p->dados);
}

void imprimir_buffer(FILE* saida, Pacote* buffer, int n) {
    // 1. Imprime o pipe inicial da linha inteira
    fprintf(saida, "|");
    
    // 2. Loop pelos pacotes (do maior para o menor prioridade)
    for (int i = n - 1; i >= 0; i--) {
        // Imprime o conteúdo do pacote
        for (int j = 0; j < buffer[i].tamanho; j++) {
            fprintf(saida, "%s", buffer[i].dados[j]);
            if (j < buffer[i].tamanho - 1) {
                fprintf(saida, ",");
            }
        }
        // 3. Imprime APENAS o pipe de fechamento deste pacote
        fprintf(saida, "|");
    }
    // Pula linha no final
    fprintf(saida, "\n"); 
}