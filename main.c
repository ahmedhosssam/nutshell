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
        printf(">> ");
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
        token = strtok(input, " ");
        cmd = token;
        int token_cnt = 0;
        args[token_cnt] = token;

        while (token != NULL) {
            args[token_cnt] = token;
            token = strtok(NULL, " ");
            token_cnt++;
        }
        char tmp[1024] = "/bin/";
        strcat(tmp, args[0]);
        args[0] = tmp;
        //strcpy(args[0], tmp);

        for(int i = 0 ;i < token_cnt; i++) {
            //printf("%s\n", args[i]);
            // /usr/bin
            // "/usr/bin" + "/" + args[0]
        }

        int rc = fork();
        if (rc == 0) {
            if (execv(args[0], args) == -1) {
                printf("%s: this command is not found\n", cmd);
            }
        } else {
            rc = wait(NULL);
        }
    }
    return 0;
}
