// main.cpp
#include "dcel.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
  bool verbose = false;
  // Verifica se -v foi passada como argumento
  for (int i = 1; i < argc; ++i) {
    if (string(argv[i]) == "-v") {
      verbose = true;
      break;
    }
  }

  int n, f;
  cin >> n >> f;
  vector<pair<int, int>> vertices(n); // índice 1-based

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
    return 0;
  }

  try {
    // Constrói a DCEL se a malha for válida
    DCEL dcel;
    dcel.buildFromMesh(vertices, faces);
    
    if (verbose) {
      cout << "\n=== DCEL construída com sucesso ===\n";
      // debugPrintDCEL(dcel);
    }
    
    // Gera a saída no formato requerido
    dcel.printDCELOutput();
    
  } catch (const exception& e) {
    // Em caso de erro na construção da DCEL
    if (verbose) {
      cout << "Erro ao construir DCEL: " << e.what() << endl;
    }
    return 1;
  }
  
  return 0;
}