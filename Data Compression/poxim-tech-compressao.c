#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256
#define MAX_BUFFER_SIZE 20000

typedef struct {
    unsigned char* bytes;
    int tamanho;
    int capacidade;
} ByteBuffer;

typedef struct {
    ByteBuffer* dados_comprimidos;
    float taxa_compressao;
    char algoritmo[10];
} ResultadoCompressao;

typedef struct MinHeapNode {
    unsigned char dado;
    unsigned freq;
    struct MinHeapNode *esq, *dir;
} MinHeapNode;

typedef struct MinHeap {
    unsigned tamanho;
    unsigned capacidade;
    MinHeapNode** array;
} MinHeap;

/* Protótipos */
ByteBuffer* criar_buffer(int capacidade);
void adicionar_byte(ByteBuffer* buffer, unsigned char b);
void liberar_buffer(ByteBuffer* buffer);

ResultadoCompressao* criar_resultado(char* algoritmo);
void liberar_resultado(ResultadoCompressao* res);
void imprimir_resultado(FILE* saida, int id_source, ResultadoCompressao* res);

ResultadoCompressao* executar_rle(ByteBuffer* entrada);
ResultadoCompressao* executar_huffman(ByteBuffer* entrada);

ResultadoCompressao* compressao_huffman(unsigned char *dados, int qtd_bytes);
ResultadoCompressao* compressao_rle(unsigned char *dados, int qtd_bytes);

MinHeapNode* novo_no(unsigned char dado, unsigned freq);
MinHeap* criar_min_heap(unsigned capacidade);
void trocar_no(MinHeapNode** a, MinHeapNode** b);
void min_heapify(MinHeap* min_heap, int idx);
MinHeapNode* extrair_min(MinHeap* min_heap);
void inserir_min_heap(MinHeap* min_heap, MinHeapNode* node);
int e_tamanho_um(MinHeap* min_heap);
MinHeapNode* construir_arvore_huffman(unsigned char dados[], int freq[], int tamanho);
void gerar_codigos(MinHeapNode* raiz, int arr[], int topo, char* codigos[256]);
void liberar_arvore_huffman(MinHeapNode* raiz);

/* ================= MAIN ================= */
int main(int argc, char *argv[]) {

    FILE *entrada = fopen(argv[1], "r");
    FILE *saida   = fopen(argv[2], "w");

    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos.\n");
        return 1;
    }

    int num_fontes;
    fscanf(entrada, "%d", &num_fontes);

    for (int id_source = 0; id_source < num_fontes; id_source++) {

        int qtd_bytes;
        fscanf(entrada, "%d", &qtd_bytes);

        unsigned char *dados = malloc(qtd_bytes);
        for (int i = 0; i < qtd_bytes; i++) {
            unsigned int temp;
            fscanf(entrada, "%x", &temp);
            dados[i] = (unsigned char) temp;
        }

        /* Executa compressões */
        ResultadoCompressao *huf = compressao_huffman(dados, qtd_bytes);
        ResultadoCompressao *rle = compressao_rle(dados, qtd_bytes);

        /* Decide o que imprimir */
        if (huf->taxa_compressao < rle->taxa_compressao) {
            imprimir_resultado(saida, id_source, huf);
        }
        else if (huf->taxa_compressao > rle->taxa_compressao) {
            imprimir_resultado(saida, id_source, rle);
        }
        else {
            imprimir_resultado(saida, id_source, huf);
            imprimir_resultado(saida, id_source, rle);
        }

        /* Liberação de memória */
        liberar_resultado(huf);
        liberar_resultado(rle);
        
        free(dados);
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}

/* --- Implementação Gerenciamento de Memória --- */

ByteBuffer* criar_buffer(int capacidade) {
    ByteBuffer* b = malloc(sizeof(ByteBuffer));
    b->capacidade = capacidade;
    b->tamanho = 0;
    b->bytes = calloc(capacidade, sizeof(unsigned char));
    return b;
}

void adicionar_byte(ByteBuffer* buffer, unsigned char b) {
    if (buffer->tamanho >= buffer->capacidade) {
        buffer->capacidade *= 2;
        buffer->bytes = realloc(buffer->bytes,
                                 buffer->capacidade * sizeof(unsigned char));
    }
    buffer->bytes[buffer->tamanho++] = b;
}

void liberar_buffer(ByteBuffer* buffer) {
    free(buffer->bytes);
    free(buffer);
}

ResultadoCompressao* criar_resultado(char* algoritmo) {
    ResultadoCompressao* r = malloc(sizeof(ResultadoCompressao));
    r->dados_comprimidos = criar_buffer(MAX_BUFFER_SIZE);
    strcpy(r->algoritmo, algoritmo);
    r->taxa_compressao = 0;
    return r;
}

