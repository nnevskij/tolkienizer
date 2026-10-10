#include "config.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cJSON.h>

void config_init(tolkienizer_config_t *cfg) {
    if (!cfg) return;
    cfg->api_key = NULL;
    cfg->base_url = safe_strdup(DEFAULT_BASE_URL);
    cfg->model = safe_strdup(DEFAULT_MODEL);
    cfg->default_style = safe_strdup(DEFAULT_STYLE);
    cfg->temperature = DEFAULT_TEMPERATURE;
}

void config_free(tolkienizer_config_t *cfg) {
    if (!cfg) return;
    free(cfg->api_key);
    free(cfg->base_url);
    free(cfg->model);
    free(cfg->default_style);
    cfg->api_key = NULL;
    cfg->base_url = NULL;
    cfg->model = NULL;
    cfg->default_style = NULL;
}

int config_load(tolkienizer_config_t *cfg) {
    config_init(cfg);

    char *cfg_path = get_config_file_path();
    if (cfg_path) {
        size_t len = 0;
        char *content = read_file_contents(cfg_path, &len);
        if (content) {
            cJSON *root = cJSON_Parse(content);
            if (root) {
                cJSON *item = cJSON_GetObjectItemCaseSensitive(root, "api_key");
                if (cJSON_IsString(item) && item->valuestring && strlen(item->valuestring) > 0) {
                    free(cfg->api_key);
                    cfg->api_key = safe_strdup(item->valuestring);
                }

                item = cJSON_GetObjectItemCaseSensitive(root, "base_url");
                if (cJSON_IsString(item) && item->valuestring && strlen(item->valuestring) > 0) {
                    free(cfg->base_url);
                    cfg->base_url = safe_strdup(item->valuestring);
                }

                item = cJSON_GetObjectItemCaseSensitive(root, "model");
                if (cJSON_IsString(item) && item->valuestring && strlen(item->valuestring) > 0) {
                    free(cfg->model);
                    cfg->model = safe_strdup(item->valuestring);
                }

                item = cJSON_GetObjectItemCaseSensitive(root, "style");
                if (cJSON_IsString(item) && item->valuestring && strlen(item->valuestring) > 0) {
                    free(cfg->default_style);
                    cfg->default_style = safe_strdup(item->valuestring);
                }

                item = cJSON_GetObjectItemCaseSensitive(root, "temperature");
                if (cJSON_IsNumber(item)) {
                    cfg->temperature = item->valuedouble;
                }

                cJSON_Delete(root);
            }
            free(content);
        }
        free(cfg_path);
    }

    /* Environment variables take precedence if key wasn't in config file or if env overrides are present */
    if (!cfg->api_key || strlen(cfg->api_key) == 0) {
        const char *env_key = getenv("TOLKIENIZER_API_KEY");
        if (!env_key) env_key = getenv("OPENAI_API_KEY");
        if (!env_key) env_key = getenv("GROQ_API_KEY");
        if (!env_key) env_key = getenv("GEMINI_API_KEY");
        if (!env_key) env_key = getenv("OPENROUTER_API_KEY");

        if (env_key && strlen(env_key) > 0) {
            free(cfg->api_key);
            cfg->api_key = safe_strdup(env_key);
        }
    }

    const char *env_url = getenv("TOLKIENIZER_BASE_URL");
    if (env_url && strlen(env_url) > 0) {
        free(cfg->base_url);
        cfg->base_url = safe_strdup(env_url);
    }

    const char *env_model = getenv("TOLKIENIZER_MODEL");
    if (env_model && strlen(env_model) > 0) {
        free(cfg->model);
        cfg->model = safe_strdup(env_model);
    }

    const char *env_style = getenv("TOLKIENIZER_STYLE");
    if (env_style && strlen(env_style) > 0) {
        free(cfg->default_style);
        cfg->default_style = safe_strdup(env_style);
    }

    return 0;
}

