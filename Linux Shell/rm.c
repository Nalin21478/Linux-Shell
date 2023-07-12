#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int is_folder(char name[])
{
    struct dirent **files;
    int n = scandir(".", &files, NULL, alphasort);
    int flag = 0;
    
    for (int i = 0; i < n; ++i)
    {
        if (strcmp(name, files[i]->d_name) == 0)
        {
            if (files[i]->d_type == DT_DIR)
                flag = 1;
        }
        free(files[i]);
    }
    
    free(files);
    return flag;
}

void delete_files(int argc, char *argv[], int flag_prompt, int flag_explain)
{
    if ((flag_prompt == 1 || flag_explain == 1) && argc == 2)
    {
        printf("Insufficient arguments received\n");
        return;
    }
    
    char c = 'Y';
    for (int i = 2; i < argc; ++i)
    {
        if (is_folder(argv[i]))
        {
            printf("%s is a directory, can't delete\n", argv[i]);
        }
        else
        {
            if (flag_prompt == 1)
            {
                printf("Do you want to remove %s [Y/N]: ", argv[i]);
                scanf(" %c", &c);
                if (c == 'N')
                    continue;
            }
            
            int r = remove(argv[i]);
            if (r == -1)
                printf("%s could not be removed\n", argv[i]);
            
            if (flag_explain == 1)
                printf("%s is removed\n", argv[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Insufficient arguments received\n");
    }
    else
    {
        int i = 1;
        int flag_prompt = 0;
        int flag_explain = 0;
        
        if (strcmp(argv[1], "-i") == 0)
        {
            flag_prompt = 1;
            ++i;
        }
        else if (strcmp(argv[1], "-v") == 0)
        {
            flag_explain = 1;
            ++i;
        }
        
        delete_files(argc, argv, flag_prompt, flag_explain);
    }

    return 0;
}
