#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // from parent to child, parent write, child read
    int pp2c[2];
    // from child to parent, child write, parent read
    int pc2p[2];

    pipe(pp2c);
    pipe(pc2p);

    switch (fork()) {
    // fork failed
    case -1:
        break;
    // child
    case 0:
        // connect pp2c to stdin
        close(pp2c[1]);
        dup2(pp2c[0], STDIN_FILENO);
        close(pp2c[0]);
        // connect pc2p to stdout
        close(pc2p[0]);
        dup2(pc2p[1], STDOUT_FILENO);
        close(pc2p[1]);

        // exec "rev" to reverse lines
        execlp("rev", "rev", (char*)NULL);
        // exit with code 127 if exec fails
        perror("exec failed");
        exit(127);
        break;
    // parent
    default:
        // close unecessary pipes
        close(pp2c[0]);
        close(pc2p[1]);
        // open pipes as stream
        FILE* out = fdopen(pp2c[1], "w");
        FILE* in = fdopen(pc2p[0], "r");

        char word[1024];
        // redirect input to child process
        while (scanf("%s", word) != EOF) {
            fprintf(out, "%s\n", word);
        }
        fclose(out);
        // read child process output
        while (fscanf(in, "%s", word) != EOF) {
            printf("%s\n", word);
        }
        fclose(in);

        // call wait on exited child
        wait(NULL);
        break;
    }

    return 0;
}
