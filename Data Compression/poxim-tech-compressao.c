#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256
#define MAX_BUFFER_SIZE 20000

/* ================= ESTRUTURAS ================= */

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

/* ================= PROTÓTIPOS ================= */

/* Gerenciamento de Memória e Buffer */
ByteBuffer* criar_buffer(int capacidade);
void adicionar_byte(ByteBuffer* buffer, unsigned char b);
void liberar_buffer(ByteBuffer* buffer);
ResultadoCompressao* criar_resultado(char* algoritmo);
void liberar_resultado(ResultadoCompressao* res);
void imprimir_resultado(FILE* saida, int id_source, ResultadoCompressao* res);

/* Algoritmos de Compressão */
ResultadoCompressao* compressao_rle(unsigned char *dados, int qtd_bytes);
ResultadoCompressao* compressao_huffman(unsigned char *dados, int qtd_bytes);
ResultadoCompressao* executar_rle(ByteBuffer* entrada);
ResultadoCompressao* executar_huffman(ByteBuffer* entrada);

/* Funções Auxiliares de Ordenação (Quicksort) */
void meu_quicksort(MinHeapNode** arr, int baixo, int alto);
int particao(MinHeapNode** arr, int baixo, int alto);
int eh_prioritario(MinHeapNode* a, MinHeapNode* b);
void trocar_elem_array(MinHeapNode** a, MinHeapNode** b);

/* Funções de Heap e Árvore de Huffman */
MinHeapNode* novo_no(unsigned char dado, unsigned freq);
MinHeap* criar_min_heap(unsigned capacidade);
void min_heapify(MinHeap* min_heap, int idx);
MinHeapNode* extrair_min(MinHeap* min_heap);
void inserir_min_heap(MinHeap* min_heap, MinHeapNode* node);
int e_tamanho_um(MinHeap* min_heap);
MinHeapNode* construir_arvore_huffman(unsigned char dados[], int freq[], int tamanho);
void gerar_codigos(MinHeapNode* raiz, int arr[], int topo, char* codigos[256]);
void liberar_arvore_huffman(MinHeapNode* raiz);

/* ================= MAIN ================= */

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    FILE *saida   = fopen(argv[2], "w");

    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos.\n");
        if (entrada) fclose(entrada);
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

        /* Decide o que imprimir (Critério: menor taxa vence. Empate: Huffman vence) */
        if (huf->taxa_compressao <= rle->taxa_compressao) {
            imprimir_resultado(saida, id_source, huf);
        }
        else {
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

/* ================= IMPLEMENTAÇÕES GERAIS ================= */

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
        buffer->bytes = realloc(buffer->bytes, buffer->capacidade * sizeof(unsigned char));
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
    fprintf(saida, "%d->%s(%.2f%%)=", id, r->algoritmo, r->taxa_compressao);
    for (int i = 0; i < r->dados_comprimidos->tamanho; i++)
        fprintf(saida, "%02X", r->dados_comprimidos->bytes[i]);
    fprintf(saida, "\n");
}

/* ================= IMPLEMENTAÇÃO QUICKSORT & CRITÉRIOS ================= */

/* CRITÉRIO DETERMINÍSTICO DE COMPARAÇÃO (Crucial para F8 vs F9)
   Retorna 1 se 'a' tem prioridade sobre 'b' (deve vir antes).
   1. Menor Frequência.
   2. Desempate: Menor valor ASCII.
*/
int eh_prioritario(MinHeapNode* a, MinHeapNode* b) {
    if (a->freq < b->freq) return 1;
    if (a->freq == b->freq && a->dado < b->dado) return 1;
    return 0;
}

void trocar_elem_array(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

int particao(MinHeapNode** arr, int baixo, int alto) {
    MinHeapNode* pivo = arr[alto]; 
    int i = (baixo - 1); 

    for (int j = baixo; j <= alto - 1; j++) {
        /* Se arr[j] <= pivo (com nosso critério de desempate) */
        if (eh_prioritario(arr[j], pivo) || (arr[j]->freq == pivo->freq && arr[j]->dado == pivo->dado)) {
            i++; 
            trocar_elem_array(&arr[i], &arr[j]);
        }
    }
    trocar_elem_array(&arr[i + 1], &arr[alto]);
    return (i + 1);
}

void meu_quicksort(MinHeapNode** arr, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particao(arr, baixo, alto);
        meu_quicksort(arr, baixo, pi - 1);
        meu_quicksort(arr, pi + 1, alto);
    }
}

/* ================= IMPLEMENTAÇÃO MIN-HEAP (ÁRVORE PADRÃO) ================= */

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

