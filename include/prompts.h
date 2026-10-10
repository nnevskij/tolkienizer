#ifndef TOLKIENIZER_PROMPTS_H
#define TOLKIENIZER_PROMPTS_H

/* Returns the system prompt for the specified style.
 * Supported styles: "epic" (default), "hobbit", "elvish", "mordor".
 * If style_name is NULL or unknown, returns the default "epic" prompt.
 */
const char *prompt_get_by_style(const char *style_name);

/* List available style names */
void prompt_list_styles(void);

#endif /* TOLKIENIZER_PROMPTS_H */