void liberar_resultado(ResultadoCompressao* res) {
    liberar_buffer(res->dados_comprimidos);
    free(res);
}

void imprimir_resultado(FILE* saida, int id, ResultadoCompressao* r) {
    fprintf(saida, "%d->%s(%.2f%%)=",
            id, r->algoritmo, r->taxa_compressao);
    for (int i = 0; i < r->dados_comprimidos->tamanho; i++)
        fprintf(saida, "%02X", r->dados_comprimidos->bytes[i]);
    fprintf(saida, "\n");
}

/* --- Implementação RLE --- */

ResultadoCompressao* executar_rle(ByteBuffer* entrada) {
    ResultadoCompressao* res = criar_resultado("RLE");
    int i = 0;
    int len = entrada->tamanho;

    while (i < len) {
        unsigned char atual = entrada->bytes[i];
        int contagem = 1;

        /* Conta ocorrências consecutivas */
        while (i + 1 < len && entrada->bytes[i+1] == atual) {
            contagem++;
            i++;
            /* Limite de 1 byte para a contagem (255) */
            if (contagem == 255) break; 
        }

        /* Escreve no buffer de saída: [Quantidade] [Valor] */
        adicionar_byte(res->dados_comprimidos, (unsigned char)contagem);
        adicionar_byte(res->dados_comprimidos, atual);
        
        i++;
    }

    /* Calcula Taxa: (Tamanho Comprimido / Tamanho Original) * 100 */
    /* Proteção contra divisão por zero */
    if (len > 0)
        res->taxa_compressao = ((float)res->dados_comprimidos->tamanho / (float)len) * 100.0f;
    else
        res->taxa_compressao = 0.0f;

    return res;
}

/* --- Implementação Huffman --- */

MinHeapNode* novo_no(unsigned char dado, unsigned freq) {
    MinHeapNode* temp = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    temp->esq = temp->dir = NULL;
    temp->dado = dado;
    temp->freq = freq;
    return temp;
}

MinHeap* criar_min_heap(unsigned capacidade) {
    MinHeap* min_heap = (MinHeap*)malloc(sizeof(MinHeap));
    min_heap->tamanho = 0;
    min_heap->capacidade = capacidade;
    min_heap->array = (MinHeapNode**)malloc(min_heap->capacidade * sizeof(MinHeapNode*));
    return min_heap;
}

