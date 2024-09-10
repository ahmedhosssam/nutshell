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
