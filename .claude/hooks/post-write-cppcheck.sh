#!/bin/bash
# .claude/hooks/post-write-cppcheck.sh
# Hook PostToolUse — lance cppcheck automatiquement après chaque écriture
# de fichier .c ou .h dans le projet BSP STM32F410

# Lire le JSON envoyé par Claude Code sur stdin
INPUT=$(cat)

# Extraire le chemin du fichier modifié
FILE=$(echo "$INPUT" | jq -r '.tool_input.file_path // empty')

# Si aucun fichier détecté, sortir silencieusement
if [ -z "$FILE" ]; then
    exit 0
fi

# Normaliser les séparateurs Windows → Unix (Git Bash)
FILE="${FILE//\\//}"

# Ne traiter que les fichiers .c et .h
if [[ "$FILE" != *.c ]] && [[ "$FILE" != *.h ]]; then
    exit 0
fi

# Vérifier que le fichier existe réellement sur le disque
if [ ! -f "$FILE" ]; then
    exit 0
fi

# Exclure les fichiers protégés — CMSIS, HAL, vendor
if [[ "$FILE" == *"Drivers/CMSIS/"* ]] || \
   [[ "$FILE" == *"Drivers/STM32F4xx_HAL_Driver/"* ]] || \
   [[ "$FILE" == *"vendor/"* ]]; then
    exit 0
fi

# Lancer cppcheck
echo "--- cppcheck : $FILE ---" >&2

cppcheck \
    --enable=warning,style,performance \
    --std=c11 \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --template="{file}:{line}: {severity}: {message} [{id}]" \
    "$FILE" 2>&1 | head -30

EXIT_CODE=${PIPESTATUS[0]}

# Résumé
if [ $EXIT_CODE -eq 0 ]; then
    echo "--- cppcheck OK ---" >&2
else
    echo "--- cppcheck : erreurs détectées ---" >&2
fi

# Toujours exit 0 — hook informatif, ne bloque pas
exit 0