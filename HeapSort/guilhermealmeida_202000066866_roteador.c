#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int prioridade;
    int tamanho;
    int id_chegada; 
    char** dados;   
} Pacote;

void trocar(Pacote* a, Pacote* b);
void min_heapify(Pacote* vetor, int tamanho_heap, int i);
void build_min_heap(Pacote* vetor, int n);
void heapsort_decrescente(Pacote* vetor, int n);
void liberar_pacote(Pacote* p);
void imprimir_buffer(FILE* saida, Pacote* buffer, int n);

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

    Pacote* buffer = (Pacote*)malloc(total_pacotes * sizeof(Pacote));
    int qtd_no_buffer = 0;
    int carga_atual = 0;
    int contador_chegada = 0;

    for (int i = 0; i < total_pacotes; i++) {
        Pacote novo_pacote;
        novo_pacote.id_chegada = contador_chegada++;
        
        fscanf(entrada, "%d %d", &novo_pacote.prioridade, &novo_pacote.tamanho);

        novo_pacote.dados = (char**)malloc(novo_pacote.tamanho * sizeof(char*));
        for (int j = 0; j < novo_pacote.tamanho; j++) {
            novo_pacote.dados[j] = (char*)malloc(4 * sizeof(char)); 
            fscanf(entrada, "%s", novo_pacote.dados[j]);
        }

        if (carga_atual + novo_pacote.tamanho > capacidade_maxima) {
            heapsort_decrescente(buffer, qtd_no_buffer);
            imprimir_buffer(saida, buffer, qtd_no_buffer);

            for (int k = 0; k < qtd_no_buffer; k++) {
                liberar_pacote(&buffer[k]);
            }
            qtd_no_buffer = 0;
            carga_atual = 0;
        }

        buffer[qtd_no_buffer] = novo_pacote;
        qtd_no_buffer++;
        carga_atual += novo_pacote.tamanho;
    }

    if (qtd_no_buffer > 0) {
        heapsort_decrescente(buffer, qtd_no_buffer);
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

void trocar(Pacote* a, Pacote* b) {
    Pacote temp = *a;
    *a = *b;
    *b = temp;
}

void min_heapify(Pacote* vetor, int tamanho_heap, int i) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < tamanho_heap) {
        if (vetor[esq].prioridade < vetor[menor].prioridade) {
            menor = esq;
        } 
        else if (vetor[esq].prioridade == vetor[menor].prioridade) {
             if (vetor[esq].id_chegada > vetor[menor].id_chegada) {
                 menor = esq;
             }
        }
    }

    if (dir < tamanho_heap) {
        if (vetor[dir].prioridade < vetor[menor].prioridade) {
            menor = dir;
        }
        else if (vetor[dir].prioridade == vetor[menor].prioridade) {
             if (vetor[dir].id_chegada > vetor[menor].id_chegada) {
                 menor = dir;
             }
        }
    }

    if (menor != i) {
        trocar(&vetor[i], &vetor[menor]);
        min_heapify(vetor, tamanho_heap, menor);
    }
}

void build_min_heap(Pacote* vetor, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        min_heapify(vetor, n, i);
    }
}

void heapsort_decrescente(Pacote* vetor, int n) {
    build_min_heap(vetor, n);

    for (int i = n - 1; i > 0; i--) {
        trocar(&vetor[0], &vetor[i]);
        min_heapify(vetor, i, 0); 
    }
}

void liberar_pacote(Pacote* p) {
    for (int i = 0; i < p->tamanho; i++) {
        free(p->dados[i]);
    }
    free(p->dados);
}

void imprimir_buffer(FILE* saida, Pacote* buffer, int n) {
    fprintf(saida, "|");
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < buffer[i].tamanho; j++) {
            fprintf(saida, "%s", buffer[i].dados[j]);
            if (j < buffer[i].tamanho - 1) {
                fprintf(saida, ",");
            }
        }
        fprintf(saida, "|");
    }
    fprintf(saida, "\n"); 
}