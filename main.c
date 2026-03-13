#include <unistd.h>
#include <pty.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>
#include <espeak-ng/speak_lib.h>
#define small_delay() usleep(100000)
#define delay() usleep(1000000)
// generated on my system using:
// echo | bash --rcfile <(echo "PS1='$PS1'") -i 2>&1 | head -n1 | sed -n l | sed 's/\$$//'
#define PS1 "\033[01;32mroot\033[00m@\033[01;34m/root/snake-vid\033[00m$ "

int fd;

static FILE *out_audio;
static int sample_rate;
static volatile long long samples = 0;
static struct timespec audio_end = {0};

int synth_cb(short *wav, int numsamples, espeak_EVENT *events) {
    if (wav && numsamples) {
        fwrite(wav, sizeof(short), numsamples, out_audio);
        samples += numsamples;
        printf("%lld ", samples); fflush(stdout);
    }
    return 0;
}

void audio_wait() {
    espeak_Synchronize();
    printf("  %lld  ", samples); fflush(stdout);
    long long ns = (1000000000LL * samples) / sample_rate;
    audio_end.tv_nsec += ns % 1000000000LL;
    audio_end.tv_sec += ns / 1000000000LL + audio_end.tv_nsec / 1000000000LL;
    audio_end.tv_nsec %= 1000000000LL;
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &audio_end, NULL);
    samples = 0;
}

void _shell(const char *command, int wait_audio) {
    fflush(stdout);
    delay();
    for (; *command; command++) {
        printf("%c", *command);
        fflush(stdout);
        small_delay();
    }
    delay();
    if (wait_audio) audio_wait();
    puts("");
}

int shell(const char *command) {
    _shell(command, 1);
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

void say_comment(char *comment) {
    fflush(stdout);
    audio_wait();
    clock_gettime(CLOCK_MONOTONIC, &audio_end);
    espeak_Synth(comment + 1, strlen(comment), 0, POS_CHARACTER,
                 0, espeakCHARS_AUTO, NULL, NULL);
}

int main() {
    out_audio = fopen("main.raw", "wb");
    sample_rate = espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0);
    espeak_SetSynthCallback(synth_cb);
    FILE *sample_rate_file = fopen("fr.txt", "w");
    fprintf(sample_rate_file, "%d", sample_rate);
    fclose(sample_rate_file);
    FILE *fp = fopen("main.sh", "r");
    printf(PS1);
    fflush(stdout);
    clock_gettime(CLOCK_MONOTONIC, &audio_end);
    while (1) {
        char *line = NULL;
        size_t len;
        ssize_t read;
        if ((read = getline(&line, &len, fp)) <= 0) break;
        line[read - 1] = 0;
        if (line[0] != '#') {
            if (shell(line)) exit(1);
        }
        else say_comment(line);
        free(line);
    }
    fclose(fp);
    int pid = forkpty(&fd, NULL, NULL, NULL);
    if (pid == 0) {
        setenv("EMU_SLOW_FACTOR", "200000", 1);
        execvp("emu2", (char*[]){"emu2", "snake.com", NULL});
    }
    _shell("EMU_SLOW_FACTOR=200000 emu2 snake.com", 0);
    write_halt("\x1b[B", 2.25);
    write_halt("\x1b[D", 0.8);
    write_halt("\x1b[A", 0.8);
    write_halt("\x1b[D", 0.8);
    write_halt("\x1b[A", 0.3);
    write_halt("\x1b[D", 0.9);
    write_halt("\x1b[A", 0.8);
    write_halt("\x1b[1;7F", 0);
    waitpid(pid, NULL, 0);
    audio_wait();
    fclose(out_audio);
    return 0;
}
