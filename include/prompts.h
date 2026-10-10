#ifndef TOLKIENIZER_PROMPTS_H
#define TOLKIENIZER_PROMPTS_H

/* Returns an allocated system prompt for the specified style and language.
 * Caller is responsible for calling free() on the returned string.
 *
 * Supported styles:
 *   - "epic"   (default): Chronicler of Ancient Lands
 *   - "hobbit"          : Tale-spinner of the Shire
 *   - "elvish"          : Lore-singer of the Elven Havens
 *   - "mordor"          : Voice of the Dark Tower
 *
 * Supported languages:
 *   - "en" (default)    : English
 *   - "it"              : Italian
 *   - "es"              : Spanish
 *   - "fr"              : French
 *   - "de"              : German
 *   - "auto"            : Preserve/detect input language
 *   - Custom            : Any language code or name (e.g. "ja", "pt", "Latin")
 */
char *prompt_get(const char *style_name, const char *lang_name);

/* List available style names */
void prompt_list_styles(void);

/* List supported languages */
void prompt_list_languages(void);

/* Normalizes language code or name (e.g. "english" -> "en", "italiano" -> "it") */
const char *prompt_normalize_language(const char *lang);

#endif /* TOLKIENIZER_PROMPTS_H */
