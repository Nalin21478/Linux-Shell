#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void print_sorted_directory(int print_all)
{
    struct dirent **file_list;
    int count = scandir(".", &file_list, NULL, alphasort);
    
    for (int i = 0; i < count; ++i)
    {
        if (!print_all && (strcmp(file_list[i]->d_name, ".") == 0 || strcmp(file_list[i]->d_name, "..") == 0))
            continue;
        
        printf("%s\n", file_list[i]->d_name);
        free(file_list[i]);
    }
    
    free(file_list);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
        print_sorted_directory(0);
    else if (argc > 2)
        printf("Excess arguments\n");
    else if (strcmp(argv[1], "-A") == 0)
        print_sorted_directory(1);
    else if (strcmp(argv[1], "-a") == 0)
        print_sorted_directory(0);
    else
        printf("Unrecognized flag\n");

    return 0;
}
