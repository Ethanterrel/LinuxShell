

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>


#define CD "cd"
#define CLR "clr"
#define DR "dir"
#define ENVIRON "environ"
#define ECHO "echo"
#define PATH "path"
#define ECHO "echo"
#define HELP "help"
#define PAUSE "pause"
#define QUIT "quit"

//global variables
char path[50][50];
char pwd[100];
char shellpath[100];
int pathsize = 1;

typedef struct{
    int builtin;
    char args[50][20];
    int argcount; int out;
    int out1; int in;
    int pipe; int backexec;
}command;


void error(); void initstruct(command *);
void parse(char *, command *);void handlebuiltin(command);
void cd(command);void dir(command);
void environ(command);void changepath(command);
void echo(command);void help(command);
void pauseshell(command);



int main(int argc, char *argv[]){

    strcpy(path[0], "/bin");
    strcpy(path[1], "\0");
    getcwd(pwd, 100);
    getcwd(shellpath, 100);
    command cmd;
    initstruct(&cmd);

    // batchmode
    int batchmode;
    FILE *fp;
    if (argc == 2){
        batchmode = 1;   fp = fopen(argv[1], "r");
        if (fp == NULL){
            printf("file cannot be opened");
            exit(1);
        }
    }
    else if (argc > 2){
        printf("too many arguments");
        exit(1);
    }
    else batchmode = 0;
    size_t size = 100; char *line = (char *)malloc(size*sizeof(char)); // MEM for user input

    while(1){
        printf("myshell> ");

        if (batchmode){
            if (getline(&line, &size, fp) == -1){
                break;
            }
        }
        else getline(&line, &size, stdin);

        if (strcmp(line, "\n") != 0){
            parse(line, &cmd);
            if (cmd.builtin){
                handlebuiltin(cmd);
            }
        }
        initstruct(&cmd);
    }
    fclose(fp);
    free(line);
}

void error(){
    char *error_msg = "an error has occured\n";
    write(STDERR_FILENO, error_msg, strlen(error_msg));
}

void initstruct(command *cmd){ //sets initial numbers at 0
    cmd->builtin = 0; cmd->argcount = 0;
    cmd->out = 0; cmd->out1 = 0; cmd->in = 0;
    cmd->pipe = 0;    cmd->backexec = 0;
}

void parse(char *line, command *cmd){

    const char *delims = " \n\t";
    char *buffer = strtok(line, delims);

    if (strcmp(buffer, CD) == 0){  cmd->builtin = 1;
    }
    else if (strcmp(buffer, CLR) == 0){  cmd->builtin = 1;
    }
    else if (strcmp(buffer, DR) == 0){  cmd->builtin = 1;
    }
    else if (strcmp(buffer, ENVIRON) == 0){   cmd->builtin = 1;
    }
    else if (strcmp(buffer, PATH) == 0){  cmd->builtin = 1;
    }
    else if (strcmp(buffer, ECHO) == 0){ cmd->builtin = 1;
    }
    else if (strcmp(buffer, HELP) == 0){   cmd->builtin = 1;
    }
    else if (strcmp(buffer, PAUSE) == 0){   cmd->builtin = 1;
    }
    else if (strcmp(buffer, QUIT) == 0){  exit(0);
    }
    else{   cmd->builtin = 0;
    }

    strcpy(cmd->args[0], buffer); // array for parsed info

    int i = 1;

    while((buffer = strtok(NULL, delims)) != NULL){

        switch (buffer[0]){
            case '>':
                if (buffer[0] == '>'){
                    cmd->out1 = i;
                    if (buffer[1] != '\0'){ cmd->builtin = -1;
                    }
                }
                else{
                    cmd->out = i;
                    if (buffer[2] != '\0'){ cmd->builtin = -1;
                    }
                }
                break;
            case '<':
                cmd->in = i;
                if (buffer[1] != '\0'){ cmd->builtin = -1;
                    return;
                }
                break;
            case '|':
                cmd->pipe++;
                strncpy(cmd->args[i++], buffer, 50);
                cmd->argcount++;
                if (buffer[1] != '\0'){ cmd->builtin = -1;
                    return;
                }
                break;
            case '&':
                cmd->backexec = 1;
                if (buffer[1] != '\0'){ cmd->builtin = -1;
                    return;
                }
                break;case '\t':
                break; case '\n':
                break;
            default:
                strncpy(cmd->args[i++], buffer, 50); cmd->argcount++;
        }
    }
    free(buffer);
}

