#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char *paths[1024] = {};

char *name = "PATH";
char *pathenv = NULL;
int path_cnt = 0;

void runcmd(char *line) {
    char *args[1024] = {};
    char *token;
    char *cmd; // the command itself
    char *file = NULL;
    int token_cnt = 0;
    int redirection_found = 0;

    while ((token = strsep(&line, " ")) != NULL) {
        if (redirection_found == 1) {
            file = token;
            break;
        }
        if (strcmp(token, ">") == 0) {
            redirection_found = 1;
            continue;
        }
        args[token_cnt] = token;
        token_cnt++;
    }

    cmd = args[0];

    // Built-in commands
    char *exitstr = "exit";
    char *cdstr = "cd";

    if (strcmp(cmd, exitstr) == 0) {
        exit(0);
    }

    if (strcmp(cmd, cdstr) == 0) {
        if (token_cnt == 1 || token_cnt > 2) {
            printf("cd takes only one argument\nexample: `cd ..` or `cd src`\n");
            return;
        }
        char directory[1024] = "./";
        strcat(directory, args[1]);
        if(chdir(directory) != 0) {
            printf("error: %s is not a directory\n", args[1]);
        }
        return;
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
        if (redirection_found) {
            int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
        }
        if (execv(cmd_dir, args) != 0) {
            printf("%s: command doesn't exists\n", cmd);
        }
    } else {
        rc = wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    pathenv = getenv(name);
    char *path_token = strtok(pathenv, ":");

    while (path_token != NULL) {
        paths[path_cnt] = path_token;
        path_token = strtok(NULL, ":");
        path_cnt++;
    }

    if (argc > 2) {
        printf("usage: ./nutshell or ./nutshell <file>\n");
        exit(1);
    }

    if (argc == 2) {
        FILE *file;
        //char *line = malloc(sizeof(char)*1024);
        char line[1024];
        char *stringp;
        file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("Error opening file");
            return -1;
        }
        while (fgets(line, sizeof(line), file)) {
            stringp = line;
            runcmd(stringp);
        }
        return 0;
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
        runcmd(input);
    }
    return 0;
}