void trocar_no(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void min_heapify(MinHeap* min_heap, int idx) {
    int menor = idx;
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2;

    if (esq < min_heap->tamanho && min_heap->array[esq]->freq < min_heap->array[menor]->freq)
        menor = esq;

    if (dir < min_heap->tamanho && min_heap->array[dir]->freq < min_heap->array[menor]->freq)
        menor = dir;

    if (menor != idx) {
        trocar_no(&min_heap->array[menor], &min_heap->array[idx]);
        min_heapify(min_heap, menor);
    }
}

MinHeapNode* extrair_min(MinHeap* min_heap) {
    MinHeapNode* temp = min_heap->array[0];
    min_heap->array[0] = min_heap->array[min_heap->tamanho - 1];
    --min_heap->tamanho;
    min_heapify(min_heap, 0);
    return temp;
}

void inserir_min_heap(MinHeap* min_heap, MinHeapNode* min_heap_node) {
    ++min_heap->tamanho;
    int i = min_heap->tamanho - 1;
    while (i && min_heap_node->freq < min_heap->array[(i - 1) / 2]->freq) {
        min_heap->array[i] = min_heap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    min_heap->array[i] = min_heap_node;
}

int e_tamanho_um(MinHeap* min_heap) {
    return (min_heap->tamanho == 1);
}

MinHeapNode* construir_arvore_huffman(unsigned char dados[], int freq[], int tamanho) {
    MinHeapNode *esq, *dir, *top;
    MinHeap* min_heap = criar_min_heap(tamanho);

    for (int i = 0; i < tamanho; ++i)
        inserir_min_heap(min_heap, novo_no(dados[i], freq[i]));

    while (!e_tamanho_um(min_heap)) {
        esq = extrair_min(min_heap);
        dir = extrair_min(min_heap);
        /* '$' é valor interno especial */
        top = novo_no('$', esq->freq + dir->freq);
        top->esq = esq;
        top->dir = dir;
        inserir_min_heap(min_heap, top);
    }

    MinHeapNode* raiz = extrair_min(min_heap);
    
    /* Limpeza da estrutura do Heap (não dos nós da árvore) */
    free(min_heap->array);
    free(min_heap);
    
    return raiz;
}

/* Gera a tabela de códigos (strings de "0" e "1") */
void gerar_codigos(MinHeapNode* raiz, int arr[], int topo, char* codigos[256]) {
    if (raiz->esq) {
        arr[topo] = 0;
        gerar_codigos(raiz->esq, arr, topo + 1, codigos);
    }
    if (raiz->dir) {
        arr[topo] = 1;
        gerar_codigos(raiz->dir, arr, topo + 1, codigos);
    }
    if (!(raiz->esq) && !(raiz->dir)) {
        /* Nó folha encontrado: salva o código */
        codigos[raiz->dado] = (char*)malloc((topo + 1) * sizeof(char));
        for (int i = 0; i < topo; i++) {
            codigos[raiz->dado][i] = arr[i] ? '1' : '0';
        }
        codigos[raiz->dado][topo] = '\0';
    }
}

void liberar_arvore_huffman(MinHeapNode* raiz) {
    if (raiz == NULL) return;
    liberar_arvore_huffman(raiz->esq);
    liberar_arvore_huffman(raiz->dir);
    free(raiz);
}

ResultadoCompressao* executar_huffman(ByteBuffer* entrada) {
    ResultadoCompressao* res = criar_resultado("HUF");
    
    if (entrada->tamanho == 0) {
        res->taxa_compressao = 0.0;
        return res;
    }

    /* 1. Análise de Frequência */
    int freq[256] = {0};
    for (int i = 0; i < entrada->tamanho; i++) {
        freq[entrada->bytes[i]]++;
    }

    /* 2. Prepara vetores para Heap */
    unsigned char dados_unicos[256];
    int freqs_unicas[256];
    int tamanho_unicos = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            dados_unicos[tamanho_unicos] = (unsigned char)i;
            freqs_unicas[tamanho_unicos] = freq[i];
            tamanho_unicos++;
        }
    }

    /* Caso especial: Apenas 1 tipo de byte repetido (entropia zero) */
    /* Ex: AA AA AA. Huffman precisa de min 1 bit. */
    char* tabela_codigos[256] = {0};
    
    if (tamanho_unicos == 1) {
        /* Atribui código "0" para o único caractere */
        tabela_codigos[dados_unicos[0]] = strdup("0"); 
    } else {
        /* Constrói árvore e gera códigos */
        MinHeapNode* raiz = construir_arvore_huffman(dados_unicos, freqs_unicas, tamanho_unicos);
        int arr[MAX_TREE_HT];
        gerar_codigos(raiz, arr, 0, tabela_codigos);
        liberar_arvore_huffman(raiz);
    }

    /* 3. Bit Packing (Empacotamento de bits) */
    unsigned char byte_atual = 0;
    int contador_bits = 0;

    for (int i = 0; i < entrada->tamanho; i++) {
        char* codigo = tabela_codigos[entrada->bytes[i]];
        for (int j = 0; codigo[j] != '\0'; j++) {
            /* Desloca byte atual para esquerda e insere o bit (0 ou 1) */
            byte_atual = byte_atual << 1;
            if (codigo[j] == '1') {
                byte_atual = byte_atual | 1;
            }
            contador_bits++;

            /* Se completou 8 bits, escreve no buffer */
            if (contador_bits == 8) {
                adicionar_byte(res->dados_comprimidos, byte_atual);
                contador_bits = 0;
                byte_atual = 0;
            }
        }
    }

    /* Padding: Se sobraram bits, desloca para o topo do byte */
    if (contador_bits > 0) {
        byte_atual = byte_atual << (8 - contador_bits);
        adicionar_byte(res->dados_comprimidos, byte_atual);
    }

    /* Limpeza da tabela de códigos */
    for (int i = 0; i < 256; i++) {
        if (tabela_codigos[i]) free(tabela_codigos[i]);
    }

    /* Cálculo da taxa */
    res->taxa_compressao = ((float)res->dados_comprimidos->tamanho / (float)entrada->tamanho) * 100.0f;

    return res;
}

ResultadoCompressao* compressao_rle(unsigned char *dados, int qtd_bytes) {
    ByteBuffer* entrada = criar_buffer(qtd_bytes);
    for (int i = 0; i < qtd_bytes; i++) {
        adicionar_byte(entrada, dados[i]);
    }

    ResultadoCompressao* res = executar_rle(entrada);
    liberar_buffer(entrada);
    return res;
}

ResultadoCompressao* compressao_huffman(unsigned char *dados, int qtd_bytes) {
    ByteBuffer* entrada = criar_buffer(qtd_bytes);
    for (int i = 0; i < qtd_bytes; i++) {
        adicionar_byte(entrada, dados[i]);
    }

    ResultadoCompressao* res = executar_huffman(entrada);
    liberar_buffer(entrada);
    return res;
}
