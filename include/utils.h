#ifndef TOLKIENIZER_UTILS_H
#define TOLKIENIZER_UTILS_H

#include <stddef.h>
#include <stdbool.h>

/* Safe string duplicate (handles NULL gracefully) */
char *safe_strdup(const char *s);

/* Trim whitespace from beginning and end in-place, returns pointer to trimmed start */
char *str_trim(char *str);

/* Reads entire file content into an allocated buffer. Sets out_len if not NULL. */
char *read_file_contents(const char *path, size_t *out_len);

/* Reads all content from standard input until EOF. Sets out_len if not NULL. */
char *read_all_stdin(size_t *out_len);

/* Returns path to user home directory */
const char *get_home_directory(void);

/* Returns allocated path to config directory (~/.config/tolkienizer) */
char *get_config_dir(void);

/* Returns allocated path to config file (~/.config/tolkienizer/config.json) */
char *get_config_file_path(void);

/* Creates directory recursively if it doesn't already exist */
int ensure_dir_exists(const char *dir_path);

/* Reads a line of user input with optional password masking */
char *read_line_prompt(const char *prompt, bool hide_input);

/* Spinner for terminal feedback */
void spinner_start(const char *message);
void spinner_stop(void);

#endif /* TOLKIENIZER_UTILS_H */
