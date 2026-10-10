#!/usr/bin/env bash
set -e

# Colors
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${CYAN}===================================================${NC}"
echo -e "${CYAN}        Tolkienizer - Middle-earth CLI Installer    ${NC}"
echo -e "${CYAN}===================================================${NC}"

# Check compiler
if ! command -v clang >/dev/null 2>&1 && ! command -v gcc >/dev/null 2>&1; then
    echo -e "${RED}Errore: Nessun compilatore C trovato (clang o gcc).${NC}"
    exit 1
fi

# Detect OS and recommend packages if dependencies missing
echo -e "\n${YELLOW}[1/3] Verifica dipendenze (libcurl, cjson)...${NC}"
MISSING=0

if ! command -v curl-config >/dev/null 2>&1 && ! pkg-config --exists libcurl 2>/dev/null; then
    echo -e "${RED}Attenzione: libcurl non trovato.${NC}"
    MISSING=1
fi

if ! pkg-config --exists libcjson 2>/dev/null && [ ! -d "/opt/homebrew/include/cjson" ] && [ ! -d "/usr/local/include/cjson" ]; then
    echo -e "${RED}Attenzione: libcjson non trovato.${NC}"
    MISSING=1
fi

if [ "$MISSING" -eq 1 ]; then
    echo -e "\nPer installare le dipendenze mancanti:"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo -e "  ${GREEN}brew install curl cjson pkg-config${NC}"
    elif command -v apt-get >/dev/null 2>&1; then
        echo -e "  ${GREEN}sudo apt update && sudo apt install -y build-essential libcurl4-openssl-dev libcjson-dev pkg-config${NC}"
    elif command -v dnf >/dev/null 2>&1; then
        echo -e "  ${GREEN}sudo dnf install -y libcurl-devel cjson-devel gcc make pkgconfig${NC}"
    elif command -v pacman >/dev/null 2>&1; then
        echo -e "  ${GREEN}sudo pacman -S --needed curl cjson gcc make pkgconf${NC}"
    fi
    echo ""
    read -p "Vuoi tentare la compilazione comunque? [s/N] " proceed
    if [[ ! "$proceed" =~ ^[sSyY]$ ]]; then
        exit 1
    fi
fi

# Build
echo -e "\n${YELLOW}[2/3] Compilazione di tolkienizer...${NC}"
make clean
make

# Installation destination
echo -e "\n${YELLOW}[3/3] Installazione binario...${NC}"
INSTALL_DIR="$HOME/.local/bin"

if [ -w "/usr/local/bin" ] && [ "$EUID" -eq 0 ]; then
    INSTALL_DIR="/usr/local/bin"
fi

mkdir -p "$INSTALL_DIR"
cp tolkienizer "$INSTALL_DIR/tolkienizer"
chmod 755 "$INSTALL_DIR/tolkienizer"

echo -e "\n${GREEN}✓ tolkienizer installato con successo in: $INSTALL_DIR/tolkienizer${NC}"

# Check if INSTALL_DIR is in PATH
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo -e "${YELLOW}Nota: $INSTALL_DIR non sembra essere nel tuo PATH.${NC}"
    echo -e "Aggiungi questa riga al tuo ~/.zshrc o ~/.bashrc:"
    echo -e "  ${CYAN}export PATH=\"$INSTALL_DIR:\$PATH\"${NC}"
fi

echo -e "\n${CYAN}Puoi iniziare subito eseguendo:${NC}"
echo -e "  ${GREEN}tolkienizer --config${NC}     # Wizard per configurare la tua chiave API"
echo -e "  ${GREEN}tolkienizer --help${NC}       # Guida all'uso"
