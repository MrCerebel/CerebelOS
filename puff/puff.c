#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINES 1000
#define MAX_LINE_LEN 1024

/* SFO */
/* Puff - A simple text editor for CerebelOS */

char lines[MAX_LINES][MAX_LINE_LEN];
int num_lines = 0;
int cursor_row = 0;
int cursor_col = 0;
char filename[256];
int command_mode = 0;

void clear_screen() {
    printf("\033[2J\033[H");
}

void draw_editor() {
    clear_screen();
    if (command_mode)
        printf("Puff - %s | [COMMAND] s: Save | q: Quit\n", filename);
    else
        printf("Puff - %s | ESC: Command mode\n", filename);
    printf("----------------------------------------\n");
    for (int i = 0; i < num_lines; i++) {
        int len = strlen(lines[i]);
        for (int j = 0; j <= len; j++) {
            if (i == cursor_row && j == cursor_col)
                printf("\033[7m%c\033[0m", j < len ? lines[i][j] : ' ');
            else if (j < len)
                printf("%c", lines[i][j]);
        }
        printf("\n");
    }
}

void load_file(const char *path) {
    strcpy(filename, path);
    FILE *f = fopen(path, "r");
    if (!f) {
        num_lines = 1;
        lines[0][0] = '\0';
        return;
    }
    while (fgets(lines[num_lines], MAX_LINE_LEN, f) && num_lines < MAX_LINES) {
        lines[num_lines][strcspn(lines[num_lines], "\n")] = '\0';
        num_lines++;
    }
    fclose(f);
    if (num_lines == 0) {
        num_lines = 1;
        lines[0][0] = '\0';
    }
}

void save_file() {
    FILE *f = fopen(filename, "w");
    if (!f) return;
    for (int i = 0; i < num_lines; i++)
        fprintf(f, "%s\n", lines[i]);
    fclose(f);
}

int read_key() {
    int c = getchar();
    if (c == 27) {
        int c2 = getchar();
        if (c2 == '[') {
            int c3 = getchar();
            if (c3 == 'A') return 1000; /* up */
                if (c3 == 'B') return 1001; /* down */
                    if (c3 == 'C') return 1002; /* right */
                        if (c3 == 'D') return 1003; /* left */
        }
        return 27; /* plain ESC */
    }
    return c;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: puff <filename>\n");
        return 1;
    }

    load_file(argv[1]);

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (1) {
        draw_editor();

        int c = read_key();

        if (c == 27) {
            command_mode = !command_mode;
        } else if (command_mode) {
            if (c == 'q') break;
            if (c == 's') save_file();
        } else if (c == 1000 && cursor_row > 0) {
            cursor_row--;
            if (cursor_col > (int)strlen(lines[cursor_row]))
                cursor_col = strlen(lines[cursor_row]);
        } else if (c == 1001 && cursor_row < num_lines - 1) {
            cursor_row++;
            if (cursor_col > (int)strlen(lines[cursor_row]))
                cursor_col = strlen(lines[cursor_row]);
        } else if (c == 1002) {
            if (cursor_col < (int)strlen(lines[cursor_row]))
                cursor_col++;
        } else if (c == 1003) {
            if (cursor_col > 0)
                cursor_col--;
        } else if (c == '\n') {
            num_lines++;
            for (int i = num_lines - 1; i > cursor_row + 1; i--)
                strcpy(lines[i], lines[i-1]);
            lines[cursor_row + 1][0] = '\0';
            cursor_row++;
            cursor_col = 0;
        } else if (c == 127) {
            int len = strlen(lines[cursor_row]);
            if (cursor_col > 0) {
                memmove(&lines[cursor_row][cursor_col-1],
                        &lines[cursor_row][cursor_col],
                        len - cursor_col + 1);
                cursor_col--;
            }
        } else if (c >= 32 && c < 127) {
            int len = strlen(lines[cursor_row]);
            if (len < MAX_LINE_LEN - 1) {
                memmove(&lines[cursor_row][cursor_col+1],
                        &lines[cursor_row][cursor_col],
                        len - cursor_col + 1);
                lines[cursor_row][cursor_col] = c;
                cursor_col++;
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    clear_screen();
    printf("Goodbye from Puff!\n");
    return 0;
}
