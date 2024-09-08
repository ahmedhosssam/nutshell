#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    while (1) {
        printf("> ");
        char *input = NULL;
        size_t bufsize = 0; // to make getline() allocate memory as needed
        ssize_t chars = 0;

        chars = getline(&input, &bufsize, stdin);

        if (chars == -1) {
            perror("getline");
        }
        input[strcspn(input, "\n")] = 0;

        char *args[1024] = {};

        char *token;
        token = strtok(input, " ");
        int token_cnt = 0;
        args[token_cnt] = token;
        token_cnt++;

        while (token != NULL) {

            args[token_cnt] = token;
            token = strtok(NULL, " ");
            token_cnt++;
        }

        int rc = fork();
        if (rc == 0) {
            execv(args[0], args);
        } else {
            rc = wait(NULL);
        }
    }
    return 0;
}
