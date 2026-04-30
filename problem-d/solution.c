/* Problem D — Pipeline of Three
 * Usage: ./solution <filename>
 *
 * parent  --pipe1-->  child1 (tolower)  --pipe2-->  child2 (count vowels)
 *                                                        |
 *                                               pipe3 (int count)
 *                                                        |
 *                                                    parent
 *
 * Parent reads the file line by line and writes lines into pipe1.
 * Child1 lowercases each line and writes it to pipe2.
 * Child2 counts vowels across all lines and writes the total to pipe3.
 * Parent reads the total from pipe3 and prints it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFSIZE 4096

int is_vowel(char c) {
    c = tolower((unsigned char)c);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    int pipe1[2], pipe2[2], pipe3[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        perror("pipe"); exit(1);
    }

    /* ---- Child 1: lowercase ---- */
    pid_t pid1 = fork();
    if (pid1 == -1) { perror("fork"); exit(1); }

    if (pid1 == 0) {
        /* Read from pipe1, write to pipe2 */
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe3[0]); close(pipe3[1]);

        char buf[BUFSIZE];
        ssize_t n;
        while ((n = read(pipe1[0], buf, sizeof(buf))) > 0) {
            printf("[child1] received %zd bytes, lowercasing\n", n);
            fflush(stdout);
            for (ssize_t i = 0; i < n; i++)
                buf[i] = tolower((unsigned char)buf[i]);
            write(pipe2[1], buf, n);
            printf("[child1] forwarded %zd bytes to child2\n", n);
            fflush(stdout);
        }
        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    /* ---- Child 2: count vowels ---- */
    pid_t pid2 = fork();
    if (pid2 == -1) { perror("fork"); exit(1); }

    if (pid2 == 0) {
        /* Read from pipe2, write count to pipe3 */
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[1]);
        close(pipe3[0]);

        char buf[BUFSIZE];
        ssize_t n;
        long count = 0;
        while ((n = read(pipe2[0], buf, sizeof(buf))) > 0) {
            printf("[child2] received %zd bytes\n", n);
            fflush(stdout);
            for (ssize_t i = 0; i < n; i++)
                if (is_vowel(buf[i])) count++;
        }
        close(pipe2[0]);

        printf("[child2] total vowels counted: %ld, sending to parent\n", count);
        fflush(stdout);
        write(pipe3[1], &count, sizeof(count));
        close(pipe3[1]);
        exit(0);
    }

    /* ---- Parent: read file, send to child1, receive result ---- */
    close(pipe1[0]);
    close(pipe2[0]); close(pipe2[1]);
    close(pipe3[1]);

    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("fopen"); exit(1); }

    char buf[BUFSIZE];
    size_t bytes;
    while ((bytes = fread(buf, 1, sizeof(buf), f)) > 0) {
        printf("[parent] sending %zu bytes to child1\n", bytes);
        fflush(stdout);
        write(pipe1[1], buf, bytes);
    }
    fclose(f);
    close(pipe1[1]); /* signal EOF to child1 */

    long total = 0;
    read(pipe3[0], &total, sizeof(total));
    close(pipe3[0]);

    printf("[parent] received vowel count from child2\n");
    printf("Total vowels: %ld\n", total);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
