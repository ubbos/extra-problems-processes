/* Problem A — Exec Pipeline
 * Usage: ./solution <filename> <word>
 * Creates two children connected by a pipe:
 *   child1: grep <word> <filename>  (writes to pipe)
 *   child2: wc -l                   (reads from pipe)
 * Parent reads the count from child2 and prints it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <word>\n", argv[0]);
        exit(1);
    }

    char *filename = argv[1];
    char *word     = argv[2];

    /* pipe_gw: grep -> wc */
    int pipe_gw[2];
    if (pipe(pipe_gw) == -1) { perror("pipe"); exit(1); }

    /* pipe_wp: wc -> parent */
    int pipe_wp[2];
    if (pipe(pipe_wp) == -1) { perror("pipe"); exit(1); }

    /* Child 1: grep <word> <filename> — writes to pipe_gw */
    pid_t pid1 = fork();
    if (pid1 == -1) { perror("fork"); exit(1); }

    if (pid1 == 0) {
        close(pipe_gw[0]);
        close(pipe_wp[0]); close(pipe_wp[1]);
        dup2(pipe_gw[1], STDOUT_FILENO);
        close(pipe_gw[1]);
        execlp("grep", "grep", word, filename, (char *)NULL);
        perror("execlp grep");
        exit(1);
    }

    /* Child 2: wc -l — reads from pipe_gw, writes to pipe_wp */
    pid_t pid2 = fork();
    if (pid2 == -1) { perror("fork"); exit(1); }

    if (pid2 == 0) {
        close(pipe_gw[1]);
        close(pipe_wp[0]);
        dup2(pipe_gw[0], STDIN_FILENO);
        close(pipe_gw[0]);
        dup2(pipe_wp[1], STDOUT_FILENO);
        close(pipe_wp[1]);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("execlp wc");
        exit(1);
    }

    /* Parent: close pipe ends we don't use */
    close(pipe_gw[0]);
    close(pipe_gw[1]);
    close(pipe_wp[1]);

    /* Read count from wc */
    char buf[64];
    ssize_t n = read(pipe_wp[0], buf, sizeof(buf) - 1);
    close(pipe_wp[0]);

    if (n > 0) {
        buf[n] = '\0';
        /* wc -l output may have leading spaces */
        char *p = buf;
        while (*p == ' ' || *p == '\t') p++;
        printf("Matching lines: %s", p);
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
