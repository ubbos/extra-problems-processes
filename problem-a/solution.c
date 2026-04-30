/* Problem A — Exec Pipeline
 * Usage: ./solution <filename> <word>
 * Creates two children connected by a pipe:
 *   child1: grep <word> <filename>  (writes to pipe)
 *   child2: wc -l                   (reads from pipe)
 * Parent waits for both children.
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

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    /* Child 1: grep <word> <filename> — writes to pipe write-end */
    pid_t pid1 = fork();
    if (pid1 == -1) { perror("fork"); exit(1); }

    if (pid1 == 0) {
        close(pipefd[0]);               /* close read-end */
        dup2(pipefd[1], STDOUT_FILENO); /* redirect stdout to pipe */
        close(pipefd[1]);
        execlp("grep", "grep", word, filename, (char *)NULL);
        perror("execlp grep");
        exit(1);
    }

    /* Child 2: wc -l — reads from pipe read-end */
    pid_t pid2 = fork();
    if (pid2 == -1) { perror("fork"); exit(1); }

    if (pid2 == 0) {
        close(pipefd[1]);               /* close write-end */
        dup2(pipefd[0], STDIN_FILENO);  /* redirect stdin from pipe */
        close(pipefd[0]);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("execlp wc");
        exit(1);
    }

    /* Parent: close both ends and wait */
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
