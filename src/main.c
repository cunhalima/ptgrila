#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define FILE_NAME   "data/auto.txt"

static int num_states;
static int num_symbols;
static int *table;
static int *states;
static char *symbols;
static bool *final_states;
static int sym2col[256];

#define TAB(state, symbol) table[(state)*num_symbols+symbol]


void readfile(FILE *file, int phase)
{
    int row;
    char *lineptr;

    fseek(file, 0, SEEK_SET);
    row = 0;
    for (;;) {
        size_t n;
        char *endptr;
        int cols;
        int nread;

        lineptr = NULL;
        nread = getline(&lineptr, &n, file);
        if (nread < 0) {
            free(lineptr);
            break;
        }
        endptr = lineptr;
        cols = 0;
        if (row == 0) {
            int i;
            for (i = 0; lineptr[i] != '\0'; i++) {
                unsigned char c = lineptr[i];
                if (c >= 'A' && c <= 'Z') {
                    c += ('a' - 'A');
                }
                if (c > ' ') {
                    if (phase == 1) {
                        symbols[cols] = c;
                        sym2col[(int)c] = cols + 1;
                    }
                    cols++;
                }
            }
        } else {
            for (;;) {
                const char *oldnptr = endptr;
                int number = strtol(endptr, &endptr, 0);
                if (number == 0 && oldnptr == endptr) {
                    break;
                }
                if (phase == 1 && cols == 0) {
                    states[row - 1] = number;
                    final_states[row - 1] = (number < 0);
                } else if (phase == 2 && cols > 0) {
                    int i;
                    int state_code;

                    if (number == 0) {
                        state_code = -1;
                    } else {
                        state_code = -1;
                        for (i = 0; i < num_states; i++) {
                            if (states[i] == number) {
                                state_code = i;
                                break;
                            }
                        }
                    }
                    TAB(row - 1, cols - 1) = state_code;
                }
                cols++;
            }
            cols--;
        }
        //printf("%d agora %d\n", row, cols);
        if (num_symbols < cols) {
            num_symbols = cols;
        } else if (cols < num_symbols) {
            fprintf(stderr, "ERROR! wrong number of symbols on row %d\n", row + 1);
        }
        row++;
        free(lineptr);
    }
    row--;
    num_states = row;
    //printf("%d\n", num_symbols);
    //printf("%d\n", num_states);
}

static void showtab(void)
{
    int i, j;

    for (i = 0; i < num_symbols; i++) {
        printf("%c", symbols[i]);
    }
    printf("\n");
    for (i = 0; i < num_states; i++) {
        printf("%c %d\n", final_states[i] ? '*' : ' ', states[i]);
    }
    printf("\n");
    for (i = 0; i < num_states; i++) {
        for (j = 0; j < num_symbols; j++) {
            printf("%d ", TAB(i, j));
        }
        printf("\n");
    }
}

bool deuerro = false;

static const char *doparse(const char *tape)
{
    int i;
    int state;
    while(isspace(*tape)) {
        tape++;
    }
    if (*tape == 0) {
        return NULL;
    }
    for (state = 0, i = 0; tape[i] != '\0' && !isspace(tape[i]); i++) {
        unsigned char sym = tape[i];
        //if (sym == '\n' || sym == '\r') {
        //    sym = ' ';
        //}
        int col = sym2col[(int)sym];
        //printf("state %d(%d)ffoi %c %d\n", state, states[state], sym, col);
        if (col == 0) {
            //printf("symbol not found\n");
            printf("%d ", -1);
            deuerro = true;
            return NULL;
        }
        col--;
        state = TAB(state, col);
        if (state < 0) {
            //printf("unexpected symbol (%c)\n", sym);
            deuerro = true;
            printf("%d ", -1);
            return NULL;
        }
    }
    printf("%d ", states[state]);
    //printf("State = %d\n", states[state]);
    return (tape[i] == '\0') ? NULL : &tape[i + 1];
}

static void lexparse(const char *tape)
{
    do {
        tape = doparse(tape);
    } while(tape != NULL);
}

int main(void)
{
    FILE *file;
    int nread;
    size_t n;
    char *lineptr;

    file = fopen(FILE_NAME, "rt");
    if (file == NULL) {
        fprintf(stderr, "Unable to open %s\n", FILE_NAME);
        return 1;
    }
    num_states = 0;
    num_symbols = 0;
    memset(sym2col, 0, sizeof(sym2col));
    readfile(file, 0);
    table = (int *)malloc(sizeof(int) * num_states * num_symbols);
    symbols = (char *)malloc(sizeof(char) * num_symbols);
    states = (int *)malloc(sizeof(int) * num_states);
    final_states = (bool *)malloc(sizeof(bool) * num_states);
    readfile(file, 1);
    readfile(file, 2);

    char *essalinha = NULL;
    int tamatual = 0;
    //showtab();
    for (;;) {
        lineptr = NULL;
        nread = getline(&lineptr, &n, stdin);
        if (lineptr != NULL) {
            if (nread <= 0) {
                free(lineptr);
                break;
            }
            essalinha = realloc(essalinha, tamatual + nread + 1);
            if (tamatual == 0) {
                essalinha[0] = 0;
            }
            strcat(essalinha, lineptr);
            tamatual += nread;
            //lexparse("SI PUIS SINON PENDANT FAIRE PAIRE ENSEMBLE FLOTTANT LETTRE MAT COPRENDRE PRINCIPALE RETOUR TRUE NALSE $AAA $CBA * / + - = != % < > <= >= : ( ) ^ V //");
            //lexparse("si i puis sinon pendant faire paire ensemble flottant lettre mat coprendre principale retour true nalse $aaa $cba * / + - = != % < > <= >= : ( ) ^ v //");
            //printf("aaa (%s) %d\n", lineptr, nread);
            free(lineptr);
            //fflush(stdout);
        } else {
            break;
        }
        if (nread == 0) {
            break;
        }
    }
    //printf("%s\n", essalinha);
    lexparse(essalinha);
    if (deuerro) {
        //printf("erro\n");
    } else {
        printf("%d ", 0);
    }
    printf("\n");
    free(essalinha);
    //lexparse("SI PUIS");
    fclose(file);
    free(states);
    free(symbols);
    free(table);
    return 0;
}