void trocar_no_heap(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void min_heapify(MinHeap* min_heap, int idx) {
    int menor = idx;
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2;

    /* Usa eh_prioritario para decidir quem sobe no Heap */
    if (esq < min_heap->tamanho && eh_prioritario(min_heap->array[esq], min_heap->array[menor]))
        menor = esq;

    if (dir < min_heap->tamanho && eh_prioritario(min_heap->array[dir], min_heap->array[menor]))
        menor = dir;

    if (menor != idx) {
        trocar_no_heap(&min_heap->array[menor], &min_heap->array[idx]);
        min_heapify(min_heap, menor);
    }
}

int e_tamanho_um(MinHeap* min_heap) {
    return (min_heap->tamanho == 1);
}

MinHeapNode* extrair_min(MinHeap* min_heap) {
    MinHeapNode* temp = min_heap->array[0];
    min_heap->array[0] = min_heap->array[min_heap->tamanho - 1];
    --min_heap->tamanho;
    min_heapify(min_heap, 0);
    return temp;
}

void inserir_min_heap(MinHeap* min_heap, MinHeapNode* node) {
    ++min_heap->tamanho;
    int i = min_heap->tamanho - 1;
    
    /* Bubble up usando o critério de prioridade */
    while (i && eh_prioritario(node, min_heap->array[(i - 1) / 2])) {
        min_heap->array[i] = min_heap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    min_heap->array[i] = node;
}

MinHeapNode* construir_arvore_huffman(unsigned char dados[], int freq[], int tamanho) {
    MinHeapNode *esq, *dir, *top;

    /* 1. Criar Min Heap */
    MinHeap* min_heap = criar_min_heap(tamanho);

    /* 2. Criar array de nós iniciais */
    MinHeapNode** nos_iniciais = (MinHeapNode**)malloc(tamanho * sizeof(MinHeapNode*));
    for (int i = 0; i < tamanho; ++i) {
        nos_iniciais[i] = novo_no(dados[i], freq[i]);
    }

    /* 3. Ordenar entrada (Requisito do professor + Determinismo) */
    meu_quicksort(nos_iniciais, 0, tamanho - 1);

    /* 4. Popular o Heap */
    for (int i = 0; i < tamanho; ++i) {
        inserir_min_heap(min_heap, nos_iniciais[i]);
    }
    free(nos_iniciais); 

    /* 5. Algoritmo Padrão de Huffman (Loop while size > 1) */
    while (!e_tamanho_um(min_heap)) {
        // Extrai os dois menores
        esq = extrair_min(min_heap);
        dir = extrair_min(min_heap);

        // Cria nó interno (soma das frequências)
        top = novo_no('$', esq->freq + dir->freq);
        top->esq = esq;
        top->dir = dir;

        inserir_min_heap(min_heap, top);
    }

    // A sobra é a raiz
    MinHeapNode* raiz = extrair_min(min_heap);
    
    // Limpa a estrutura do heap (os nós da árvore ficam)
    free(min_heap->array);
    free(min_heap);
    
    return raiz;
}

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

/* ================= COMPRESSÃO E LÓGICA FINAL ================= */

ResultadoCompressao* executar_huffman(ByteBuffer* entrada) {
    ResultadoCompressao* res = criar_resultado("HUF");
    
    if (entrada->tamanho == 0) {
        res->taxa_compressao = 0.0;
        return res;
    }

    int freq[256] = {0};
    for (int i = 0; i < entrada->tamanho; i++) freq[entrada->bytes[i]]++;

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

    char* tabela_codigos[256] = {0};
    
    if (tamanho_unicos == 1) {
        tabela_codigos[dados_unicos[0]] = strdup("0"); 
    } else {
        /* Chama a nova função padronizada com Heap */
        MinHeapNode* raiz = construir_arvore_huffman(dados_unicos, freqs_unicas, tamanho_unicos);
        int arr[MAX_TREE_HT];
        gerar_codigos(raiz, arr, 0, tabela_codigos);
        liberar_arvore_huffman(raiz);
    }

    unsigned char byte_atual = 0;
    int contador_bits = 0;

    for (int i = 0; i < entrada->tamanho; i++) {
        char* codigo = tabela_codigos[entrada->bytes[i]];
        for (int j = 0; codigo[j] != '\0'; j++) {
            byte_atual = (byte_atual << 1) | (codigo[j] == '1' ? 1 : 0);
            contador_bits++;

            if (contador_bits == 8) {
                adicionar_byte(res->dados_comprimidos, byte_atual);
                contador_bits = 0;
                byte_atual = 0;
            }
        }
    }

    if (contador_bits > 0) {
        byte_atual = byte_atual << (8 - contador_bits);
        adicionar_byte(res->dados_comprimidos, byte_atual);
    }

    for (int i = 0; i < 256; i++) if (tabela_codigos[i]) free(tabela_codigos[i]);

    res->taxa_compressao = ((float)res->dados_comprimidos->tamanho / (float)entrada->tamanho) * 100.0f;
    return res;
}

ResultadoCompressao* executar_rle(ByteBuffer* entrada) {
    ResultadoCompressao* res = criar_resultado("RLE");
    int i = 0;
    int len = entrada->tamanho;

    while (i < len) {
        unsigned char atual = entrada->bytes[i];
        int contagem = 1;
        while (i + 1 < len && entrada->bytes[i+1] == atual) {
            contagem++;
            i++;
            if (contagem == 255) break; 
        }
        adicionar_byte(res->dados_comprimidos, (unsigned char)contagem);
        adicionar_byte(res->dados_comprimidos, atual);
        i++;
    }

    if (len > 0) res->taxa_compressao = ((float)res->dados_comprimidos->tamanho / (float)len) * 100.0f;
    else res->taxa_compressao = 0.0f;

    return res;
}

ResultadoCompressao* compressao_rle(unsigned char *dados, int qtd_bytes) {
    ByteBuffer* entrada = criar_buffer(qtd_bytes);
    for (int i = 0; i < qtd_bytes; i++) adicionar_byte(entrada, dados[i]);
    ResultadoCompressao* res = executar_rle(entrada);
    liberar_buffer(entrada);
    return res;
}

ResultadoCompressao* compressao_huffman(unsigned char *dados, int qtd_bytes) {
    ByteBuffer* entrada = criar_buffer(qtd_bytes);
    for (int i = 0; i < qtd_bytes; i++) adicionar_byte(entrada, dados[i]);
    ResultadoCompressao* res = executar_huffman(entrada);
    liberar_buffer(entrada);
    return res;
}