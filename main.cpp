#include "dcel.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int main() {
  int n, f;
  cin >> n >> f;

  vector<pair<int, int>> vertices(n + 1); // índice 1-based

  // Lê os vértices
  for (int i = 1; i <= n; ++i) {
    int x, y;
    cin >> x >> y;
    vertices[i] = {x, y};
  }

  cout << "Número de vértices: " << n << "\n";
  for (int i = 1; i <= n; ++i) {
    cout << "Vértice " << i << ": (" << vertices[i].first << ", "
         << vertices[i].second << ")\n";
  }

  // Lê as faces
  vector<vector<int>> faces(f);
  for (int i = 0; i < f; ++i) {
    int v;
    faces[i] = vector<int>();
    string line;
    getline(cin >> ws,
            line); // Lê a linha inteira ignorando whitespace anterior
    istringstream iss(line);
    while (iss >> v) {
      faces[i].push_back(v);
    }
  }

  cout << "Número de faces: " << f << "\n";
  for (int i = 0; i < f; ++i) {
    cout << "Face " << i + 1 << ":";
    for (int idx : faces[i]) {
      cout << " " << idx;
    }
    cout << "\n";
  }

  return 0;
}
