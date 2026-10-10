#ifndef TOLKIENIZER_AI_CLIENT_H
#define TOLKIENIZER_AI_CLIENT_H

#include "config.h"

/* Query the AI model to transform the user input using the specified system prompt.
 *
 * Parameters:
 *   cfg          - Configuration (keys, model, base url, temperature)
 *   system_prompt- Pre-prompt instructing the model on Tolkien persona & guidelines
 *   user_input   - Text provided by the user to be transformed
 *   out_response - Pointer to receive allocated string containing the transformed text
 *   out_error    - Pointer to receive allocated error message string on failure
 *
 * Returns:
 *   0 on success, non-zero on failure.
 */
int ai_transform_text(const tolkienizer_config_t *cfg,
                      const char *system_prompt,
                      const char *user_input,
                      char **out_response,
                      char **out_error);

#endif /* TOLKIENIZER_AI_CLIENT_H */
