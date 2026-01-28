#include <stdio.h>
#include <stdlib.h>

#define MAX 100

#define NORTE 0
#define LESTE 1
#define SUL   2
#define OESTE 3

char lab[MAX][MAX];
int visitado[MAX][MAX];

int H, W;
int start_x, start_y;
FILE *out;

/* Movimentos absolutos: NORTE, LESTE, SUL, OESTE */
int dx[4] = { -1, 0, 1, 0 };
int dy[4] = { 0, 1, 0, -1 };

/* Direções relativas: D, F, E, T */
int rel_dir[4] = {
    1,  // Direita
    0,  // Frente
    3,  // Esquerda
    2   // Trás
};

char rel_char[4] = { 'D', 'F', 'E', 'T' };

int eh_saida(int x, int y) {
    if (x == start_x && y == start_y)
        return 0;

    return (x == 0 || y == 0 || x == H - 1 || y == W - 1);
}

int valido(int x, int y) {
    return x >= 0 && x < H && y >= 0 && y < W &&
           lab[x][y] != '1' && !visitado[x][y];
}

int dfs(int x, int y, int dir) {
    if (eh_saida(x, y)) {
        fprintf(out, "|FIM@%d,%d", x, y);
        return 1;
    }

    visitado[x][y] = 1;

    for (int i = 0; i < 4; i++) {
        int nova_dir = (dir + rel_dir[i]) % 4;
        int nx = x + dx[nova_dir];
        int ny = y + dy[nova_dir];

        if (valido(nx, ny)) {
            fprintf(out, "|%c->%d,%d", rel_char[i], nx, ny);

            if (dfs(nx, ny, nova_dir))
                return 1;

            fprintf(out, "|BT@%d,%d->%d,%d", nx, ny, x, y);
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada.txt saida.txt\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    out = fopen(argv[2], "w");

    if (!in || !out) {
        fprintf(stderr, "Erro ao abrir arquivos\n");
        return 1;
    }

    int casos;
    fscanf(in, "%d", &casos);

    for (int l = 0; l < casos; l++) {
        fscanf(in, "%d %d", &W, &H);

        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                fscanf(in, " %c", &lab[i][j]);
                visitado[i][j] = 0;

                if (lab[i][j] == 'X') {
                    start_x = i;
                    start_y = j;
                }
            }
        }

        fprintf(out, "L%d:INI@%d,%d", l, start_x, start_y);

        if (!dfs(start_x, start_y, NORTE)) {
            fprintf(out, "|FIM@-,-");
        }

        if (l < casos - 1)
            fprintf(out, "\n");
    }

    fclose(in);
    fclose(out);
    return 0;
}