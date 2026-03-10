#include <unistd.h>
#include <pty.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>
#define small_delay() usleep(100000)
#define delay() usleep(1000000)
// generated on my system using:
// echo | bash --rcfile <(echo "PS1='$PS1'") -i 2>&1 | head -n1 | sed -n l | sed 's/\$$//'
#define PS1 "\033[01;32mroot\033[00m@\033[01;34m/root/snake-vid\033[00m$ "

int fd;

void _shell(const char *command) {
    fflush(stdout);
    delay();
    for (; *command; command++) {
        printf("%c", *command);
        fflush(stdout);
        small_delay();
    }
    delay();
    puts("");
}

int shell(const char *command) {
    _shell(command);
    int status = system(command);
    printf(PS1);
    return status || !WIFEXITED(status) || WEXITSTATUS(status);
}

double now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

void write_halt(const char *w, double seconds) {
    write(fd, w, strlen(w));
    char c;
    double end = now() + seconds;
    while (now() < end) write(STDOUT_FILENO, &c, read(fd, &c, 1));
}

int main() {
    FILE *fp = fopen("main.sh", "r");
    printf(PS1);
    while (1) {
        char *line = NULL;
        size_t len;
        ssize_t read;
        if ((read = getline(&line, &len, fp)) <= 0) break;
        line[read - 1] = 0;
        shell(line);
        free(line);
    }
    fclose(fp);
    int pid = forkpty(&fd, NULL, NULL, NULL);
    if (pid == 0) {
        setenv("EMU_SLOW_FACTOR", "200000", 1);
        execvp("emu2", (char*[]){"emu2", "snake.com", NULL});
    }
    else {
        _shell("EMU_SLOW_FACTOR=200000 emu2 snake.com");
        write_halt("\x1b[B", 1.9);
        write_halt("\x1b[D", 0.9);
        write_halt("\x1b[A", 0.8);
        write_halt("\x1b[D", 0.8);
        write_halt("\x1b[A", 0.3);
        write_halt("\x1b[D", 0.9);
        write_halt("\x1b[A", 0.8);
        write_halt("\x1b[1;7F", 0);
        waitpid(pid, NULL, 0);
        printf(PS1);
    }
    return 0;
}
