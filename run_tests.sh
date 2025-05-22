#!/bin/bash

# Caminho para o diretório de testes
TEST_DIR="tests"
VERBOSE=""

# Verifica se a flag -v foi passada
if [[ "$1" == "-v" ]]; then
  VERBOSE="-v"
fi

# Verifica se o executável existe e é executável
if [[ ! -x ./malha ]]; then
  echo "Erro: o executável ./malha não foi encontrado ou não tem permissão de execução."
  exit 1
fi

# Loop por todos os arquivos .in na pasta de testes
for test_file in "$TEST_DIR"/*.in; do
  echo "Executando teste: $test_file"
  ./malha $VERBOSE < "$test_file"
  echo "-----------------------------"
done
