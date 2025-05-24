#!/bin/bash

# Caminho para o diretório de testes
TEST_DIR="tests/inputs"
OUTPUT_DIR="tests/outputs"
VERBOSE=""
PRINT_STDOUT=false

# Verifica as flags passadas
while [[ $# -gt 0 ]]; do
  case "$1" in
    -v)
      VERBOSE="-v"
      shift
      ;;
    -o)
      PRINT_STDOUT=true
      shift
      ;;
    *)
      echo "Uso: $0 [-v] [-o]"
      exit 1
      ;;
  esac
done

# Verifica se o executável existe e é executável
if [[ ! -x ./malha ]]; then
  echo "Erro: o executável ./malha não foi encontrado ou não tem permissão de execução."
  exit 1
fi

# Cria a pasta de saída se necessário
mkdir -p "$OUTPUT_DIR"

# Loop por todos os arquivos .in na pasta de testes
for test_file in "$TEST_DIR"/*.in; do
  test_name=$(basename "$test_file" .in)
  output_file="$OUTPUT_DIR/${test_name}.out"

  echo "Executando teste: $test_file"

  if $PRINT_STDOUT; then
    ./malha $VERBOSE < "$test_file"
  else
    ./malha $VERBOSE < "$test_file" > "$output_file"
    echo "Saída escrita em: $output_file"
  fi

  echo "-----------------------------"
done