int config_save(const tolkienizer_config_t *cfg) {
    if (!cfg) return -1;

    char *dir = get_config_dir();
    if (!dir) return -1;
    ensure_dir_exists(dir);
    free(dir);

    char *cfg_path = get_config_file_path();
    if (!cfg_path) return -1;

    cJSON *root = cJSON_CreateObject();
    if (!root) {
        free(cfg_path);
        return -1;
    }

    cJSON_AddStringToObject(root, "api_key", cfg->api_key ? cfg->api_key : "");
    cJSON_AddStringToObject(root, "base_url", cfg->base_url ? cfg->base_url : DEFAULT_BASE_URL);
    cJSON_AddStringToObject(root, "model", cfg->model ? cfg->model : DEFAULT_MODEL);
    cJSON_AddStringToObject(root, "style", cfg->default_style ? cfg->default_style : DEFAULT_STYLE);
    cJSON_AddNumberToObject(root, "temperature", cfg->temperature);

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (!json_str) {
        free(cfg_path);
        return -1;
    }

    FILE *f = fopen(cfg_path, "w");
    if (!f) {
        free(json_str);
        free(cfg_path);
        return -1;
    }

    fputs(json_str, f);
    fclose(f);
    chmod(cfg_path, 0600); /* Secure permissions: read/write only by owner */

    free(json_str);
    free(cfg_path);
    return 0;
}

int config_set_key(const char *key) {
    tolkienizer_config_t cfg;
    config_load(&cfg);
    free(cfg.api_key);
    cfg.api_key = safe_strdup(key);
    int res = config_save(&cfg);
    config_free(&cfg);
    return res;
}

int config_set_model(const char *model) {
    tolkienizer_config_t cfg;
    config_load(&cfg);
    free(cfg.model);
    cfg.model = safe_strdup(model);
    int res = config_save(&cfg);
    config_free(&cfg);
    return res;
}

int config_set_url(const char *url) {
    tolkienizer_config_t cfg;
    config_load(&cfg);
    free(cfg.base_url);
    cfg.base_url = safe_strdup(url);
    int res = config_save(&cfg);
    config_free(&cfg);
    return res;
}

int config_set_style(const char *style) {
    tolkienizer_config_t cfg;
    config_load(&cfg);
    free(cfg.default_style);
    cfg.default_style = safe_strdup(style);
    int res = config_save(&cfg);
    config_free(&cfg);
    return res;
}

void config_show(const tolkienizer_config_t *cfg) {
    char *cfg_path = get_config_file_path();
    fprintf(stderr, "\033[1;36m📜 Tolkienizer Configuration:\033[0m\n");
    fprintf(stderr, "  Config file : %s\n", cfg_path ? cfg_path : "(unknown)");
    free(cfg_path);

    if (cfg->api_key && strlen(cfg->api_key) > 0) {
        size_t len = strlen(cfg->api_key);
        if (len > 8) {
            fprintf(stderr, "  API Key     : %.4s...%.4s (configured)\n", cfg->api_key, cfg->api_key + len - 4);
        } else {
            fprintf(stderr, "  API Key     : ******** (configured)\n");
        }
    } else {
        fprintf(stderr, "  API Key     : \033[31m(not configured)\033[0m\n");
    }

    fprintf(stderr, "  Base URL    : %s\n", cfg->base_url ? cfg->base_url : DEFAULT_BASE_URL);
    fprintf(stderr, "  Model       : %s\n", cfg->model ? cfg->model : DEFAULT_MODEL);
    fprintf(stderr, "  Style       : %s\n", cfg->default_style ? cfg->default_style : DEFAULT_STYLE);
    fprintf(stderr, "  Temperature : %.2f\n", cfg->temperature);
}

