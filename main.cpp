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

  if (verbose) {
    cout << "Número de vértices: " << n << "\n";
    for (int i = 0; i < n; ++i) {
      cout << "Vértice " << i << ": (" << vertices[i].first << ", "
           << vertices[i].second << ")\n";
    }
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
      faces[i].push_back(v);
    }
  }

  if (verbose) {
    cout << "Número de faces: " << f << "\n";
    for (int i = 0; i < f; ++i) {
      cout << "Face " << i + 1 << ":";
      for (int idx : faces[i]) {
        cout << " " << idx;
      }
      cout << "\n";
    }
    
    // Imprime detalhes da malha para debug
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
  return 0;
  
  try {
    // Constrói a DCEL se a malha for válida
    DCEL dcel = buildDCEL(vertices, faces);
    
    if (verbose) {
      cout << "\n=== DCEL construída com sucesso ===\n";
      debugPrintDCEL(dcel);
    }
    
    // Gera a saída no formato requerido
    string output = dcelToString(dcel);
    cout << output;
    
  } catch (const exception& e) {
    // Em caso de erro na construção da DCEL
    if (verbose) {
      cout << "Erro ao construir DCEL: " << e.what() << endl;
    }
    cout << "não subdivisão planar" << endl;
    return 1;
  }

}