#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <strings.h>

#include "config.h"
#include "prompts.h"
#include "ai_client.h"
#include "utils.h"

#define PROGRAM_NAME    "tolkienizer"
#define PROGRAM_VERSION "1.1.0"

static void print_version(void) {
    printf("%s version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    printf("Transmute ordinary words into the mythic prose of J.R.R. Tolkien.\n");
}

static void print_usage(const char *prog_name) {
    printf("Uso: %s [OPZIONI] [TESTO...]\n\n", prog_name);
    printf("Trasforma qualsiasi testo in prosa epica in stile J.R.R. Tolkien tramite AI.\n\n");
    printf("Opzioni di input:\n");
    printf("  TESTO...                  Testo da trasformare direttamente da riga di comando\n");
    printf("  -f, --file FILE           Legge il testo da un file\n");
    printf("  (pipe / stdin)            Accetta testo via standard input (es. echo \"testo\" | %s)\n\n", prog_name);
    printf("Opzioni di lingua e stile:\n");
    printf("  -l, --lang LINGUA         Lingua target: en (default), it, es, fr, de, auto (o personalizzata)\n");
    printf("  -s, --style STILE         Stile Tolkien: epic (default), hobbit, elvish, mordor\n");
    printf("  -p, --prompt TESTO        System prompt personalizzato (sovrascrive stile e lingua)\n");
    printf("      --list-langs          Elenca le lingue disponibili e le opzioni multilingua\n");
    printf("      --list-styles         Elenca tutti gli stili narrativi disponibili\n\n");
    printf("Opzioni di esecuzione:\n");
    printf("  -m, --model MODELLO       Modello AI (default: da configurazione o gpt-4o-mini)\n");
    printf("  -k, --key CHIAVE          Chiave API per questa singola esecuzione\n");
    printf("  -u, --url URL             URL di base API (default: https://api.openai.com/v1)\n");
    printf("  -t, --temp FLOAT          Temperatura di generazione (0.0 - 2.0, default: 0.7)\n");
    printf("  -q, --quiet               Modalità silenziosa (nessun indicatore di caricamento)\n");
    printf("  -v, --version             Mostra la versione del programma\n");
    printf("  -h, --help                Mostra questo messaggio di aiuto\n\n");
    printf("Gestione configurazione e chiavi API:\n");
    printf("      --config              Avvia il wizard interattivo di configurazione\n");
    printf("      --set-key CHIAVE      Salva la chiave API nel file di configurazione\n");
    printf("      --set-lang LINGUA     Salva la lingua predefinita (en, it, es, fr, de, auto)\n");
    printf("      --set-model MODELLO   Salva il modello predefinito nella configurazione\n");
    printf("      --set-url URL         Salva l'endpoint API nella configurazione\n");
    printf("      --set-style STILE     Salva lo stile predefinito nella configurazione\n");
    printf("      --show-config         Mostra le impostazioni attuali\n\n");
    printf("Provider supportati:\n");
    printf("  OpenAI, Groq, Google Gemini (OpenAI endpoint), OpenRouter, Ollama (locale).\n\n");
    printf("Esempi:\n");
    printf("  %s \"The production server crashed due to out of memory\"\n", prog_name);
    printf("  %s -l it \"Oggi il server di produzione è andato in crash\"\n", prog_name);
    printf("  %s -l es -s hobbit \"Tengo que ir al mercado a comprar pan y queso\"\n", prog_name);
    printf("  %s -l fr -s elvish \"Le soleil se couche sur la forêt enchantée\"\n", prog_name);
    printf("  echo \"I finished all sprint tasks\" | %s -s epic\n", prog_name);
}

static void print_missing_key_guide(void) {
    fprintf(stderr, "\n\033[1;31m[!] Nessuna chiave API configurata.\033[0m\n\n");
    fprintf(stderr, "Per usare %s, configura la tua chiave API in uno dei seguenti modi:\n\n", PROGRAM_NAME);
    fprintf(stderr, "  1. Configurazione interattiva guidata (scelta provider, lingua e chiave):\n");
    fprintf(stderr, "     \033[1;32m%s --config\033[0m\n\n", PROGRAM_NAME);
    fprintf(stderr, "  2. Salvataggio diretto della chiave nel tuo profilo utente:\n");
    fprintf(stderr, "     \033[1;32m%s --set-key \"tua-chiave-api\"\033[0m\n\n", PROGRAM_NAME);
    fprintf(stderr, "  3. Variabile d'ambiente temporanea o nel tuo ~/.zshrc / ~/.bashrc:\n");
    fprintf(stderr, "     \033[1;32mexport OPENAI_API_KEY=\"tua-chiave-api\"\033[0m  (oppure TOLKIENIZER_API_KEY)\n\n");
    fprintf(stderr, "  4. Parametro singolo da linea di comando:\n");
    fprintf(stderr, "     \033[1;32m%s -k \"tua-chiave-api\" \"testo da trasformare\"\033[0m\n\n", PROGRAM_NAME);
    fprintf(stderr, "Supporta chiavi OpenAI, Groq (gratuitamente veloce), Gemini, OpenRouter o Ollama locale.\n\n");
}

int main(int argc, char *argv[]) {
    tolkienizer_config_t cfg;
    config_load(&cfg);

    char *file_path = NULL;
    char *custom_prompt = NULL;
    char *cli_style = NULL;
    char *cli_lang = NULL;
    bool quiet = false;

    /* Long options flags */
    enum {
        OPT_CONFIG_WIZARD = 1000,
        OPT_SET_KEY,
        OPT_SET_MODEL,
        OPT_SET_URL,
        OPT_SET_STYLE,
        OPT_SET_LANG,
        OPT_SHOW_CONFIG,
        OPT_LIST_STYLES,
        OPT_LIST_LANGS
    };

    static struct option long_options[] = {
        {"help",        no_argument,       NULL, 'h'},
        {"version",     no_argument,       NULL, 'v'},
        {"file",        required_argument, NULL, 'f'},
        {"style",       required_argument, NULL, 's'},
        {"lang",        required_argument, NULL, 'l'},
        {"language",    required_argument, NULL, 'l'},
        {"prompt",      required_argument, NULL, 'p'},
        {"model",       required_argument, NULL, 'm'},
        {"key",         required_argument, NULL, 'k'},
        {"url",         required_argument, NULL, 'u'},
        {"temp",        required_argument, NULL, 't'},
        {"quiet",       no_argument,       NULL, 'q'},
        {"config",      no_argument,       NULL, OPT_CONFIG_WIZARD},
        {"set-key",     required_argument, NULL, OPT_SET_KEY},
        {"set-model",   required_argument, NULL, OPT_SET_MODEL},
        {"set-url",     required_argument, NULL, OPT_SET_URL},
        {"set-style",   required_argument, NULL, OPT_SET_STYLE},
        {"set-lang",    required_argument, NULL, OPT_SET_LANG},
        {"show-config", no_argument,       NULL, OPT_SHOW_CONFIG},
        {"list-styles", no_argument,       NULL, OPT_LIST_STYLES},
        {"list-langs",  no_argument,       NULL, OPT_LIST_LANGS},
        {NULL,          0,                 NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hvf:s:l:p:m:k:u:t:q", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                config_free(&cfg);
                return 0;
            case 'v':
                print_version();
                config_free(&cfg);
                return 0;
            case 'f':
                file_path = safe_strdup(optarg);
                break;
            case 's':
                cli_style = safe_strdup(optarg);
                break;
            case 'l':
                cli_lang = safe_strdup(optarg);
                break;
            case 'p':
                custom_prompt = safe_strdup(optarg);
                break;
            case 'm':
                free(cfg.model);
                cfg.model = safe_strdup(optarg);
                break;
            case 'k':
                free(cfg.api_key);
                cfg.api_key = safe_strdup(optarg);
                break;
            case 'u':
                free(cfg.base_url);
                cfg.base_url = safe_strdup(optarg);
                break;
            case 't':
                cfg.temperature = atof(optarg);
                if (cfg.temperature < 0.0) cfg.temperature = 0.0;
                if (cfg.temperature > 2.0) cfg.temperature = 2.0;
                break;
            case 'q':
                quiet = true;
                break;
            case OPT_CONFIG_WIZARD: {
                int res = config_interactive_wizard();
                config_free(&cfg);
                return res;
            }
            case OPT_SET_KEY: {
                int res = config_set_key(optarg);
                if (res == 0) {
                    fprintf(stderr, "\033[1;32m✓ Chiave API salvata con successo!\033[0m\n");
                } else {
                    fprintf(stderr, "\033[1;31m✗ Errore nel salvataggio della chiave API.\033[0m\n");
                }
                config_free(&cfg);
                return res;
            }
            case OPT_SET_MODEL: {
                int res = config_set_model(optarg);
                if (res == 0) {
                    fprintf(stderr, "\033[1;32m✓ Modello predefinito aggiornato: %s\033[0m\n", optarg);
                } else {
                    fprintf(stderr, "\033[1;31m✗ Errore nel salvataggio del modello.\033[0m\n");
                }
                config_free(&cfg);
                return res;
            }
            case OPT_SET_URL: {
                int res = config_set_url(optarg);
                if (res == 0) {
                    fprintf(stderr, "\033[1;32m✓ URL endpoint aggiornato: %s\033[0m\n", optarg);
                } else {
                    fprintf(stderr, "\033[1;31m✗ Errore nel salvataggio dell'URL.\033[0m\n");
                }
                config_free(&cfg);
                return res;
            }
            case OPT_SET_STYLE: {
                int res = config_set_style(optarg);
                if (res == 0) {
                    fprintf(stderr, "\033[1;32m✓ Stile predefinito aggiornato: %s\033[0m\n", optarg);
                } else {
                    fprintf(stderr, "\033[1;31m✗ Errore nel salvataggio dello stile.\033[0m\n");
                }
                config_free(&cfg);
                return res;
            }
            case OPT_SET_LANG: {
                int res = config_set_lang(optarg);
                if (res == 0) {
                    fprintf(stderr, "\033[1;32m✓ Lingua predefinita aggiornata: %s\033[0m\n", optarg);
                } else {
                    fprintf(stderr, "\033[1;31m✗ Errore nel salvataggio della lingua.\033[0m\n");
                }
                config_free(&cfg);
                return res;
            }
            case OPT_SHOW_CONFIG:
                config_show(&cfg);
                config_free(&cfg);
                return 0;
            case OPT_LIST_STYLES:
                prompt_list_styles();
                config_free(&cfg);
                return 0;
            case OPT_LIST_LANGS:
                prompt_list_languages();
                config_free(&cfg);
                return 0;
            default:
                print_usage(argv[0]);
                config_free(&cfg);
                return 1;
        }
    }

    /* Acquire input text */
    char *input_text = NULL;

    if (file_path) {
        size_t len = 0;
        input_text = read_file_contents(file_path, &len);
        if (!input_text) {
            fprintf(stderr, "\033[1;31mErrore:\033[0m Impossibile leggere il file '%s'\n", file_path);
            free(file_path);
            free(cli_style);
            free(cli_lang);
            free(custom_prompt);
            config_free(&cfg);
            return 1;
        }
        free(file_path);
    } else if (optind < argc) {
        /* Concatenate all remaining positional arguments */
        size_t total_len = 0;
        for (int i = optind; i < argc; i++) {
            total_len += strlen(argv[i]) + 1;
        }
        input_text = malloc(total_len + 1);
        if (input_text) {
            input_text[0] = '\0';
            for (int i = optind; i < argc; i++) {
                strcat(input_text, argv[i]);
                if (i < argc - 1) {
                    strcat(input_text, " ");
                }
            }
        }
    } else if (!isatty(STDIN_FILENO)) {
        /* Read from pipe or redirected stdin */
        size_t len = 0;
        input_text = read_all_stdin(&len);
    } else {
        /* Interactive stdin mode */
        fprintf(stderr, "\033[1;33m📜 Inserisci il testo da trasformare (premi Invio, poi Ctrl+D per confermare):\033[0m\n");
        size_t len = 0;
        input_text = read_all_stdin(&len);
    }

    if (!input_text || strlen(str_trim(input_text)) == 0) {
        fprintf(stderr, "Nessun testo fornito. Usa '%s --help' per la guida.\n", argv[0]);
        free(input_text);
        free(cli_style);
        free(cli_lang);
        free(custom_prompt);
        config_free(&cfg);
        return 0;
    }

    /* Verify API key availability */
    if (!config_has_api_key(&cfg)) {
        if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO)) {
            print_missing_key_guide();
            char *ans = read_line_prompt("Vuoi avviare la configurazione guidata adesso? [S/n]: ", false);
            bool proceed = true;
            if (ans && (strcasecmp(str_trim(ans), "n") == 0 || strcasecmp(str_trim(ans), "no") == 0)) {
                proceed = false;
            }
            free(ans);

            if (proceed) {
                config_interactive_wizard();
                config_free(&cfg);
                config_load(&cfg);
            }
        } else {
            print_missing_key_guide();
        }

        if (!config_has_api_key(&cfg)) {
            fprintf(stderr, "\033[1;31mOperazione annullata: chiave API non disponibile.\033[0m\n");
            free(input_text);
            free(cli_style);
            free(cli_lang);
            free(custom_prompt);
            config_free(&cfg);
            return 1;
        }
    }

    /* Choose system prompt */
    char *sys_prompt = NULL;
    bool free_sys_prompt = false;

    if (custom_prompt) {
        sys_prompt = custom_prompt;
    } else {
        const char *selected_style = cli_style ? cli_style : cfg.default_style;
        const char *selected_lang = cli_lang ? cli_lang : cfg.language;
        sys_prompt = prompt_get(selected_style, selected_lang);
        free_sys_prompt = true;
    }

    /* Show loading spinner if not quiet and stderr is a terminal */
    if (!quiet) {
        spinner_start("✨ Evoking the Lore of Middle-earth...");
    }

    char *response = NULL;
    char *error_msg = NULL;
    int status = ai_transform_text(&cfg, sys_prompt, input_text, &response, &error_msg);

    if (!quiet) {
        spinner_stop();
    }

    if (status != 0) {
        fprintf(stderr, "\033[1;31m[Errore]\033[0m %s\n", error_msg ? error_msg : "Richiesta fallita.");
        free(error_msg);
        free(input_text);
        free(cli_style);
        free(cli_lang);
        free(custom_prompt);
        if (free_sys_prompt) free(sys_prompt);
        config_free(&cfg);
        return 1;
    }

    if (response) {
        if (isatty(STDOUT_FILENO)) {
            printf("\n%s\n\n", response);
        } else {
            printf("%s\n", response);
        }
        free(response);
    }

    free(input_text);
    free(cli_style);
    free(cli_lang);
    free(custom_prompt);
    if (free_sys_prompt) free(sys_prompt);
    config_free(&cfg);

    return 0;
}
