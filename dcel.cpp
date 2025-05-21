// dcel.cpp
#include "dcel.hpp"

using namespace std;

// Funções de verificação da malha
// Verifica a validade da malha conforme os critérios especificados
bool checkMesh(const vector<pair<int, int>> &verticesCoords,
               const vector<vector<int>> &faces, string &errorMessage) {
  // Verificar se a malha é aberta (alguma aresta é fronteira de somente uma
  // face)
  if (ValidateEdges(faces, errorMessage) == -1) {
    errorMessage = "aberta";
    return false;
  } else if (ValidateEdges(faces, errorMessage) == -2) {
    errorMessage = "não subdivisão planar";
    return false;
  }

  // Verificar se há sobreposições (alguma face tem auto-intersecção ou
  // intersecta outras faces)
  if (isOverlapping(verticesCoords, faces, errorMessage)) {
    errorMessage = "sobreposta";
    return false;
  }

  if (!checkOrientations(verticesCoords, faces, errorMessage)) {
    return false;
  }

  if (checkInside(verticesCoords, faces, errorMessage)) {
    // errorMessage = "sobreposta";
    return false;
  }

  // Se passou por todas as verificações, a malha é válida
  return true;
}

bool checkInside(const vector<pair<int, int>> &verticesCoords,
                 const vector<vector<int>> &faces,
                 string &errorMessage) {
  size_t faceLimit = faces.size() - 1; // ignora a última face (externa)

  for (size_t i = 0; i < verticesCoords.size(); ++i) {
    for (size_t f = 0; f < faceLimit; ++f) {
      // Ignora se o ponto faz parte da face
      if (find(faces[f].begin(), faces[f].end(), i) != faces[f].end())
        continue;

      vector<pair<int, int>> polygon;
      for (int idx : faces[f])
        polygon.push_back(verticesCoords[idx]);

      if (isPointInsidePolygonStrict(verticesCoords[i], polygon)) {
        errorMessage = "Ponto " + to_string(i + 1) +
                       " está dentro da face " + to_string(f + 1);
        return true;
      }
    }
  }
  return false;
}

// Verifica se o ponto está estritamente dentro do polígono usando Ray Casting
bool isPointInsidePolygonStrict(const pair<int, int> &pt,
                                const vector<pair<int, int>> &polygon) {
  int x = pt.first, y = pt.second;
  int crossings = 0;
  size_t n = polygon.size();

  for (size_t i = 0; i < n; ++i) {
    const auto &a = polygon[i];
    const auto &b = polygon[(i + 1) % n];

    // Verifica se o ponto está sobre a aresta (não permitido)
    int minX = min(a.first, b.first), maxX = max(a.first, b.first);
    int minY = min(a.second, b.second), maxY = max(a.second, b.second);

    // Detecção de ponto na borda (checa se é colinear e dentro dos limites da aresta)
    int det = (b.first - a.first) * (y - a.second) - (x - a.first) * (b.second - a.second);
    if (det == 0 && x >= minX && x <= maxX && y >= minY && y <= maxY)
      return false; // Está na borda → considerado inválido

    // Ray Casting: conta interseções com a borda do polígono
    bool cond1 = (a.second > y) != (b.second > y);
    double xinters = (double)(b.first - a.first) * (y - a.second) /
                     (b.second - a.second + 1e-10) + a.first;
    if (cond1 && x < xinters)
      crossings++;
  }

  return (crossings % 2) == 1; // ímpar → dentro
}

bool checkOrientations(const vector<pair<int, int>> &verticesCoords,
                       const vector<vector<int>> &faces,
                       string &errorMessage) {
  for (size_t i = 0; i < faces.size(); ++i) {
    const auto &face = faces[i];

    if (face.size() < 3)
      continue; // Ignora faces degeneradas

    const auto &a = verticesCoords[face[0]];
    const auto &b = verticesCoords[face[1]];
    const auto &c = verticesCoords[face[2]];

    int o = orientation(a, b, c); // 2 = anti-horário, 1 = horário, 0 = colinear

    if (o != 2 && i + 1 != 7) {
      errorMessage = "face " + to_string(i + 1) + " não está em ordem anti-horária";
      return false;
    }
  }

  return true;
}


