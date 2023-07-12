#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_lines_from_stdin(int* line_count, int is_line_number, int is_mark_end) {
    char line[1000];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        ++(*line_count);
        if (is_line_number)
            printf("%5d ", *line_count);
        printf("%s", line);
        if (is_mark_end)
            printf("$\n");
        else
            printf("\n");
    }
}

void print_lines_from_file(FILE* file, int* line_count, int is_line_number, int is_mark_end) {
    char line[1000];
    while (fgets(line, sizeof(line), file) != NULL) {
        ++(*line_count);
        if (is_line_number)
            printf("%5d ", *line_count);
        printf("%s", line);
        if (is_mark_end)
            printf("$\n");
        else
            printf("\n");
    }
}

void print_file_content(char stream[1000], int* line_count, int is_line_number, int is_mark_end) {
    if (strcmp(stream, "-") == 0)
        print_lines_from_stdin(line_count, is_line_number, is_mark_end);
    else {
        FILE* file = fopen(stream, "r");
        if (file == NULL)
            printf("cat: '%s': No such file or directory\n", stream);
        else {
            print_lines_from_file(file, line_count, is_line_number, is_mark_end);
            fclose(file);
        }
    }
}

int main(int argc, char* argv[]) {
    int line_count = 0;
    int is_line_number = 0;
    int is_mark_end = 0;
    
    if (argc == 1)
        print_lines_from_stdin(&line_count, is_line_number, is_mark_end);
    
    int i = 1;
    if (strcmp(argv[1], "-n") == 0) {
        is_line_number = 1;
        ++i;
    } else if (strcmp(argv[1], "-E") == 0) {
        is_mark_end = 1;
        ++i;
    }
    
    for (; i < argc; ++i)
        print_file_content(argv[i], &line_count, is_line_number, is_mark_end);

    return 0;
}
