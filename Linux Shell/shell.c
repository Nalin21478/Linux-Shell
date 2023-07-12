#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 1000

void execute_command(char tokens[][MAX_SIZE], int size) {
    char commands[][10] = {"cd", "echo", "history", "pwd", "exit", "ls", "cat", "date", "rm", "mkdir"};
    int commandsSize = 10;

    char fileName[MAX_SIZE];
    char absolutePath[MAX_SIZE];
    FILE *historyFile;

    if (getcwd(absolutePath, sizeof(absolutePath)) == NULL) {
        printf("Serious breakdown, terminating program\n");
        return;
    }

    strcat(absolutePath, "/");
    strcpy(fileName, absolutePath);
    strcat(fileName, "history.txt");
    historyFile = fopen(fileName, "a+");
    if (historyFile == NULL) {
        printf("Unable to open file\n");
        return;
    }
    fseek(historyFile, SEEK_END, 0);

    while (1) {
        char command[MAX_SIZE];
        int i;

        printf(">> ");
        if (fgets(command, MAX_SIZE, stdin) == NULL) {
            printf("Error occurred while reading input\n");
            break;
        }

        // Trim leading and trailing whitespaces
        char *tempCommand = command;
        while (isspace(*tempCommand))
            ++tempCommand;

        int len = strlen(tempCommand);
        while (len > 0 && isspace(tempCommand[len - 1])) {
            tempCommand[len - 1] = '\0';
            --len;
        }

        if (len == 0)
            continue;

        char tokens[MAX_SIZE][MAX_SIZE];
        int quotesFlag = 0;
        int tokenIndex = 0;
        int tokenLen = 0;

        for (i = 0; i < len; ++i) {
            if (isspace(tempCommand[i])) {
                if (quotesFlag) {
                    tokens[tokenIndex][tokenLen] = ' ';
                    ++tokenLen;
                } else if (tokenLen != 0) {
                    tokens[tokenIndex][tokenLen] = '\0';
                    ++tokenIndex;
                    tokenLen = 0;
                }
            } else if (tempCommand[i] == '\"') {
                if (tokenLen != 0) {
                    tokens[tokenIndex][tokenLen] = '\0';
                    ++tokenIndex;
                    tokenLen = 0;
                }
                quotesFlag = !quotesFlag;
            } else {
                tokens[tokenIndex][tokenLen] = tempCommand[i];
                ++tokenLen;
            }
        }

        if (tokenLen != 0) {
            tokens[tokenIndex][tokenLen] = '\0';
            ++tokenIndex;
        }

        if (tokenIndex == 0)
            continue;

        int notFound = 1;
        for (i = 0; i < commandsSize; ++i) {
            if (strcmp(tokens[0], commands[i]) == 0) {
                notFound = 0;
                break;
            }
        }

        if (notFound) {
            printf("Error: Not a valid command\n");
            continue;
        }

        if (strcmp(tokens[0], "cd") == 0) {
            if (tokenIndex > 2) {
                printf("Error: Excess of arguments\n");
            } else if (tokenIndex == 1 || (tokenIndex == 2 && strcmp(tokens[1], "~") == 0)) {
                if (chdir("/") == -1) {
                    printf("Error: Unable to open path\n");
                }
                if (chdir("home") == -1) {
                    printf("Error: Unable to open path\n");
                }
                char *user = getlogin();
                if (user == NULL) {
                    printf("Error: Unable to open path\n");
                }
                if (chdir(user) == -1) {
                    printf("Error: Unable to open path\n");
                }
            } else if (tokenIndex == 2 && strcmp(tokens[1], "--help") == 0) {
                strcpy(tokens[0], "cat");
                strcpy(tokens[1], absolutePath);
                strcat(tokens[1], "CD/help_cd.txt");
                runCat(tokens, tokenIndex);
            } else if (chdir(tokens[1]) == -1) {
                printf("Error: Invalid Path\n");
            }
        } else if (strcmp(tokens[0], "echo") == 0) {
            if (tokenIndex == 1) {
                printf("\n");
            } else if (strcmp(tokens[1], "-n") == 0) {
                for (i = 2; i < tokenIndex; ++i)
                    printf("%s ", tokens[i]);
            } else if (strcmp(tokens[1], "-E") == 0) {
                for (i = 2; i < tokenIndex; ++i)
                    printf("%s ", tokens[i]);
                printf("\n");
            } else if (strcmp(tokens[1], "-e") != 0) {
                for (i = 1; i < tokenIndex; ++i)
                    printf("%s ", tokens[i]);
                printf("\n");
            } else {
                printf("Error: Unrecognized flag\n");
            }
        } else if (strcmp(tokens[0], "history") == 0) {
            if (tokenIndex == 1) {
                fseek(historyFile, SEEK_SET, 0);
                char line[MAX_SIZE];
                int count = 0;
                while (fgets(line, sizeof(line), historyFile) != NULL) {
                    ++count;
                    printf("%5d %s", count, line);
                }
                fseek(historyFile, SEEK_END, 0);
            } else if (tokenIndex > 2) {
                printf("Error: Excess arguments\n");
            } else if (strcmp(tokens[1], "--help") == 0) {
                strcpy(tokens[0], "cat");
                strcpy(tokens[1], absolutePath);
                strcat(tokens[1], "HISTORY/help_history.txt");
                runCat(tokens, tokenIndex);
            } else if (strcmp(tokens[1], "-c") == 0) {
                fclose(historyFile);
                historyFile = fopen(fileName, "w");
                fclose(historyFile);
                historyFile = fopen(fileName, "a+");
                fseek(historyFile, SEEK_END, 0);
            } else {
                int j = 0;
                int flag = 1;
                while (tokens[1][j] != '\0') {
                    if (!isdigit(tokens[1][j])) {
                        flag = 0;
                        break;
                    }
                    ++j;
                }
                if (flag == 0) {
                    printf("Error: Invalid argument\n");
                } else {
                    int num = atoi(tokens[1]);
                    if (num == 0 || num < 0) {
                        printf("Error: Invalid argument\n");
                    } else {
                        char lines[MAX_SIZE][MAX_SIZE];

                        fseek(historyFile, SEEK_SET, 0);
                        char line[MAX_SIZE];
                        int count = 0;
                        while (fgets(line, sizeof(line), historyFile) != NULL) {
                            strcpy(lines[count], line);
                            ++count;
                        }
                        fseek(historyFile, SEEK_END, 0);
                        int min = (num < count) ? num : count;
                        for (i = count - min; i < count; ++i)
                            printf("%5d %s", i + 1, lines[i]);
                    }
                }
            }
        } else if (strcmp(tokens[0], "pwd") == 0) {
            if (tokenIndex > 2) {
                printf("Error: Excess arguments\n");
            } else if (tokenIndex == 1 || (tokenIndex == 2 && strcmp(tokens[1], "-P") == 0)) {
                char path[MAX_SIZE];
                if (getcwd(path, MAX_SIZE) == NULL) {
                    printf("Error: Unable to access current working directory\n");
                } else {
                    printf("%s\n", path);
                }
            } else if (tokenIndex == 2 && strcmp(tokens[1], "--help") == 0) {
                strcpy(tokens[0], "cat");
                strcpy(tokens[1], absolutePath);
                strcat(tokens[1], "PWD/help_pwd.txt");
                runCat(tokens, tokenIndex);
            } else {
                printf("Error: Unrecognized flag\n");
            }
        } else if (strcmp(tokens[0], "exit") == 0) {
            if (tokenIndex == 2 && strcmp(tokens[1], "--help") == 0) {
                strcpy(tokens[0], "cat");
                strcpy(tokens[1], absolutePath);
                strcat(tokens[1], "EXIT/help_exit.txt");
                runCat(tokens, tokenIndex);
            } else if (tokenIndex <= 2) {
                fclose(historyFile);
                exit(0);
            } else {
                printf("Error: Excess arguments\n");
            }
        } else if (strcmp(tokens[0], "ls") == 0) {
            char** modifiedTokens = (char**)calloc(tokenIndex + 1, sizeof(char*));
            char commandPath[MAX_SIZE];
            strcpy(commandPath, absolutePath);
            strcat(commandPath, "LS/LS");
            modifiedTokens[0] = commandPath;
            for (i = 1; i < tokenIndex; ++i)
                modifiedTokens[i] = tokens[i];
            modifiedTokens[tokenIndex] = (char*)NULL;
            ++tokenIndex;
            pid_t pid = fork();
            if (pid < 0) {
                printf("Failed to create process\n");
            } else if (pid == 0) {
                if (execv(modifiedTokens[0], modifiedTokens) == -1)
                    exit(-1);
                else
                    exit(0);
            } else {
                if (waitpid(pid, NULL, 0) == -1)
                    printf("Process didn't work\n");
            }
        } else if (strcmp(tokens[0], "cat") == 0) {
            char** modifiedTokens = (char**)calloc(tokenIndex + 1, sizeof(char*));
            char commandPath[MAX_SIZE];
            strcpy(commandPath, absolutePath);
            strcat(commandPath, "CAT/CAT");
            modifiedTokens[0] = commandPath;
            for (i = 1; i < tokenIndex; ++i)
                modifiedTokens[i] = tokens[i];
            modifiedTokens[tokenIndex] = (char*)NULL;
            ++tokenIndex;
            pid_t pid = fork();
            if (pid < 0) {
                printf("Failed to create process\n");
            } else if (pid == 0) {
                if (execv(modifiedTokens[0], modifiedTokens) == -1)
                    exit(-1);
                else
                    exit(0);
            } else {
                if (waitpid(pid, NULL, 0) == -1)
                    printf("Process didn't work\n");
            }
        } else if (strcmp(tokens[0], "date") == 0) {
            char** modifiedTokens = (char**)calloc(tokenIndex + 1, sizeof(char*));
            char commandPath[MAX_SIZE];
            strcpy(commandPath, absolutePath);
            strcat(commandPath, "DATE/DATE");
            modifiedTokens[0] = commandPath;
            for (i = 1; i < tokenIndex; ++i)
                modifiedTokens[i] = tokens[i];
            modifiedTokens[tokenIndex] = (char*)NULL;
            ++tokenIndex;
            pid_t pid = fork();
            if (pid < 0) {
                printf("Failed to create process\n");
            } else if (pid == 0) {
                if (execv(modifiedTokens[0], modifiedTokens) == -1)
                    exit(-1);
                else
                    exit(0);
            } else {
                if (waitpid(pid, NULL, 0) == -1)
                    printf("Process didn't work\n");
            }
        } else if (strcmp(tokens[0], "rm") == 0) {
            char** modifiedTokens = (char**)calloc(tokenIndex + 1, sizeof(char*));
            char commandPath[MAX_SIZE];
            strcpy(commandPath, absolutePath);
            strcat(commandPath, "RM/RM");
            modifiedTokens[0] = commandPath;
            for (i = 1; i < tokenIndex; ++i)
                modifiedTokens[i] = tokens[i];
            modifiedTokens[tokenIndex] = (char*)NULL;
            ++tokenIndex;
            pid_t pid = fork();
            if (pid < 0) {
                printf("Failed to create process\n");
            } else if (pid == 0) {
                if (execv(modifiedTokens[0], modifiedTokens) == -1)
                    exit(-1);
                else
                    exit(0);
            } else {
                if (waitpid(pid, NULL, 0) == -1)
                    printf("Process didn't work\n");
            }
        } else if (strcmp(tokens[0], "mkdir") == 0) {
            char** modifiedTokens = (char**)calloc(tokenIndex + 1, sizeof(char*));
            char commandPath[MAX_SIZE];
            strcpy(commandPath, absolutePath);
            strcat(commandPath, "MKDIR/MKDIR");
            modifiedTokens[0] = commandPath;
            for (i = 1; i < tokenIndex; ++i)
                modifiedTokens[i] = tokens[i];
            modifiedTokens[tokenIndex] = (char*)NULL;
            ++tokenIndex;
            pid_t pid = fork();
            if (pid < 0) {
                printf("Failed to create process\n");
            } else if (pid == 0) {
                if (execv(modifiedTokens[0], modifiedTokens) == -1)
                    exit(-1);
                else
                    exit(0);
            } else {
                if (waitpid(pid, NULL, 0) == -1)
                    printf("Process didn't work\n");
            }
        }
    }

    fclose(historyFile);
}

int main() {
    execute_command(NULL, 0);
    return 0;
}
