#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    char *paths[1024] = {};

    char *name = "PATH";
    char *pathenv = getenv(name);
    int path_cnt = 0;

    char *path_token = strtok(pathenv, ":");

    while (path_token != NULL) {
        paths[path_cnt] = path_token;
        path_token = strtok(NULL, ":");
        path_cnt++;
    }

    while (1) {
        printf("\e[0;32m>> \033[000m");

        char *input = NULL;
        size_t bufsize = 0; // to make getline() allocate memory as needed
        ssize_t chars = 0;

        chars = getline(&input, &bufsize, stdin);

        input[strcspn(input, "\n")] = 0;
        if (chars == EOF) {
            exit(0);
        }

        char *args[1024] = {};

        char *token;
        char *cmd; // the command itself
        
        int token_cnt = 0;
        
        while ((token = strsep(&input, " ")) != NULL) {
            args[token_cnt] = token;
            token_cnt++;
        }

        cmd = args[0];

        char *exitstr = "exit";
        char *cdstr = "cd";

        if (strcmp(cmd, exitstr) == 0) {
            exit(0);
        }

        if (strcmp(cmd, cdstr) == 0) {
            if (token_cnt == 1 || token_cnt > 2) {
                printf("cd takes only one argument\nexample: `cd ..` or `cd src`\n");
                continue;
            }
            char directory[1024] = "./";
            strcat(directory, args[1]);
            if(chdir(directory) != 0) {
                printf("error: %s is not a directory\n", args[1]);
            }
            continue;
        }

        char cmd_dir[1024];
        for(int i = 0; i < path_cnt; i++) {
            sprintf(cmd_dir, "%s/%s", paths[i], cmd);
            if (access(cmd_dir, F_OK) == 0) {
                break;
            }
        }

        int rc = fork();
        if (rc == 0) {
            if (execv(cmd_dir, args) != 0) {
                printf("%s: command doesn't exists\n", cmd);
            }
        } else {
            rc = wait(NULL);
        }
    }
    return 0;
}
