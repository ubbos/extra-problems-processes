/* Problem B — Parallel File Processing
 * Usage: ./solution file1 file2 ... fileN
 * For each file, spawns a child that runs `wc -w` and sends output
 * back to the parent via a dedicated pipe. Parent prints per-file
 * word counts and the grand total.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 [file2 ...]\n", argv[0]);
        exit(1);
    }

    int n = argc - 1;

    /* Allocate one pipe per child */
    int (*pipes)[2] = malloc(n * sizeof(*pipes));
    if (!pipes) { perror("malloc"); exit(1); }

    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) == -1) { perror("pipe"); exit(1); }

        pid_t pid = fork();
        if (pid == -1) { perror("fork"); exit(1); }

        if (pid == 0) {
            /* Only close pipes that have actually been created (j <= i).
             * Pipes for j > i do not exist yet — their slots are uninitialised.
             * The parent has not yet closed any write-ends (done after all forks),
             * so all FD numbers are stable and unique. */
            for (int j = 0; j < i; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            close(pipes[i][0]); /* close our own read-end */
            dup2(pipes[i][1], STDOUT_FILENO);
            close(pipes[i][1]);
            execlp("wc", "wc", "-w", argv[i + 1], (char *)NULL);
            perror("execlp wc");
            exit(1);
        }
        /* Do NOT close write-ends here — doing so causes FD reuse before later
         * children fork, leading to double-closes in their cleanup loops. */
    }

    /* Parent: now that all children are forked, close all write-ends. */
    for (int i = 0; i < n; i++)
        close(pipes[i][1]);

    /* Parent collects results */
    long total = 0;
    char buf[256];

    for (int i = 0; i < n; i++) {
        ssize_t bytes = read(pipes[i][0], buf, sizeof(buf) - 1);
        close(pipes[i][0]);

        if (bytes > 0) {
            buf[bytes] = '\0';
            /* wc -w output: "  <count> <filename>\n" or just "<count>\n" */
            long count = atol(buf);
            printf("%s: %ld words\n", argv[i + 1], count);
            total += count;
        }
    }

    printf("Total: %ld words\n", total);

    /* Wait for all children */
    for (int i = 0; i < n; i++) wait(NULL);

    free(pipes);
    return 0;
}
