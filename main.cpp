/***********************************************************************
 *
 * Autor: Richard Fernando Heise Ferreira
 * Matrícula: 201900121214
 * Data: 03/2025
 * Instituição: Universidade Federal do Paraná
 * Curso: Mestrado em Segurança da Computação - PPG-Inf
 * Motivo: Trabalho 2 da disciplina de Geometria Computacional
 *
 ************************************************************************/
#include "dcel.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
  bool verbose = false;
  // Verifica se é modo verbose para debug
  for (int i = 1; i < argc; ++i) {
    if (string(argv[i]) == "-v") {
      verbose = true;
      break;
    }
  }

  int n, f;
  cin >> n >> f;
  vector<pair<int, int>> vertices(n);

  // Lê os vértices
  for (int i = 0; i < n; ++i) {
    int x, y;
    cin >> x >> y;
    vertices[i] = {x, y};
  }

  // Lê as faces
  vector<vector<int>> faces(f);
  for (int i = 0; i < f; ++i) {
    int v;
    faces[i] = vector<int>();
    string line;
    getline(cin >> ws, line); // Lê a linha inteira ignorando whitespace anterior
    istringstream iss(line);
    while (iss >> v) {
      faces[i].push_back(v-1);
    }
  }

  if (verbose) {
    debugPrintMesh(vertices, faces);
  }

  // Verifica a validade da malha
  string errorMessage;
  bool isValid = checkMesh(vertices, faces, errorMessage);
  
  if (!isValid) {
    // Se a malha não for válida, imprime a mensagem de erro e termina
    cout << errorMessage << endl;
    return 1;
  }

  try {
    // Constrói a DCEL, pois a malha é válida
    DCEL dcel;
    dcel.buildFromMesh(vertices, faces);
    
    // Gera a saída no formato requerido
    dcel.printDCELOutput();
    
  } catch (const exception& e) {
    // Em caso de erro na construção da DCEL
    if (verbose) {
      cerr << "Erro ao construir DCEL: " << e.what() << endl;
    }
    return 1;
  }
  
  return 0;
}