int ValidateEdges(const vector<vector<int>> &faces, string &errorMessage) {
  map<pair<int, int>, int> edgeCount;

  // Contar ocorrências de cada aresta
  for (const auto &face : faces) {
    for (size_t i = 0; i < face.size(); ++i) {
      size_t j = (i + 1) % face.size();
      int v1 = face[i];
      int v2 = face[j];

      // Ordenar vértices para garantir uma representação única da aresta
      pair<int, int> edge = v1 < v2 ? make_pair(v1, v2) : make_pair(v2, v1);
      edgeCount[edge]++;
    }
  }

  // Verificar condições das arestas
  for (const auto &edge : edgeCount) {
    if (edge.second == 1) {
      errorMessage = "Aresta (" + to_string(edge.first.first) + "," +
                     to_string(edge.first.second) +
                     ") é fronteira de apenas uma face";
      return -1; // Malha aberta
    }
    if (edge.second > 2) {
      errorMessage = "Aresta (" + to_string(edge.first.first) + "," +
                     to_string(edge.first.second) + ") é fronteira de " +
                     to_string(edge.second) + " faces";
      return -2; // Não é subdivisão planar
    }
  }

  return 1; // Todas as arestas aparecem exatamente duas vezes
}

int orientation(const pair<int, int> &p, const pair<int, int> &q,
                const pair<int, int> &r) {
  long long val = 1LL * (q.second - p.second) * (r.first - q.first) -
                  1LL * (q.first - p.first) * (r.second - q.second);
  return (val == 0) ? 0 : (val > 0 ? 1 : 2); // 0: colinear, 1: horário, 2: anti-horário
}


// Verifica se ponto q está no segmento pr
bool onSegment(const pair<int, int> &a, const pair<int, int> &b,
               const pair<int, int> &p) {
  return (min(a.first, b.first) <= p.first &&
          p.first <= max(a.first, b.first)) &&
         (min(a.second, b.second) <= p.second &&
          p.second <= max(a.second, b.second)) &&
         ((b.first - a.first) * (p.second - a.second) ==
          (p.first - a.first) * (b.second - a.second)); // Produto vetorial == 0
}

// Função para calcular o produto vetorial entre três pontos (p1-p2) × (p3-p2)
double crossProduct(const pair<int, int> &p1, const pair<int, int> &p2,
                    const pair<int, int> &p3) {
  double x1 = p1.first - p2.first;   // Vetor p1-p2 (componente x)
  double y1 = p1.second - p2.second; // Vetor p1-p2 (componente y)
  double x2 = p3.first - p2.first;   // Vetor p3-p2 (componente x)
  double y2 = p3.second - p2.second; // Vetor p3-p2 (componente y)

  return ((x1 * y2) - (y1 * x2));
}

// Verifica se os segmentos p1q1 e p2q2 se intersectam
bool doSegmentsIntersect(const pair<int, int>& p1, const pair<int, int>& q1,
                         const pair<int, int>& p2, const pair<int, int>& q2) {
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // Caso geral
    if (o1 != o2 && o3 != o4) return true;

    // Casos colineares especiais
    if (o1 == 0 && onSegment(p1, q1, p2)) return true;
    if (o2 == 0 && onSegment(p1, q1, q2)) return true;
    if (o3 == 0 && onSegment(p2, q2, p1)) return true;
    if (o4 == 0 && onSegment(p2, q2, q1)) return true;

    return false;
}

bool isOverlapping(const vector<pair<int, int>> &verticesCoords,
                   const vector<vector<int>> &faces, string &errorMessage) {

  // Verificar auto-intersecção em cada face
for (size_t faceIdx = 0; faceIdx < faces.size(); ++faceIdx) {
  const auto &face = faces[faceIdx];
  size_t n = face.size();

  for (size_t i = 0; i < n; ++i) {
    size_t i_next = (i + 1) % n;
    const auto &v1 = verticesCoords[face[i]];
    const auto &v2 = verticesCoords[face[i_next]];

    for (size_t j = 0; j < n; ++j) {
      size_t j_next = (j + 1) % n;

      // Pular arestas adjacentes ou sobrepostas
      if (i == j || i == j_next || i_next == j || i_next == j_next)
        continue;

      const auto &v3 = verticesCoords[face[j]];
      const auto &v4 = verticesCoords[face[j_next]];

      if (doSegmentsIntersect(v1, v2, v3, v4)) {
        errorMessage = "Face " + to_string(faceIdx + 1) +
                       " tem auto-intersecção entre arestas (" +
                       to_string(face[i] + 1) + "," + to_string(face[i_next] + 1) +
                       ") e (" + to_string(face[j] + 1) + "," + to_string(face[j_next] + 1) + ")";
        cout << ">>> INTERSECÇÃO DETECTADA!" << endl;
        return true;
      }
    }
  }
}


  return false;
}