bool config_has_api_key(const tolkienizer_config_t *cfg) {
    if (!cfg) return false;
    /* Local Ollama typically runs on localhost and does not require an API key */
    if (cfg->base_url && (strstr(cfg->base_url, "localhost") || strstr(cfg->base_url, "127.0.0.1"))) {
        return true;
    }
    return (cfg->api_key != NULL && strlen(cfg->api_key) > 0);
}

int config_interactive_wizard(void) {
    tolkienizer_config_t cfg;
    config_load(&cfg);

    fprintf(stderr, "\n");
    fprintf(stderr, "\033[1;33m🧙================================================🧙\033[0m\n");
    fprintf(stderr, "\033[1;33m       Tolkienizer - Configuration Wizard         \033[0m\n");
    fprintf(stderr, "\033[1;33m🧙================================================🧙\033[0m\n\n");

    fprintf(stderr, "Select your AI provider:\n");
    fprintf(stderr, "  1) OpenAI (Default: gpt-4o-mini)\n");
    fprintf(stderr, "  2) Groq (Ultra-fast: llama-3.3-70b-versatile)\n");
    fprintf(stderr, "  3) Google Gemini (via OpenAI compat: gemini-2.0-flash)\n");
    fprintf(stderr, "  4) OpenRouter (Multi-model: anthropic/claude-3.5-haiku)\n");
    fprintf(stderr, "  5) Ollama (Local model: llama3, no API key required)\n");
    fprintf(stderr, "  6) Custom OpenAI-compatible endpoint\n\n");

    char *choice = read_line_prompt("Choice [1-6, default: 1]: ", false);
    int opt = 1;
    if (choice && strlen(str_trim(choice)) > 0) {
        opt = atoi(choice);
    }
    free(choice);

    free(cfg.base_url);
    free(cfg.model);

    bool needs_key = true;

    switch (opt) {
        case 2:
            cfg.base_url = safe_strdup("https://api.groq.com/openai/v1");
            cfg.model = safe_strdup("llama-3.3-70b-versatile");
            break;
        case 3:
            cfg.base_url = safe_strdup("https://generativelanguage.googleapis.com/v1beta/openai");
            cfg.model = safe_strdup("gemini-2.0-flash");
            break;
        case 4:
            cfg.base_url = safe_strdup("https://openrouter.ai/api/v1");
            cfg.model = safe_strdup("anthropic/claude-3.5-haiku");
            break;
        case 5:
            cfg.base_url = safe_strdup("http://localhost:11434/v1");
            cfg.model = safe_strdup("llama3");
            needs_key = false;
            break;
        case 6: {
            char *custom_url = read_line_prompt("Enter base URL (e.g. https://api.example.com/v1): ", false);
            cfg.base_url = safe_strdup(custom_url ? str_trim(custom_url) : DEFAULT_BASE_URL);
            free(custom_url);

            char *custom_model = read_line_prompt("Enter model name (e.g. gpt-4o-mini): ", false);
            cfg.model = safe_strdup(custom_model ? str_trim(custom_model) : DEFAULT_MODEL);
            free(custom_model);
            break;
        }
        case 1:
        default:
            cfg.base_url = safe_strdup("https://api.openai.com/v1");
            cfg.model = safe_strdup("gpt-4o-mini");
            break;
    }

    if (needs_key) {
        fprintf(stderr, "\nEnter your API key (input hidden):\n");
        char *key = read_line_prompt("API Key: ", true);
        if (key && strlen(str_trim(key)) > 0) {
            free(cfg.api_key);
            cfg.api_key = safe_strdup(key);
        }
        free(key);
    } else {
        free(cfg.api_key);
        cfg.api_key = safe_strdup("ollama");
    }

    int res = config_save(&cfg);
    if (res == 0) {
        fprintf(stderr, "\n\033[1;32m✓ Configuration successfully saved!\033[0m\n");
        config_show(&cfg);
    } else {
        fprintf(stderr, "\n\033[1;31m✗ Error: Failed to save configuration file.\033[0m\n");
    }

    config_free(&cfg);
    return res;
}
