/* Problem E — Fan-in Collection
 * Usage: ./solution <N> <M>
 *   N children each generate M random ints [1..1000].
 *   Each child sends its numbers to the parent via a dedicated pipe.
 *   Parent computes and prints sum and average of all N*M numbers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <N> <M>\n", argv[0]);
        exit(1);
    }

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    if (N < 1 || M < 1) { fprintf(stderr, "N and M must be >= 1\n"); exit(1); }

    /* One pipe per child */
    int (*pipes)[2] = malloc(N * sizeof(*pipes));
    if (!pipes) { perror("malloc"); exit(1); }

    for (int i = 0; i < N; i++) {
        if (pipe(pipes[i]) == -1) { perror("pipe"); exit(1); }

        pid_t pid = fork();
        if (pid == -1) { perror("fork"); exit(1); }

        if (pid == 0) {
            /* Close all pipes except this child's write-end */
            for (int j = 0; j < N; j++) {
                if (j == i) {
                    close(pipes[j][0]);
                } else {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }

            srand(time(NULL) ^ (getpid() << 4));

            for (int k = 0; k < M; k++) {
                int val = (rand() % 1000) + 1;
                printf("[child %d] sending value %d (%d/%d)\n", i, val, k + 1, M);
                fflush(stdout);
                write(pipes[i][1], &val, sizeof(int));
            }
            close(pipes[i][1]);

            free(pipes);
            exit(0);
        }

        /* Parent: close write-end of this child's pipe */
        close(pipes[i][1]);
    }

    /* Parent reads from all pipes */
    long sum = 0;
    long total = (long)N * M;

    for (int i = 0; i < N; i++) {
        for (int k = 0; k < M; k++) {
            int val;
            read(pipes[i][0], &val, sizeof(int));
            printf("[parent] received %d from child %d (%d/%d)\n", val, i, k + 1, M);
            fflush(stdout);
            sum += val;
        }
        close(pipes[i][0]);
    }

    printf("Sum: %ld\n", sum);
    printf("Average: %.2f\n", (double)sum / total);

    for (int i = 0; i < N; i++) wait(NULL);

    free(pipes);
    return 0;
}
