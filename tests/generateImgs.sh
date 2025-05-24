#!/bin/bash

SCRIPT="printFigure.py"
DIR="inputs/"

# Cria o diretório de saída de imagens, se não existir
mkdir -p .. /imgs

# Itera sobre os arquivos .in de forma segura
while IFS= read -r -d '' input_file; do
  echo "Generating image for: $input_file"
  python3 "$SCRIPT" "$input_file"
done < <(find "$DIR" -name "*.in" -print0)