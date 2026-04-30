/* Problem C — Fan-out Aggregation
 * Usage: ./solution <K>
 *   Reads N integers from stdin (first line: N, then N integers).
 *   Splits them into K chunks, sends each to a child via a pipe.
 *   Each child finds the max of its chunk and returns it.
 *   Parent prints the global maximum.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <K>\n", argv[0]);
        exit(1);
    }

    int K = atoi(argv[1]);
    if (K < 1) { fprintf(stderr, "K must be >= 1\n"); exit(1); }

    int N;
    printf("Enter N: ");
    fflush(stdout);
    scanf("%d", &N);
    if (N < 1) { fprintf(stderr, "N must be >= 1\n"); exit(1); }

    int *nums = malloc(N * sizeof(int));
    if (!nums) { perror("malloc"); exit(1); }

    printf("Enter %d integers:\n", N);
    for (int i = 0; i < N; i++) scanf("%d", &nums[i]);

    /* parent->child pipes (send chunk) */
    int (*pc)[2] = malloc(K * sizeof(*pc));
    /* child->parent pipes (receive max) */
    int (*cp)[2] = malloc(K * sizeof(*cp));
    if (!pc || !cp) { perror("malloc"); exit(1); }

    for (int i = 0; i < K; i++) {
        if (pipe(pc[i]) == -1 || pipe(cp[i]) == -1) { perror("pipe"); exit(1); }
    }

    int base  = N / K;
    int extra = N % K;
    int offset = 0;

    /* Fork all children first — no pipes are closed until all children exist,
     * so every child inherits a fully-initialised, stable FD table. */
    for (int i = 0; i < K; i++) {
        int chunk_size = base + (i < extra ? 1 : 0);

        pid_t pid = fork();
        if (pid == -1) { perror("fork"); exit(1); }

        if (pid == 0) {
            /* Close all pipes except pc[i][0] (read) and cp[i][1] (write) */
            for (int j = 0; j < K; j++) {
                if (j == i) {
                    close(pc[j][1]);
                    close(cp[j][0]);
                } else {
                    close(pc[j][0]); close(pc[j][1]);
                    close(cp[j][0]); close(cp[j][1]);
                }
            }

            /* Read chunk */
            int *chunk = malloc(chunk_size * sizeof(int));
            if (!chunk) { perror("malloc"); exit(1); }

            printf("[child %d] waiting to receive %d integers\n", i, chunk_size);
            fflush(stdout);
            read(pc[i][0], chunk, chunk_size * sizeof(int));
            close(pc[i][0]);
            printf("[child %d] received chunk of size %d\n", i, chunk_size);
            fflush(stdout);

            int max = chunk[0];
            for (int j = 1; j < chunk_size; j++)
                if (chunk[j] > max) max = chunk[j];

            printf("[child %d] sending max = %d\n", i, max);
            fflush(stdout);
            write(cp[i][1], &max, sizeof(int));
            close(cp[i][1]);

            free(chunk);
            free(nums); free(pc); free(cp);
            exit(0);
        }
    }

    /* Parent: close child-side pipe ends and send chunks */
    for (int i = 0; i < K; i++) {
        int chunk_size = base + (i < extra ? 1 : 0);

        close(pc[i][0]);
        close(cp[i][1]);

        printf("[parent] sending %d integers to child %d\n", chunk_size, i);
        fflush(stdout);
        write(pc[i][1], nums + offset, chunk_size * sizeof(int));
        close(pc[i][1]);

        offset += chunk_size;
    }

    /* Collect max from each child */
    int global_max;
    int first = 1;
    for (int i = 0; i < K; i++) {
        int child_max;
        read(cp[i][0], &child_max, sizeof(int));
        close(cp[i][0]);
        printf("[parent] received max %d from child %d\n", child_max, i);
        fflush(stdout);
        if (first || child_max > global_max) { global_max = child_max; first = 0; }
    }

    printf("Global maximum: %d\n", global_max);

    for (int i = 0; i < K; i++) wait(NULL);

    free(nums); free(pc); free(cp);
    return 0;
}
