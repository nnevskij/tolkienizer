```
  _____ ___  _     _  _____ _____ _   _ ___ _____ _____ ____  
 |_   _/ _ \| |   | |/ /_ _| ____| \ | |_ _|__  / ____|  _ \ 
   | || | | | |   | ' / | ||  _| |  \| || |  / /|  _| | |_) |
   | || |_| | |___| . \ | || |___| |\  || | / /_| |___|  _ < 
   |_| \___/|_____|_|\_\___|_____|_| \_|___/____|_____|_| \_\ 

 Transmute Ordinary Words into the Mythic Prose of J.R.R. Tolkien
```

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language: C99](https://img.shields.io/badge/Language-C99-00599C.svg)](https://en.wikipedia.org/wiki/C99)
[![Homebrew](https://img.shields.io/badge/Homebrew-Tap-orange.svg)](#homebrew-macos--linux)
[![Platforms](https://img.shields.io/badge/Platforms-macOS%20%7C%20Linux-lightgrey.svg)](#installation)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://github.com/nnevskij/tolkienizer/actions)

---

## Overview

**tolkienizer** is a high-performance command-line utility written in native C that transforms everyday text (emails, commit messages, error logs, technical notes, or chat messages) into the majestic, epic high-fantasy prose of **J.R.R. Tolkien** (*The Lord of the Rings*, *The Silmarillion*, *The Hobbit*).

Powered by modern Large Language Models via any OpenAI-compatible API, `tolkienizer` brings the elevated cadence, solemn gravity, and mythic wonder of Middle-earth directly to your terminal.

---

## Features

- **Native C Implementation**: Built in clean C99 with `libcurl` and `cJSON`. Fast startup, low overhead, and no heavy runtimes.
- **Native Multi-Language Support**:
  - **English** (`en`, default): J.R.R. Tolkien's authentic high-fantasy literary register.
  - **Italian** (`it`): Solemn and archaic Italian reminiscent of classical Tolkien translations.
  - **Spanish** (`es`): Solemn, epic Castilian prose.
  - **French** (`fr`): Noble, archaic high French literature.
  - **German** (`de`): Noble, archaic High German of elder days.
  - **Auto-Detect** (`auto`): Detects and preserves the original input language while elevating it to epic fantasy prose.
  - **Custom Language**: Specify any language code or name (e.g. `-l ja`, `-l pt`, `-l la`).
- **Four Atmospheric Tolkien Styles**:
  - `epic` (default): Solemn, mythic high-fantasy chronicles of the First and Third Ages of Arda.
  - `hobbit`: Cozy, pastoral, comfortable Shire narrative with halfling good sense and rustic humor.
  - `elvish`: Melodic, lyrical, starlit elegance and sorrow of the Firstborn Eldar.
  - `mordor`: Harsh, grim, commanding decrees of cold iron, ash, and the shadow of Barad-dur.
  - Custom override: Provide your own system prompt with `-p, --prompt`.
- **Flexible Input Modes**:
  - Positional CLI arguments: `tolkienizer "The database server crashed"`
  - Unix pipelines / `stdin`: `git diff | tolkienizer`
  - File input: `tolkienizer -f notes.txt`
  - Interactive multiline input when launched without arguments.
- **Secure API Key Management (Zero Hardcoded Keys)**:
  - Interactive setup wizard via `tolkienizer --config`.
  - Secure configuration stored in `~/.config/tolkienizer/config.json` with strict `0600` file permissions.
  - Standard environment variable support (`OPENAI_API_KEY`, `TOLKIENIZER_API_KEY`, `GROQ_API_KEY`, etc.).
  - One-off CLI flag (`-k, --key`) for isolated runs or CI/CD pipelines.
- **Multi-Provider Support**:
  - **OpenAI** (default: `gpt-4o-mini`, `gpt-4o`)
  - **Groq** (`llama-3.3-70b-versatile`, ultra-fast)
  - **Google Gemini** (via OpenAI-compatible endpoint: `gemini-2.0-flash`)
  - **OpenRouter** (any model)
  - **Ollama** (100% offline, local, free: `http://localhost:11434/v1`)
  - Any custom OpenAI-compatible endpoint.

---

## Installation

### Homebrew (macOS & Linux)

The easiest way to install and maintain `tolkienizer`:

```bash
# Add tap and install
brew tap nnevskij/tap
brew install tolkienizer

# Or in a single command
brew install nnevskij/tap/tolkienizer
```

To upgrade in the future:
```bash
brew update && brew upgrade tolkienizer
```

---

### Building from Source

#### Prerequisites

Ensure you have a C compiler (`clang` or `gcc`), `libcurl`, and `cJSON` installed:

- **macOS** (Homebrew):
  ```bash
  brew install curl cjson pkg-config
  ```

- **Ubuntu / Debian**:
  ```bash
  sudo apt update
  sudo apt install -y build-essential libcurl4-openssl-dev libcjson-dev pkg-config
  ```

- **Fedora / RHEL**:
  ```bash
  sudo dnf install -y gcc make libcurl-devel cjson-devel pkgconfig
  ```

- **Arch Linux**:
  ```bash
  sudo pacman -S --needed gcc make curl cjson pkgconf
  ```

#### Build and Install

```bash
git clone https://github.com/nnevskij/tolkienizer.git
cd tolkienizer

# Option A: Interactive installer script
./install.sh

# Option B: Standard Makefile
make
make install
```

> **Note:** By default, `make install` installs the binary to `~/.local/bin/tolkienizer` (when run as a regular user) or `/usr/local/bin/tolkienizer` (when run with `sudo`). Ensure `~/.local/bin` is in your `$PATH`.

Verify the installation:
```bash
tolkienizer --version
```

---

## API Key Configuration

`tolkienizer` ships without hardcoded API keys. You can configure your credentials using any of the following methods:

### Method 1: Interactive Wizard (Recommended)

```bash
tolkienizer --config
```

The setup wizard will prompt you to choose:
1. Default language (English, Italian, Spanish, French, German, Auto)
2. AI Provider (OpenAI, Groq, Google Gemini, OpenRouter, local Ollama, or Custom)
3. API Key (masked during typing)

Settings are securely written to `~/.config/tolkienizer/config.json`.

### Method 2: Direct CLI Setters

```bash
# Save API key
tolkienizer --set-key "sk-proj-..."

# Optional: set default model, language, or endpoint
tolkienizer --set-lang "en"
tolkienizer --set-model "gpt-4o"
tolkienizer --set-url "https://api.groq.com/openai/v1"
tolkienizer --set-style "epic"
```

### Method 3: Environment Variables

Export the key in your current shell or add it to `~/.zshrc` / `~/.bashrc`:

```bash
export OPENAI_API_KEY="sk-proj-..."
# or
export TOLKIENIZER_API_KEY="sk-..."
```

### Method 4: Per-Invocation Flag

Pass the key directly for a single command:

```bash
tolkienizer -k "sk-proj-..." "Text to transform"
```

View active configuration anytime:
```bash
tolkienizer --show-config
```

---

## Usage Examples

### 1. Language Selection (`-l, --lang`)

```bash
# English (default: authentic Tolkien high fantasy)
tolkienizer "The database server crashed due to memory exhaustion"

# Italian (archaic, solemn literary Italian)
tolkienizer -l it "Oggi il server di database e andato in crash per mancanza di memoria."

# Spanish (solemn and epic Castilian)
tolkienizer -l es "Tenemos que posponer la reunion de manana por la tarde."

# French (noble and epic high French)
tolkienizer -l fr "Le deploiement de la mise a jour a reussi sans aucune erreur."

# German (noble, archaic High German)
tolkienizer -l de "Wir muessen die Festung vor Anbruch der Nacht sichern."

# Auto-detect (preserves and elevates the input language)
tolkienizer -l auto "I must remember to walk the dog before sunset"
```

### 2. Narrative Styles (`-s, --style`)

```bash
# Shire / Hobbit style
tolkienizer -s hobbit "I have to go to the grocery store to buy tea and butter."

# Elven Havens style
tolkienizer -s elvish "I watch the stars as the quiet night falls over the city."

# Shadow of Mordor style
tolkienizer -s mordor "The project manager moved the deadline forward by two weeks."
```

### 3. Unix Pipelines (`stdin`)

```bash
# Transform git commits
git log -1 --pretty=%B | tolkienizer

# Transform deployment logs
echo "Build succeeded, deploy artifacts pushed to production" | tolkienizer

# Pipe output to a file or mail
echo "Security audit passed with zero vulnerabilities" | tolkienizer > chronicle.txt
```

### 4. Reading from a File

```bash
tolkienizer -f meeting_notes.txt
```

### 5. Offline & Local AI via Ollama

If you run [Ollama](https://ollama.ai) locally:

```bash
tolkienizer -u "http://localhost:11434/v1" -m "llama3" "We completed the software deployment."
```

---

## Command Line Reference

```
Usage: tolkienizer [OPTIONS] [TEXT...]

Transform any text into high-fantasy prose in the style of J.R.R. Tolkien.

Input Options:
  TEXT...                   Text to transform directly from arguments
  -f, --file PATH           Read text from a file
  (pipe / stdin)            Accept text via standard input

Language & Style:
  -l, --lang LANG           Target language: en (default), it, es, fr, de, auto (or custom)
  -s, --style STYLE         Tolkien style: epic (default), hobbit, elvish, mordor
  -p, --prompt TEXT         Custom system prompt (overrides style and language)
      --list-langs          List all supported languages
      --list-styles         List all narrative styles

Execution Options:
  -m, --model MODEL         AI model name (default from config or gpt-4o-mini)
  -k, --key KEY             API key for this single execution
  -u, --url URL             API base URL (default: https://api.openai.com/v1)
  -t, --temp FLOAT          Sampling temperature (0.0 - 2.0, default: 0.7)
  -q, --quiet               Quiet mode (suppress spinner and status messages)
  -v, --version             Show version information
  -h, --help                Show this help message

Configuration Management:
      --config              Launch interactive configuration wizard
      --set-key KEY         Save API key to configuration file
      --set-lang LANG       Save default language (en, it, es, fr, de, auto)
      --set-model MODEL     Save default model name
      --set-url URL         Save default API endpoint URL
      --set-style STYLE     Save default narrative style
      --show-config         Display current active configuration
```

---

## Project Structure

```
tolkienizer/
├── Makefile            # Build system (targets: all, install, uninstall, clean)
├── install.sh          # Interactive installation script with dependency check
├── README.md           # Documentation
├── .gitignore          # Git exclusion rules
├── Formula/
│   └── tolkienizer.rb  # Official Homebrew formula
├── scripts/
│   └── update-brew-formula.sh # Release SHA-256 calculation utility
├── .github/
│   └── workflows/
│       └── ci.yml      # GitHub Actions CI matrix (macOS & Ubuntu)
├── include/
│   ├── ai_client.h     # HTTP/REST AI completions client (libcurl)
│   ├── config.h        # Configuration manager (~/.config/tolkienizer/config.json)
│   ├── prompts.h       # Multi-language Tolkien prompts (epic, hobbit, elvish, mordor)
│   └── utils.h         # Terminal, termios masking, string, file I/O and spinner
└── src/
    ├── ai_client.c     # libcurl HTTP client and cJSON response parser
    ├── config.c        # JSON config loader, saver (chmod 0600), and wizard
    ├── main.c          # CLI entry point and getopt_long argument parser
    ├── prompts.c       # Narrative prompt definitions and language normalizer
    └── utils.c         # System utilities and background spinner thread
```

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