void handlebuiltin(command cmd){

    if (strcmp(cmd.args[0], CD) == 0){
        if (cmd.argcount > 1){
            error();
        }
        else{
            cd(cmd);
        }
    }
    else if (strcmp(cmd.args[0], CLR) == 0){

        if (cmd.argcount > 0){
            error();
        }
        else{
            printf("\033[H\033[J");
        }
        return;
    }
    else if (strcmp(cmd.args[0], DR) == 0){ dir(cmd);
    }
    else if (strcmp(cmd.args[0], ENVIRON) == 0){ environ(cmd);
    }
    else if (strcmp(cmd.args[0], PATH) == 0){ changepath(cmd);
    }
    else if (strcmp(cmd.args[0], ECHO) == 0){ echo(cmd);
    }
    else if (strcmp(cmd.args[0], HELP) == 0){ help(cmd);
    }
    else if (strcmp(cmd.args[0], PAUSE) == 0){ pauseshell(cmd);
    }
    else if (strcmp(cmd.args[0], QUIT) == 0){
        if (cmd.argcount > 0){
            error();
        }
        else{
            exit(0);
        }
    }
    else{
        error();
    }
}


void cd(command cmd){

    if (cmd.argcount == 0){
        printf("%s\n", pwd);
    }
    else {
        if (chdir(cmd.args[1]) == -1){
            error();
        }
        else {
            getcwd(pwd, 100);
            printf("%s\n", pwd);
        }
    }
}


void dir(command cmd){

    DIR *dir;
    struct dirent *read;
    if (cmd.argcount > 2){
        error();
        return;
    }

    if (cmd.argcount == 0){
        dir = opendir(pwd);
    }
    else if ((cmd.argcount == 1)&&((cmd.out)||(cmd.out1))){
        dir = opendir(pwd);
    }
    else{

        dir = opendir(cmd.args[1]);
        if (dir == NULL){
            error();
            return;
        }
    }

    read = readdir(dir);

    if (cmd.out){
        FILE *file = fopen(cmd.args[cmd.out], "w");

        while (read != NULL){
            fprintf(file, "%s\n", read->d_name);
            read = readdir(dir);
        }
        fclose(file);
    }
    else if (cmd.out1){
        FILE *file = fopen(cmd.args[cmd.out1], "a");
        while (read != NULL){
            fprintf(file, "%s\n", read->d_name);
            read = readdir(dir);
        }
        fclose(file);
    }
    else {
        while (read != NULL){
            printf("%s\n", read->d_name);
            read = readdir(dir);
        }
    }
    closedir(dir);
}


void environ(command cmd){

    if (cmd.argcount > 1){
        error();
    }
    else if (cmd.argcount == 1){
        FILE *fp;

        if (cmd.out){ fp = fopen(cmd.args[cmd.out], "w");
        }
        else{ fp = fopen(cmd.args[cmd.out1], "a");
        }

        if (fp == NULL){
            error();
        }
        fprintf(fp, "PATH=%s", path[0]);
        int i = 1;
        while(i < pathsize){
            fprintf(fp, ":%s", path[i++]);
        }
        fprintf(fp, "\nshell=%s/myshell\n", shellpath); fprintf(fp, "PWD=%s\n", pwd); fprintf(fp, "USER=%s\n", getenv("USER")); fprintf(fp, "HOME=%s\n", getenv("HOME"));
        fclose(fp);
    }
    else{

        printf("PATH=%s", path[0]);
        int i = 1;
        while(i < pathsize){
            printf(":%s", path[i++]);
        }
        printf("\nshell=%s/myshell\n", shellpath); printf("PWD=%s\n", pwd); printf("USER=%s\n", getenv("USER")); printf("HOME=%s\n", getenv("HOME"));
    }
}



