#!/bin/bash

# Caminho para os diretórios
OUTPUT_DIR="../outputs"
VALID_DIR="valid"

# Cria a pasta de saída de imagens, se necessário
mkdir -p "$VALID_DIR"

# Loop por todos os arquivos correta*.out
for test_file in "$OUTPUT_DIR"/correta*.out; do
  test_name=$(basename "$test_file" .out)

  echo "Gerando imagem para: $test_file"
  python3 printDCEL.py "$test_file"
  echo "Imagem gerada para: $test_name"
  echo "-----------------------------"
done
