#!/usr/bin/env bash
set -e

TAG="${1:-}"
REPO="${2:-nnevskij/tolkienizer}"

if [ -z "$TAG" ]; then
    echo "Uso: $0 <tag-versione> [owner/repo]"
    echo "Esempio: $0 v1.0.0 nnevskij/tolkienizer"
    exit 1
fi

# Rimuovi eventuale 'v' iniziale per il formato numerico pulito
VERSION="${TAG#v}"
TARBALL_URL="https://github.com/${REPO}/archive/refs/tags/${TAG}.tar.gz"

echo "Scaricamento archivio release da: $TARBALL_URL"
TMP_FILE=$(mktemp)
if ! curl -sSL -f "$TARBALL_URL" -o "$TMP_FILE"; then
    echo "Errore: Impossibile scaricare $TARBALL_URL. Hai già creato e pushato il tag su GitHub?"
    rm -f "$TMP_FILE"
    exit 1
fi

if command -v shasum >/dev/null 2>&1; then
    SHA256=$(shasum -a 256 "$TMP_FILE" | awk '{print $1}')
elif command -v sha256sum >/dev/null 2>&1; then
    SHA256=$(sha256sum "$TMP_FILE" | awk '{print $1}')
else
    echo "Errore: Né shasum né sha256sum trovati."
    rm -f "$TMP_FILE"
    exit 1
fi

rm -f "$TMP_FILE"

echo "SHA256 calcolato: $SHA256"

FORMULA_FILE="Formula/tolkienizer.rb"

if [ ! -f "$FORMULA_FILE" ]; then
    echo "Errore: $FORMULA_FILE non trovato."
    exit 1
fi

# Aggiorna URL e SHA256 nella Formula
sed -i '' "s|url \".*\"|url \"$TARBALL_URL\"|g" "$FORMULA_FILE" 2>/dev/null || sed -i "s|url \".*\"|url \"$TARBALL_URL\"|g" "$FORMULA_FILE"
sed -i '' "s|sha256 \".*\"|sha256 \"$SHA256\"|g" "$FORMULA_FILE" 2>/dev/null || sed -i "s|sha256 \".*\"|sha256 \"$SHA256\"|g" "$FORMULA_FILE"

echo "✓ $FORMULA_FILE aggiornato con successo per la versione $TAG!"
echo "Puoi verificare la formula con git diff e committarla."
