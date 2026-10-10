#define _POSIX_C_SOURCE 200809L
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <termios.h>
#include <pthread.h>

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    char *copy = malloc(strlen(s) + 1);
    if (copy) {
        strcpy(copy, s);
    }
    return copy;
}

char *str_trim(char *str) {
    if (!str) return NULL;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = 0;
        end--;
    }
    return str;
}

char *read_file_contents(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 0) {
        fclose(f);
        return NULL;
    }

    char *buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read_bytes = fread(buf, 1, size, f);
    buf[read_bytes] = '\0';
    fclose(f);

    if (out_len) {
        *out_len = read_bytes;
    }
    return buf;
}

char *read_all_stdin(size_t *out_len) {
    size_t capacity = 4096;
    size_t total = 0;
    char *buf = malloc(capacity);
    if (!buf) return NULL;

    char chunk[1024];
    size_t n;
    while ((n = fread(chunk, 1, sizeof(chunk), stdin)) > 0) {
        if (total + n + 1 > capacity) {
            capacity = (total + n + 1) * 2;
            char *new_buf = realloc(buf, capacity);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        memcpy(buf + total, chunk, n);
        total += n;
    }

    buf[total] = '\0';
    if (out_len) {
        *out_len = total;
    }
    return buf;
}

const char *get_home_directory(void) {
    const char *home = getenv("HOME");
    if (home && strlen(home) > 0) {
        return home;
    }
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_dir) {
        return pw->pw_dir;
    }
    return ".";
}

char *get_config_dir(void) {
    const char *xdg = getenv("XDG_CONFIG_HOME");
    char path[1024];
    if (xdg && strlen(xdg) > 0) {
        snprintf(path, sizeof(path), "%s/tolkienizer", xdg);
    } else {
        const char *home = get_home_directory();
        snprintf(path, sizeof(path), "%s/.config/tolkienizer", home);
    }
    return safe_strdup(path);
}

char *get_config_file_path(void) {
    char *dir = get_config_dir();
    char path[1280];
    snprintf(path, sizeof(path), "%s/config.json", dir);
    free(dir);
    return safe_strdup(path);
}

int ensure_dir_exists(const char *dir_path) {
    char tmp[1024];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir_path);
    len = strlen(tmp);
    if (len == 0) return 0;
    if (tmp[len - 1] == '/') tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0700);
            *p = '/';
        }
    }
    return mkdir(tmp, 0700);
}

char *read_line_prompt(const char *prompt, bool hide_input) {
    if (prompt) {
        fprintf(stderr, "%s", prompt);
        fflush(stderr);
    }

    struct termios old_term, new_term;
    int is_tty = isatty(STDIN_FILENO);

    if (hide_input && is_tty) {
        tcgetattr(STDIN_FILENO, &old_term);
        new_term = old_term;
        new_term.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    }

    char *line = NULL;
    size_t cap = 0;
    ssize_t len = getline(&line, &cap, stdin);

    if (hide_input && is_tty) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
        fprintf(stderr, "\n");
    }

    if (len < 0) {
        free(line);
        return NULL;
    }

    /* Strip trailing newline */
    if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') {
            line[len - 2] = '\0';
        }
    }

    return line;
}

/* Spinner thread control */
static pthread_t spinner_thread;
static volatile bool spinner_running = false;
static char spinner_msg[256] = "";

static void *spinner_worker(void *arg) {
    (void)arg;
    const char *frames[] = {
        "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
    };
    int num_frames = sizeof(frames) / sizeof(frames[0]);
    int idx = 0;

    while (spinner_running) {
        fprintf(stderr, "\r\033[33m%s\033[0m %s", frames[idx], spinner_msg);
        fflush(stderr);
        idx = (idx + 1) % num_frames;
        usleep(80000); /* 80ms */
    }
    /* Clear line when done */
    fprintf(stderr, "\r\033[K");
    fflush(stderr);
    return NULL;
}

void spinner_start(const char *message) {
    if (!isatty(STDERR_FILENO)) return;
    const char *term = getenv("TERM");
    if (term && strcmp(term, "dumb") == 0) return;
    if (spinner_running) return;

    if (message) {
        strncpy(spinner_msg, message, sizeof(spinner_msg) - 1);
        spinner_msg[sizeof(spinner_msg) - 1] = '\0';
    } else {
        strcpy(spinner_msg, "Consulting the Lore of Middle-earth...");
    }

    spinner_running = true;
    pthread_create(&spinner_thread, NULL, spinner_worker, NULL);
}

void spinner_stop(void) {
    if (!spinner_running) return;
    spinner_running = false;
    pthread_join(spinner_thread, NULL);
    fprintf(stderr, "\r\033[2K\r");
    fflush(stderr);
}