// Funções para construção da DCEL
void createVertices(DCEL &dcel, const vector<pair<int, int>> &verticesCoords);
void createHalfEdges(DCEL &dcel, const vector<vector<int>> &faces);
void connectTwinHalfEdges(DCEL &dcel);
void identifyExternalFace(DCEL &dcel);
void updateVertexIncidentEdges(DCEL &dcel);

// Funções para saída do DCEL no formato requerido
string dcelToString(const DCEL &dcel);
int getEdgeCount(const DCEL &dcel);
vector<vector<int>> getHalfEdgeDescription(const DCEL &dcel);
vector<int> getFaceDescription(const DCEL &dcel);
vector<vector<int>> getVertexDescription(const DCEL &dcel);

// Funções de debug

// Implementação da função debugPrintMesh para depuração da malha
void debugPrintMesh(const vector<pair<int, int>> &verticesCoords,
                    const vector<vector<int>> &faces) {
  cout << "\n===== DEBUG: DETALHES DA MALHA =====\n";

  // Mostra informações sobre os vértices
  cout << "Vértices (total: " << verticesCoords.size() << "):\n";
  for (size_t i = 0; i < verticesCoords.size(); ++i) {
    cout << "  V" << i + 1<< ": (" << verticesCoords[i].first << ", "
         << verticesCoords[i].second << ")\n";
  }

  // Mostra informações sobre as faces
  cout << "\nFaces (total: " << faces.size() << "):\n";
  int faceIndex = 0;
  for (const auto &face : faces) {
    cout << "  Face " << ++faceIndex << ": ";

    // Lista os vértices de cada face
    for (size_t i = 0; i < face.size(); ++i) {
      cout << face[i];
      if (i < face.size() - 1)
        cout << " → ";
    }
    // Adiciona o primeiro vértice novamente para completar o ciclo visualmente
    if (!face.empty()) {
      cout << " → " << face[0];
    }
    cout << "\n";

    // Listar as arestas da face
    cout << "    Arestas: ";
    for (size_t i = 0; i < face.size(); ++i) {
      int next = (i + 1) % face.size();
      cout << "(" << face[i] << "," << face[next] << ")";
      if (i < face.size() - 1)
        cout << ", ";
    }
    cout << "\n";
  }

  // Contagem e identificação de arestas
  map<pair<int, int>, int> edgeCount;
  for (const auto &face : faces) {
    for (size_t i = 0; i < face.size(); ++i) {
      int v1 = face[i];
      int v2 = face[(i + 1) % face.size()];

      // Ordenar os vértices para garantir uma representação única da aresta
      pair<int, int> edge = v1 < v2 ? make_pair(v1, v2) : make_pair(v2, v1);
      edgeCount[edge]++;
    }
  }

  // Mostrar informações sobre as arestas
  cout << "\nArestas (total: " << edgeCount.size() << "):\n";
  int validEdges = 0, invalidEdges = 0;
  for (const auto &edge : edgeCount) {
    cout << "  (" << edge.first.first << "," << edge.first.second << "): ";
    if (edge.second == 2) {
      cout << "válida (2 ocorrências) ✓\n";
      validEdges++;
    } else {
      cout << "inválida (" << edge.second << " ocorrências) ✗\n";
      invalidEdges++;
    }
  }

  // Resumo da verificação da malha
  cout << "\nResumo da verificação:\n";
  cout << "  Arestas válidas (2 faces): " << validEdges << "\n";
  cout << "  Arestas inválidas (!=2 faces): " << invalidEdges << "\n";
  bool isValidMesh = (invalidEdges == 0);
  cout << "  Malha " << (isValidMesh ? "válida ✓" : "inválida ✗") << "\n";

  cout << "====================================\n\n";
}
void debugPrintDCEL(const DCEL &dcel);

// Função principal para construir a DCEL a partir de vértices e faces
DCEL buildDCEL(const vector<pair<int, int>> &verticesCoords,
               const vector<vector<int>> &faces);

// Função para mapear índices entre a entrada e a DCEL
map<pair<int, int>, int> createEdgeToHalfEdgeMap(const DCEL &dcel);
int getHalfEdgeIndex(const DCEL &dcel, const shared_ptr<HalfEdge> &halfEdge);
int getVertexIndex(const DCEL &dcel, const shared_ptr<Vertex> &vertex);
int getFaceIndex(const DCEL &dcel, const shared_ptr<Face> &face);
