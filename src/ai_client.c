#include "ai_client.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <cJSON.h>

typedef struct {
    char *data;
    size_t size;
} memory_buffer_t;

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    memory_buffer_t *mem = (memory_buffer_t *)userp;

    char *ptr = realloc(mem->data, mem->size + real_size + 1);
    if (!ptr) {
        return 0; /* out of memory */
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->data[mem->size] = 0;

    return real_size;
}

int ai_transform_text(const tolkienizer_config_t *cfg,
                      const char *system_prompt,
                      const char *user_input,
                      char **out_response,
                      char **out_error) {
    if (!cfg || !system_prompt || !user_input || !out_response || !out_error) {
        if (out_error) *out_error = safe_strdup("Invalid arguments provided to AI client.");
        return -1;
    }

    *out_response = NULL;
    *out_error = NULL;

    const char *demo_mode = getenv("TOLKIENIZER_DEMO");
    if (demo_mode && strcmp(demo_mode, "1") == 0) {
        usleep(1200000); /* 1.2s smooth animated spinner */
        if (strstr(system_prompt, "Comt") || strstr(system_prompt, "Shire") || strstr(system_prompt, "Contea") || strstr(system_prompt, "Comarca") || strstr(system_prompt, "Auenland")) {
            *out_response = safe_strdup("I must take a jaunt to the market for fine tea and butter,\nas a respectable halfling's pantry should never be caught in want.");
        } else if (strstr(system_prompt, "Mordor") || strstr(system_prompt, "Dark Tower") || strstr(system_prompt, "Barad-d") || strstr(system_prompt, "Torre Oscura") || strstr(system_prompt, "Tour Sombre") || strstr(system_prompt, "Dunklen Turm")) {
            *out_response = safe_strdup("The release must be unleashed upon the world ere Friday's twilight,\nlest the shadows claim your delay.");
        } else if (strstr(system_prompt, "Eldar") || strstr(system_prompt, "Elven") || strstr(system_prompt, "Elfici") || strstr(system_prompt, "Élficos") || strstr(system_prompt, "Erstgeborenen")) {
            *out_response = safe_strdup("May the starlight of Elbereth guide our journey,\nfor ancient memories fade not and peace rests upon the woodland realm.");
        } else {
            *out_response = safe_strdup("The great engine of the data-vault hath fallen into darkness,\nbrought low by the relentless tide of exhaustion.");
        }
        return 0;
    }

    /* Build endpoint URL */
    char endpoint[1024];
    size_t base_len = strlen(cfg->base_url);
    if (base_len > 0 && cfg->base_url[base_len - 1] == '/') {
        snprintf(endpoint, sizeof(endpoint), "%schat/completions", cfg->base_url);
    } else {
        snprintf(endpoint, sizeof(endpoint), "%s/chat/completions", cfg->base_url);
    }

    /* Build JSON request payload */
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        *out_error = safe_strdup("Failed to allocate JSON payload object.");
        return -1;
    }

    cJSON_AddStringToObject(root, "model", cfg->model ? cfg->model : "gpt-4o-mini");
    cJSON_AddNumberToObject(root, "temperature", cfg->temperature);

    cJSON *messages = cJSON_CreateArray();
    if (!messages) {
        cJSON_Delete(root);
        *out_error = safe_strdup("Failed to create messages JSON array.");
        return -1;
    }

    /* System message (the pre-prompt) */
    cJSON *sys_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(sys_msg, "role", "system");
    cJSON_AddStringToObject(sys_msg, "content", system_prompt);
    cJSON_AddItemToArray(messages, sys_msg);

    /* User message */
    cJSON *usr_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(usr_msg, "role", "user");
    cJSON_AddStringToObject(usr_msg, "content", user_input);
    cJSON_AddItemToArray(messages, usr_msg);

    cJSON_AddItemToObject(root, "messages", messages);

    char *post_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (!post_data) {
        *out_error = safe_strdup("Failed to serialize JSON payload.");
        return -1;
    }

    /* Initialize libcurl */
    CURL *curl = curl_easy_init();
    if (!curl) {
        free(post_data);
        *out_error = safe_strdup("Failed to initialize libcurl.");
        return -1;
    }

    memory_buffer_t chunk;
    chunk.data = malloc(1);
    chunk.size = 0;
    if (chunk.data) chunk.data[0] = '\0';

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    if (cfg->api_key && strlen(cfg->api_key) > 0) {
        char auth_header[2048];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", cfg->api_key);
        headers = curl_slist_append(headers, auth_header);
    }

    /* OpenRouter identification headers */
    headers = curl_slist_append(headers, "HTTP-Referer: https://github.com/tolkienizer");
    headers = curl_slist_append(headers, "X-Title: Tolkienizer CLI");

    curl_easy_setopt(curl, CURLOPT_URL, endpoint);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    free(post_data);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        char err_buf[512];
        snprintf(err_buf, sizeof(err_buf), "Network error: %s", curl_easy_strerror(res));
        *out_error = safe_strdup(err_buf);
        free(chunk.data);
        return -1;
    }

    if (http_code != 200) {
        char err_buf[1024];
        /* Try to extract error message from API json response */
        cJSON *err_root = cJSON_Parse(chunk.data ? chunk.data : "");
        char *api_msg = NULL;
        if (err_root) {
            cJSON *err_obj = cJSON_GetObjectItemCaseSensitive(err_root, "error");
            if (err_obj) {
                cJSON *msg_obj = cJSON_GetObjectItemCaseSensitive(err_obj, "message");
                if (cJSON_IsString(msg_obj) && msg_obj->valuestring) {
                    api_msg = safe_strdup(msg_obj->valuestring);
                }
            }
            cJSON_Delete(err_root);
        }

        if (http_code == 401) {
            snprintf(err_buf, sizeof(err_buf),
                     "Authentication failed (HTTP 401): %s\nPlease check your API key with 'tolkienizer --config' or 'tolkienizer --set-key'.",
                     api_msg ? api_msg : "Invalid API key");
        } else if (http_code == 429) {
            snprintf(err_buf, sizeof(err_buf),
                     "Rate limit or quota exceeded (HTTP 429): %s",
                     api_msg ? api_msg : "You have exceeded your current quota or request rate.");
        } else {
            snprintf(err_buf, sizeof(err_buf),
                     "API returned HTTP %ld: %s",
                     http_code,
                     api_msg ? api_msg : (chunk.data ? chunk.data : "Unknown error"));
        }

        free(api_msg);
        free(chunk.data);
        *out_error = safe_strdup(err_buf);
        return -1;
    }

    /* Parse successful response */
    cJSON *resp_json = cJSON_Parse(chunk.data ? chunk.data : "");
    free(chunk.data);

    if (!resp_json) {
        *out_error = safe_strdup("Failed to parse JSON response from AI server.");
        return -1;
    }

    cJSON *choices = cJSON_GetObjectItemCaseSensitive(resp_json, "choices");
    if (!cJSON_IsArray(choices) || cJSON_GetArraySize(choices) == 0) {
        cJSON_Delete(resp_json);
        *out_error = safe_strdup("No completions returned in API response.");
        return -1;
    }

    cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
    cJSON *msg = cJSON_GetObjectItemCaseSensitive(first_choice, "message");
    if (!msg) {
        cJSON_Delete(resp_json);
        *out_error = safe_strdup("Malformed choice object in API response.");
        return -1;
    }

    cJSON *content = cJSON_GetObjectItemCaseSensitive(msg, "content");
    if (!cJSON_IsString(content) || !content->valuestring) {
        cJSON_Delete(resp_json);
        *out_error = safe_strdup("No text content returned in message.");
        return -1;
    }

    *out_response = safe_strdup(content->valuestring);
    cJSON_Delete(resp_json);

    return 0;
}
