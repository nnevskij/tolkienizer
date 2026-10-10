#ifndef TOLKIENIZER_CONFIG_H
#define TOLKIENIZER_CONFIG_H

#include <stdbool.h>

#define DEFAULT_BASE_URL    "https://api.openai.com/v1"
#define DEFAULT_MODEL       "gpt-4o-mini"
#define DEFAULT_STYLE       "epic"
#define DEFAULT_TEMPERATURE 0.7

typedef struct {
    char *api_key;
    char *base_url;
    char *model;
    char *default_style;
    double temperature;
} tolkienizer_config_t;

/* Initialize configuration struct with default values */
void config_init(tolkienizer_config_t *cfg);

/* Free heap allocated fields of configuration */
void config_free(tolkienizer_config_t *cfg);

/* Load configuration from file and merge with environment variables */
int config_load(tolkienizer_config_t *cfg);

/* Save current configuration to user config file (~/.config/tolkienizer/config.json) */
int config_save(const tolkienizer_config_t *cfg);

/* Setters that load, update, and save configuration */
int config_set_key(const char *key);
int config_set_model(const char *model);
int config_set_url(const char *url);
int config_set_style(const char *style);

/* Display active configuration in a safe human-readable format */
void config_show(const tolkienizer_config_t *cfg);

/* Run interactive configuration wizard */
int config_interactive_wizard(void);

/* Check if an API key is present in configuration or environment */
bool config_has_api_key(const tolkienizer_config_t *cfg);

#endif /* TOLKIENIZER_CONFIG_H */
