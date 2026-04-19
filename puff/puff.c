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

void clear_screen() {
    printf("\033[2J\033[H");
}

void draw_editor() {
    clear_screen();
    printf("Puff - %s | ESC then s: Save | ESC then q: Quit\n", filename);
    printf("----------------------------------------\n");
    for (int i = 0; i < num_lines; i++) {
        if (i == cursor_row)
            printf("\033[7m%s\033[0m\n", lines[i]);
        else
            printf("%s\n", lines[i]);
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

        int c = getchar();

        static int command_mode = 0;

	if (c == 27) { /* Escape */
    		command_mode = 1;
	} else if (command_mode) {
    	if (c == 'q') {
        	break;
    	} else if (c == 's') {
        	save_file();
    	}
    command_mode = 0;
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
