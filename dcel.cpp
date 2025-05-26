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

using namespace std;

// ======================================================================================================================= //

// Verifica a validade da malha conforme os critérios especificados
bool checkMesh(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces, string &errorMessage) {
  // Verificar se a malha é aberta (alguma aresta é fronteira de somente uma face)
  if (ValidateEdges(faces, errorMessage) == -1) {
    errorMessage = "aberta";
    return false;
  } else if (ValidateEdges(faces, errorMessage) == -2) {
    errorMessage = "não subdivisão planar";
    return false;
  }

  // Verificar se há sobreposições (auto-intersecções ou pontos internos inválidos)
  if (isOverlapping(verticesCoords, faces, errorMessage) || checkInside(verticesCoords, faces, errorMessage)) {
    errorMessage = "superposta";
    return false;
  }

  // Se passou por todas as verificações, a malha é válida
  return true;
}

// ======================================================================================================================= //

// Verifica se um ponto está de um polígono válido a partir das faces
bool checkInside(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces, string &errorMessage) {
    
    // Para cada face 
    for (size_t f = 0; f < faces.size(); ++f) {
        const auto &face = faces[f];
        
        // Construir o polígono da face atual
        vector<pair<int, int>> polygon;
        for (int idx : face) {
            polygon.push_back(verticesCoords[idx]);
        }

        // Verificar orientação e printar se for CW
        if (!isCounterClockwise(polygon)) {
            continue;
        }

        // Para todos os vértices do modelo
        for (size_t i = 0; i < verticesCoords.size(); ++i) {
            // Ignora apenas se o ponto é vértice DESTA face específica
            if (find(face.begin(), face.end(), i) != face.end()) {
                continue;
            }

            // Verificação de borda (mais rápida)
            if (isPointOnPolygonBorder(verticesCoords[i], polygon)) {
                errorMessage = "Ponto " + to_string(i + 1) +
                              " está na borda da face " + to_string(f + 1);
                return true;
            }

            // Verificação de interior
            if (isPointInsidePolygon(verticesCoords[i], polygon)) {
                errorMessage = "Ponto " + to_string(i + 1) +
                              " está dentro da face " + to_string(f + 1);
                return true;
            }
        }
    }
    
    return false;
}

// ======================================================================================================================= //

// Função auxiliar para verificar se ponto está na borda do polígono
bool isPointOnPolygonBorder(const pair<int, int> &pt, const vector<pair<int, int>> &polygon) {
    int x = pt.first, y = pt.second;
    size_t n = polygon.size();

    for (size_t i = 0; i < n; ++i) {
        const auto &a = polygon[i];
        const auto &b = polygon[(i + 1) % n];

        // Verifica se o ponto está sobre a aresta
        int minX = min(a.first, b.first), maxX = max(a.first, b.first);
        int minY = min(a.second, b.second), maxY = max(a.second, b.second);

        // Checa colinearidade e se está dentro dos limites da aresta
        int det = (b.first - a.first) * (y - a.second) - (x - a.first) * (b.second - a.second);
        
        if (det == 0 && x >= minX && x <= maxX && y >= minY && y <= maxY) {
            // Ponto está na borda
            return true;
        }
    }
    return false;
}

// ======================================================================================================================= //

// Verifica se o ponto está estritamente dentro do polígono, sem contar borda, usando ray casting
bool isPointInsidePolygon(const pair<int, int> &pt, const vector<pair<int, int>> &polygon) {
    int x = pt.first, y = pt.second;
    int crossings = 0;
    size_t n = polygon.size();

    for (size_t i = 0; i < n; ++i) {
        const auto &a = polygon[i];
        const auto &b = polygon[(i + 1) % n];

        // Ray Casting: conta interseções com a borda do polígono
        bool cond1 = (a.second > y) != (b.second > y);
        double xinters = (double)(b.first - a.first) * (y - a.second) /
                        (b.second - a.second + 1e-10) + a.first;
        if (cond1 && x < xinters) {
            crossings++;
        }
    }

    bool inside = (crossings % 2) == 1;
    return inside;
}

// ======================================================================================================================= //

