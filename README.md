# 📜 tolkienizer

> **Transmute ordinary words into the mythic prose of J.R.R. Tolkien.**

`tolkienizer` è un programma da linea di comando scritto in C ad alte prestazioni che prende in input qualsiasi testo (messaggi quotidiani, email, log di errore, commit message, note tecniche) e lo trasforma in prosa epica di alta fantasia nello stile inconfondibile di **J.R.R. Tolkien** (*Il Signore degli Anelli*, *Il Silmarillion*, *Lo Hobbit*).

---

## ✨ Funzionalità

- **Linguaggio C nativo**: Veloce, leggero, senza runtime pesanti, basato su `libcurl` e `cJSON`.
- **Flessibilità di input**:
  - Da argomenti della riga di comando: `tolkienizer "Il server di produzione è andato in crash"`
  - Da pipeline / standard input: `git diff | tolkienizer`
  - Da file con flag dedicata: `tolkienizer -f commit_message.txt`
  - Modalità interattiva (se lanciato senza argomenti da terminale).
- **Stili narrativi multipli**:
  - `epic` *(default)*: Solenne e grandiosa prosa epica della Prima e Terza Era di Arda.
  - `hobbit`: Tono caldo, accogliente e bucolico della Contea tra seconde colazioni ed erba-pipa.
  - `elvish`: Melodica, malinconica ed elevata prosa elfica sotto la luce delle stelle di Elbereth.
  - `mordor`: Oscura, aspra e pesante prosa d'ombra, ferro e fuoco di Barad-dûr.
- **Supporto multi-lingua nativo**:
  - **Inglese** (`en`, default): La prosa epica inconfondibile di J.R.R. Tolkien.
  - **Italiano** (`it`): Italiano aulico ed epico tolkieniano (*Alliata / Fatica*).
  - **Spagnolo** (`es`): Castellano solemne y épico.
  - **Francese** (`fr`): Haut français noble et épique.
  - **Tedesco** (`de`): Edles, archaisches Hochdeutsch der Vorzeit.
  - **Auto** (`auto`): Rileva e preserva automaticamente la lingua del testo originale.
  - Qualsiasi altra lingua specificabile tramite codice o nome (es. `-l ja`, `-l pt`, `-l la`).
- **Gestione sicura delle chiavi API (Nessuna chiave hardcoded)**:
  - Configurazione guidata interattiva con `tolkienizer --config`.
  - Salvataggio sicuro in `~/.config/tolkienizer/config.json` con permessi ristretti `0600`.
  - Supporto per variabili d'ambiente standard (`OPENAI_API_KEY`, `GROQ_API_KEY`, `TOLKIENIZER_API_KEY`).
  - Flag da terminale `-k / --key` per esecuzioni una tantum o script CI/CD.
- **Supporto multi-provider**:
  - **OpenAI** (default: `gpt-4o-mini`, `gpt-4o`)
  - **Groq** (`llama-3.3-70b-versatile`, ultra-veloce)
  - **Google Gemini** (endpoint compatibile OpenAI: `gemini-2.0-flash`)
  - **OpenRouter** (qualsiasi modello)
  - **Ollama** (locale, nessun costo e nessuna chiave API richiesta: `http://localhost:11434/v1`)
  - Endpoint custom compatibile OpenAI

---

## 🚀 Installazione

### Metodo 1: Tramite Homebrew (Consigliato per macOS e Linux)

Il modo più rapido per installare e mantenere aggiornato `tolkienizer`:

```bash
# Aggiungi il tap e installa
brew tap nnevskij/tap
brew install tolkienizer

# Oppure installa direttamente in un unico passaggio:
brew install nnevskij/tap/tolkienizer
```

---

### Metodo 2: Compilazione da sorgente

#### Prerequisiti
Assicurati di avere installato un compilatore C, `libcurl` e `cJSON`:

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

### 2. Compilazione e installazione

Clona la repository ed esegui:

```bash
# Script automatico:
./install.sh

# Oppure tramite Makefile standard:
make
make install
```

> **Nota sul percorso:** Di default `make install` installa il binario in `~/.local/bin/tolkienizer` (se eseguito come utente normale) oppure in `/usr/local/bin/tolkienizer` (se eseguito con `sudo`). Assicurati che `~/.local/bin` sia presente nella tua variabile `$PATH` (su macOS e molte distro Linux lo è già).

Verifica l'installazione:
```bash
tolkienizer --version
```

---

## 🔑 Configurazione delle chiavi API

All'installazione, `tolkienizer` non contiene alcuna chiave API. Puoi configurare la tua chiave in uno di questi modi:

### Metodo 1: Wizard interattivo (consigliato)
```bash
tolkienizer --config
```
Il programma ti guiderà nella scelta del provider (OpenAI, Groq, Gemini, OpenRouter, Ollama locale) e salverà la configurazione in `~/.config/tolkienizer/config.json`.

### Metodo 2: Salvataggio diretto da CLI
```bash
# Salva la chiave API
tolkienizer --set-key "sk-proj-..."

# Opzionale: imposta modello o provider diverso
tolkienizer --set-model "gpt-4o"
tolkienizer --set-url "https://api.groq.com/openai/v1"
```