void changepath(command cmd){
    if (cmd.argcount == 0){
        strcpy(path[0], "\0");
        strcpy(path[1], "\0");
        return;
    }

    int i = 1;

    while (i <= cmd.argcount){
        DIR *dir = opendir(cmd.args[i]);
        if (dir == NULL){
            error();
            return;
        }
        strcpy(path[i-1], cmd.args[i]);
        i++;
        closedir(dir);
    }
    strcpy(path[i-1], "\0");
}



void echo(command cmd){
    if (cmd.argcount == 0 || cmd.argcount > 2){
        error();
        return;
    }

    if (cmd.out || cmd.out1){
        FILE *fp;
        if (cmd.out){ fp = fopen(cmd.args[cmd.out], "w");
        }
        else{ fp = fopen(cmd.args[cmd.out1], "a");
        }

        if (fp == NULL){
            error(); return;
        }
        fprintf(fp, "%s\n", cmd.args[1]);
        fclose(fp);
    }
    else {
        printf("%s\n", cmd.args[1]);
    }
}


void help(command cmd){

    if (cmd.argcount > 1){
        error(); return;
    }

    char *line = NULL;
    size_t size = 100;
    FILE *manual = fopen("readme", "r");
    if (manual == NULL){
        error(); return;
    }
    getline(&line, &size, manual);

    if (cmd.out || cmd.out1){
        FILE *fp;
        if (cmd.out){ fp = fopen(cmd.args[cmd.out], "w");
        }
        else{ fp = fopen(cmd.args[cmd.out1], "a");
        }

        if (fp == NULL){
            error();
            return;
        }
        fprintf(fp, "%s", line);
        while(getline(&line, &size, manual) != -1){ fprintf(fp, "%s", line);
        }
        fclose(fp);
    }
    else{
        printf("%s", line);
        while(getline(&line, &size, manual) != -1){ printf("%s", line);
        }
    }
    fclose(manual);
}



void pauseshell(command cmd){
    if (cmd.argcount){
        error(); return;
    }
    printf("press enter to continue\n");
    char c;
    int i;  do { fflush(stdin); i = 0;
        while ((c = getchar()) != '\n'){ i++;
        }
    } while (i);
}

void handlepipe(char args[50][50], int argc, int n){

    char arguments[n+1][50][50];

    int i = 0, j = 0, k = 0; index = 0;
    while (index <= argc){
        if (strcmp("|", args[i]) == 0){
        if (strcmp("\0", args[i+1]) == 0){
           error(); return;
            }
            i++; j++; k = 0;
        }
        else{
            strncpy(arguments[j][k], args[i], 50);
            i++; k++;
        }
        index++;
    }
    i = 0;int nextread;pid_t pid;int fd[2];
    pipe(fd); pid = fork();

    if (pid == -1){
        error();
        return;
    }

    if (pid == 0){
        close(1);
        if (dup2(fd[1], 1) == -1){
            error();
            return;
        }
        close(fd[0]); close(fd[1]);
        if (execv(arguments[0][0], NULL) == -1){
            error();exit(1);
        }
    }

    while (i < n){
        nextread = fd[0];
        pipe(fd); pid = fork();

        if (pid == -1){
            error(); return;
        }
        if (pid == 0){
            close(0);
            if (dup2(nextread, 0) == -1){
                error(); return;
            }
            close(fd[0]);

            if (dup2(fd[1], 1) == -1){
                error(); return;
            }
            close(fd[1]);

            if (execv(arguments[i][0], NULL) == -1){
                error(); exit(1);
            }
        }
        i++;
    }