// Função que valida se toda aresta passada na entrada possui uma "gêmea" para garantir malha fechada
// Também verifica se não há mais de uma aresta sobre a outra, fazendo uma possível não subdivisão planar
int ValidateEdges(const vector<vector<int>> &faces, string &errorMessage) {
  // Contador de arestas direcionadas (incluindo múltiplas ocorrências)
  map<pair<int, int>, vector<int>> edgeMap; // {aresta: [faces]}
  
  for (size_t face_idx = 0; face_idx < faces.size(); ++face_idx) {
      const auto &face = faces[face_idx];
      
      for (size_t i = 0; i < face.size(); ++i) {
          size_t j = (i + 1) % face.size();
          int v1 = face[i];
          int v2 = face[j];
          auto edge = make_pair(v1, v2);
          
          // Cria arestas a partir da entrada (ida)
          edgeMap[edge].push_back(face_idx);
      }
  }

  set<pair<int, int>> processedEdges;
  bool openMesh = false;
  bool nonPlanar = false;

  for (const auto &entry : edgeMap) {
      auto edge = entry.first;

      // Cria o vetor esperado de arestas invertidas (volta)
      auto reverseEdge = make_pair(edge.second, edge.first);

      // Pular se já processamos esse par
      if (processedEdges.count(edge) || processedEdges.count(reverseEdge)) continue;
      
      // Verificar aresta inversa
      if (!edgeMap.count(reverseEdge)) {
          errorMessage = "Aresta (" + to_string(edge.first+1) + "," +
                        to_string(edge.second+1) + ") não tem correspondente inversa";
          openMesh = true; // Aresta de ida não possui aresta de volta
          continue;
      }

      // Contar ocorrências totais (direta + inversa)
      // entry.second.size() retorna o número de faces que contêm a aresta direcionada entry.first
      int totalCount = entry.second.size() + edgeMap[reverseEdge].size();

      if (totalCount != 2) {
          errorMessage = "Aresta (" + to_string(edge.first+1) + "," +
                      to_string(edge.second+1) + ") e sua inversa aparecem " +
                      to_string(totalCount) + " vezes";
          nonPlanar = true; // Aresta de ida tem mais de um correspondente de volta
      }

      // Processadas
      processedEdges.insert(edge);
      processedEdges.insert(reverseEdge);
  }

  // Verificar duplicatas na mesma face
  for (const auto &entry : edgeMap) {
      const auto &edge = entry.first;
      const auto &face_indices = entry.second;
      
      if (face_indices.size() > 1) {
          // Verificar se aparece mais de uma vez na mesma face
          map<int, int> faceCount;
          for (int face_idx : face_indices) {
              faceCount[face_idx]++;
              if (faceCount[face_idx] > 1) {
                  errorMessage = "Aresta (" + to_string(edge.first+1) + "," +
                              to_string(edge.second+1) + ") aparece " +
                              to_string(faceCount[face_idx]) + " vezes na face " +
                              to_string(face_idx+1);
                  return -2; // Código para não-subdivisão planar
              }
          }
      }
  }

  if (nonPlanar) return -2; // Código para não-subdivisão planar
  if (openMesh) return -1; // Código para malha aberta

  return 1; // Malha fechada e é subdivisão planar
}

// ======================================================================================================================= //

// Função que retorna a orientação entre duas arestas
int orientation(const pair<int, int> &p, const pair<int, int> &q, const pair<int, int> &r) {
  long long val = 1LL * (q.second - p.second) * (r.first - q.first) -
                  1LL * (q.first - p.first) * (r.second - q.second);
  return (val == 0) ? 0 : (val > 0 ? 1 : 2); // 0: colinear, 1: horário, 2: anti-horário
}

// ======================================================================================================================= //

// Verifica se ponto q está no segmento pr
bool onSegment(const pair<int, int> &a, const pair<int, int> &b, const pair<int, int> &p) {
  return (min(a.first, b.first) <= p.first &&
          p.first <= max(a.first, b.first)) &&
         (min(a.second, b.second) <= p.second &&
          p.second <= max(a.second, b.second)) &&
         ((b.first - a.first) * (p.second - a.second) ==
          (p.first - a.first) * (b.second - a.second)); // Produto vetorial == 0
}

// ======================================================================================================================= //

// Verifica se os segmentos p1q1 e p2q2 se intersectam
bool doSegmentsIntersect(const pair<int, int>& p1, const pair<int, int>& q1, const pair<int, int>& p2, const pair<int, int>& q2) {
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

// ======================================================================================================================= //

// Verifica se há faces que se sobrepõe com base nas arestas de cada face
bool isOverlapping(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces, string &errorMessage) {

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

      // Pular arestas adjacentes ou superpostas
      if (i == j || i == j_next || i_next == j || i_next == j_next)
        continue;

      const auto &v3 = verticesCoords[face[j]];
      const auto &v4 = verticesCoords[face[j_next]];

      if (doSegmentsIntersect(v1, v2, v3, v4)) {
        errorMessage = "Face " + to_string(faceIdx + 1) +
                       " tem auto-intersecção entre arestas (" +
                       to_string(face[i] + 1) + "," + to_string(face[i_next] + 1) +
                       ") e (" + to_string(face[j] + 1) + "," + to_string(face[j_next] + 1) + ")";
        return true;
      }
    }
  }
}
  return false;
}

// ======================================================================================================================= //

// Verifica se a face está no sentido horário ou anti-horário
// Usa o algoritmo de shoelace para determinar o sentido a partir da área (https://en.wikipedia.org/wiki/Shoelace_formula)
bool isCounterClockwise(const vector<pair<int, int>>& face) {
    if (face.size() < 3) throw invalid_argument("Face inválida: menos de 3 vértices");
    
    double area = 0.0;
    int n = face.size();
    
    for (int i = 0; i < n; i++) {
        const pair<int, int>& atual = face[i];
        const pair<int, int>& proximo = face[(i + 1) % n];
        area += (proximo.first - atual.first) * (proximo.second + atual.second);
    }
    
    if (area == 0.0) throw invalid_argument("Face degenerada: área zero");
    return area < 0; // Área negativa = CCW (anti-horário)
}

// ======================================================================================================================= //

// Implementação da função debugPrintMesh para depuração da malha
void debugPrintMesh(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces) {
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
      cout << face[i]+1;
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