### Metodo 3: Variabili d'ambiente
Puoi esportare la chiave nella tua sessione o nel tuo file `~/.zshrc` / `~/.bashrc`:
```bash
export OPENAI_API_KEY="sk-proj-..."
# oppure
export TOLKIENIZER_API_KEY="sk-..."
```

### Metodo 4: Parametro singolo da linea di comando
```bash
tolkienizer -k "sk-proj-..." "Testo da trasformare"
```

Per verificare la configurazione attiva in qualsiasi momento:
```bash
tolkienizer --show-config
```

---

## 📖 Esempi di utilizzo

### 1. Selezione della lingua (`-l, --lang`)
```bash
# Inglese (predefinito: prosa epica tolkieniana originale)
tolkienizer "The database server crashed due to out of memory error"

# Italiano (italiano aulico tolkieniano)
tolkienizer -l it "Oggi il server di database è andato in crash per mancanza di memoria."

# Spagnolo (castellano solemne y épico)
tolkienizer -l es "Tenemos que posponer la reunión de mañana por la tarde."

# Francese (haut français noble et épique)
tolkienizer -l fr "Le déploiement de la mise à jour a réussi sans aucune erreur."

# Tedesco (edles, archaisches Hochdeutsch)
tolkienizer -l de "Wir müssen die Festung vor Anbruch der Nacht sichern."

# Auto (rileva e preserva automaticamente la lingua del testo fornito)
tolkienizer -l auto "Devo ricordarmi di portare a spasso il cane"
```

### 2. Diversi stili tolkieniani (`-s, --style`)
```bash
# Stile Contea / Hobbit
tolkienizer -l it -s hobbit "Devo andare a fare la spesa al supermercato per comprare latte e biscotti."

# Stile Elfico
tolkienizer -l it -s elvish "Guardo il cielo stellato mentre la notte avvolge la città silenziosa."

# Stile Mordor
tolkienizer -l it -s mordor "Il capo progetto ha anticipato la data di consegna di due settimane."
```

### 3. Pipeline Unix (`stdin`)
```bash
# Trasforma un git log
git log -1 --pretty=%B | tolkienizer

# Trasforma un commit message
echo "Fix memory leak in network driver" | tolkienizer

# Da pipe a file
echo "Il team ha superato la review di sicurezza" | tolkienizer > cronaca_epica.txt
```

### 4. Da file
```bash
tolkienizer -f appunti_riunione.txt
```

### 5. Utilizzo con Ollama (100% locale, senza connessione internet né chiavi)
Se hai [Ollama](https://ollama.ai) in esecuzione sul tuo computer:
```bash
tolkienizer -u "http://localhost:11434/v1" -m "llama3" "Abbiamo completato il rilascio in produzione."
```

---

## 🛠️ Opzioni da riga di comando

```
Uso: tolkienizer [OPZIONI] [TESTO...]

Opzioni di input:
  TESTO...                  Testo da trasformare direttamente da riga di comando
  -f, --file FILE           Legge il testo da un file
  (pipe / stdin)            Accetta testo via standard input

Opzioni di stile ed esecuzione:
  -s, --style STILE         Stile: epic (default), hobbit, elvish, mordor
  -p, --prompt TESTO        System prompt personalizzato (sovrascrive lo stile)
  -m, --model MODELLO       Modello AI (default: da config o gpt-4o-mini)
  -k, --key CHIAVE          Chiave API per questa singola esecuzione
  -u, --url URL             URL di base API (default: https://api.openai.com/v1)
  -t, --temp FLOAT          Temperatura di generazione (0.0 - 2.0, default: 0.7)
  -q, --quiet               Modalità silenziosa (disattiva animazione di attesa)
      --list-styles         Elenca tutti gli stili disponibili
  -v, --version             Mostra la versione del programma
  -h, --help                Mostra la guida

Gestione configurazione:
      --config              Avvia il wizard interattivo di configurazione
      --set-key CHIAVE      Salva la chiave API nel file di configurazione
      --set-model MODELLO   Salva il modello predefinito
      --set-url URL         Salva l'endpoint API
      --set-style STILE     Salva lo stile predefinito
      --show-config         Mostra le impostazioni attuali
```

---

## 🏛️ Struttura del progetto

```
tolkienizer/
├── Makefile            # Build system con target all, install, uninstall, clean
├── install.sh          # Script di installazione interattivo con controllo dipendenze
├── README.md           # Documentazione completa
├── .gitignore          # File esclusi da Git
├── include/
│   ├── ai_client.h     # Client HTTP/AI basato su libcurl
│   ├── config.h        # Gestione configurazione ~/.config/tolkienizer/config.json
│   ├── prompts.h       # Pre-prompt letterari tolkieniani (epic, hobbit, elvish, mordor)
│   └── utils.h         # Helper stringhe, file I/O, terminale e spinner
└── src/
    ├── ai_client.c     # Implementazione richieste REST ad endpoint OpenAI-compatible
    ├── config.c        # Parsing e salvataggio JSON sicuro
    ├── main.c          # Entrypoint CLI e parsing argomenti con getopt_long
    ├── prompts.c       # Definizioni dei prompt letterari
    └── utils.c         # Implementazione utilità di sistema e thread spinner
```

---

## 📜 Licenza

Distribuito sotto licenza MIT. Libero di essere usato, modificato e distribuito.
