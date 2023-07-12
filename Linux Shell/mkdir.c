#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

int is_directory_present(char name[])
{
	struct dirent **files;
	int count = scandir(".", &files, NULL, alphasort);
	int flag = 0;
	
	for (int i = 0; i < count; ++i)
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

void print_help()
{
	printf("                          Functions Manual\n");
	printf("\n");
	printf("NAME\n");
	printf("	mkdir\n");
	printf("\n");
	printf("SYNOPSIS\n");
	printf("	mkdir [OPTION] fileName\n");
	printf("\n");
	printf("DESCRIPTION\n");
	printf("	makes a directory.\n");
	printf("\n");
	printf("OPTION\n");
	printf("	mkdir fileName\n");
	printf("		makes a directory with name fileName\n");
	printf("	mkdir -v fileName\n");
	printf("		makes a directory and tells the status after creation\n");
	printf("	mkdir --help\n");
	printf("		display this help\n");
}

void make_directory(int explain, char *names[], int count)
{
	for (int i = 0; i < count; ++i)
	{
		if (is_directory_present(names[i]))
		{
			printf("%s is already present, can't create another\n", names[i]);
		}
		else
		{
			int mkdir_status = mkdir(names[i], 0777);
			
			if (mkdir_status == 0)
			{
				// Directory created successfully
			}
			else
			{
				printf("%s could not be created\n", names[i]);
			}
			
			if (explain == 1)
			{
				printf("%s is created\n", names[i]);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("Insufficient arguments received\n");
	}
	else if (argc == 2 && strcmp(argv[1], "--help") == 0)
	{
		print_help();
	}
	else
	{
		int explain_flag = 0;
		int start_index = 1;
		
		if (strcmp(argv[1], "-v") == 0)
		{
			explain_flag = 1;
			start_index = 2;
		}
		
		if (explain_flag == 1 && argc == 2)
		{
			printf("Insufficient arguments received\n");
		}
		else
		{
			make_directory(explain_flag, &argv[start_index], argc - start_index);
		}
	}

	return 0;
}
