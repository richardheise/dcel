// dcel.cpp
#include "dcel.hpp"


using namespace std;

// Função principal para construir a DCEL a partir de vértices e faces
DCEL buildDCEL(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces);

// Funções de verificação da malha
// Verifica a validade da malha conforme os critérios especificados
bool checkMesh(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces, string& errorMessage) {
    // Verificar se a malha é aberta (alguma aresta é fronteira de somente uma face)
    if (isOpen(faces, errorMessage)) {
        cout << "entrou1";
        errorMessage = "aberta";
        return false;
    }
    
    // Verificar se não é uma subdivisão planar (alguma aresta é fronteira de mais de duas faces)
    if (isNotPlanarSubdivision(faces, errorMessage)) {
        cout << "entrou2";
        errorMessage = "não subdivisão planar";
        return false;
    }
    
    // Verificar se há sobreposições (alguma face tem auto-intersecção ou intersecta outras faces)
    if (isOverlapping(verticesCoords, faces, errorMessage)) {
        cout << "entrou3";
        errorMessage = "sobreposta";
        return false;
    }
    
    // Verificar se todas as faces estão em ordem anti-horária
    for (const auto& face : faces) {
        if (!isCounterClockwise(verticesCoords, face)) {
            cout << "entrou4";
            errorMessage = "face " + to_string(&face - &faces[0] + 1) + " não está em ordem anti-horária";
            return false;
        }
    }
    
    // Se passou por todas as verificações, a malha é válida
    return true;
}

// Verificar se a malha é aberta (alguma aresta é fronteira de somente uma face)
bool isOpen(const vector<vector<int>>& faces, string& errorMessage) {
    // Mapeia cada aresta para o número de vezes que ela aparece
    map<pair<int, int>, int> edgeCount;
    
    // Contar ocorrências de cada aresta
    for (const auto& face : faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            size_t j = (i + 1) % face.size();
            int v1 = face[i];
            int v2 = face[j];
            
            // Ordenar vértices para garantir uma representação única da aresta
            pair<int, int> edge = v1 < v2 ? make_pair(v1, v2) : make_pair(v2, v1);
            edgeCount[edge]++;
        }
    }
    
    // Verificar se alguma aresta aparece apenas uma vez (malha aberta)
    for (const auto& edge : edgeCount) {
        if (edge.second == 1) {
            errorMessage = "Aresta (" + to_string(edge.first.first) + "," 
                         + to_string(edge.first.second) + ") é fronteira de apenas uma face";
            return true;
        }
    }
    
    return false;
}

// Verificar se não é uma subdivisão planar (alguma aresta é fronteira de mais de duas faces)
bool isNotPlanarSubdivision(const vector<vector<int>>& faces, string& errorMessage) {
    // Mapeia cada aresta para o número de vezes que ela aparece
    map<pair<int, int>, int> edgeCount;
    
    // Contar ocorrências de cada aresta
    for (const auto& face : faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            size_t j = (i + 1) % face.size();
            int v1 = face[i];
            int v2 = face[j];
            
            // Ordenar vértices para garantir uma representação única da aresta
            pair<int, int> edge = v1 < v2 ? make_pair(v1, v2) : make_pair(v2, v1);
            edgeCount[edge]++;
        }
    }
    
    // Verificar se alguma aresta aparece mais de duas vezes (não é subdivisão planar)
    for (const auto& edge : edgeCount) {
        if (edge.second > 2) {
            errorMessage = "Aresta (" + to_string(edge.first.first) + "," 
                         + to_string(edge.first.second) + ") é fronteira de " 
                         + to_string(edge.second) + " faces";
            return true;
        }
    }
    
    return false;
}

// Verifica a orientação (sentido horário ou anti-horário) de três pontos
// Retorna:
//  0 -> Colinear
//  1 -> Sentido horário
//  2 -> Sentido anti-horário
int orientation(const pair<int, int>& p, const pair<int, int>& q, const pair<int, int>& r) {
    int val = (q.second - p.second) * (r.first - q.first) -
              (q.first - p.first) * (r.second - q.second);
    
    cout << "sentido: " << val << endl;
    if (val == 0) return 0;  // Colinear
    return (val > 0) ? 1 : 2; // Sentido horário ou anti-horário
}

// Verifica se o ponto q está no segmento pr
bool onSegment(const pair<int, int>& p, const pair<int, int>& q, const pair<int, int>& r) {
    return q.first <= max(p.first, r.first) && q.first >= min(p.first, r.first) &&
           q.second <= max(p.second, r.second) && q.second >= min(p.second, r.second);
}

// Verifica se dois segmentos se intersectam
bool doSegmentsIntersect(const pair<int, int>& p1, const pair<int, int>& q1, 
                         const pair<int, int>& p2, const pair<int, int>& q2) {
    // Encontra as quatro orientações necessárias
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
    
    // Caso geral: os segmentos se intersectam se as orientações são diferentes
    if (o1 != o2 && o3 != o4) {
        cout << "entrou 4.1.1";
        return true;
    }
    
    // Casos especiais: quando os pontos são colineares
    // p1, q1 e p2 são colineares e p2 está no segmento p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    
    // p1, q1 e q2 são colineares e q2 está no segmento p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    
    // p2, q2 e p1 são colineares e p1 está no segmento p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    
    // p2, q2 e q1 são colineares e q1 está no segmento p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;
    
    return false;
}

// Verifica se há sobreposições na malha (faces com auto-intersecção ou que intersectam outras faces)
bool isOverlapping(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces, string& errorMessage) {
    // Verifica auto-intersecção em cada face
    for (size_t faceIdx = 0; faceIdx < faces.size(); ++faceIdx) {
        const auto& face = faces[faceIdx];
        
        // Verificar auto-intersecção de arestas na mesma face
        for (size_t i = 0; i < face.size(); ++i) {
            size_t i_next = (i + 1) % face.size();
            const auto& v1 = verticesCoords[face[i]];
            const auto& v2 = verticesCoords[face[i_next]];
            
            for (size_t j = i + 2; j < face.size() + (i <= 1 ? 0 : 1); ++j) {
                size_t j_next = (j + 1) % face.size();
                
                // Não verificar arestas adjacentes
                if (i == j_next || i_next == j) continue;
                
                const auto& v3 = verticesCoords[face[j]];
                const auto& v4 = verticesCoords[face[j_next]];
                
                if (doSegmentsIntersect(v1, v2, v3, v4)) {
                    cout << "entrou 3.1";
                    errorMessage = "Face " + to_string(faceIdx + 1) + 
                                   " tem auto-intersecção entre arestas (" + 
                                   to_string(face[i]) + "," + to_string(face[i_next]) + ") e (" + 
                                   to_string(face[j]) + "," + to_string(face[j_next]) + ")";
                    return true;
                }
            }
        }
    }
    
    // Verificar intersecção entre faces diferentes
    for (size_t faceIdx1 = 0; faceIdx1 < faces.size(); ++faceIdx1) {
        const auto& face1 = faces[faceIdx1];
        
        for (size_t faceIdx2 = faceIdx1 + 1; faceIdx2 < faces.size(); ++faceIdx2) {
            const auto& face2 = faces[faceIdx2];
            
            // Verificar intersecção entre arestas de duas faces diferentes
            for (size_t i = 0; i < face1.size(); ++i) {
                size_t i_next = (i + 1) % face1.size();
                const auto& v1 = verticesCoords[face1[i]];
                const auto& v2 = verticesCoords[face1[i_next]];
                
                for (size_t j = 0; j < face2.size(); ++j) {
                    size_t j_next = (j + 1) % face2.size();
                    const auto& v3 = verticesCoords[face2[j]];
                    const auto& v4 = verticesCoords[face2[j_next]];
                    
                    // Ignorar interseções em vértices comuns
                    if (face1[i] == face2[j] || face1[i] == face2[j_next] || 
                        face1[i_next] == face2[j] || face1[i_next] == face2[j_next]) {
                        continue;
                    }
                    
                    if (doSegmentsIntersect(v1, v2, v3, v4)) {
                        errorMessage = "Faces " + to_string(faceIdx1 + 1) + " e " + 
                                       to_string(faceIdx2 + 1) + " têm intersecção entre arestas (" + 
                                       to_string(face1[i]) + "," + to_string(face1[i_next]) + ") e (" + 
                                       to_string(face2[j]) + "," + to_string(face2[j_next]) + ")";
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

// Função auxiliar para calcular o produto vetorial entre dois vetores
double crossProduct(double x1, double y1, double x2, double y2) {
    return x1 * y2 - y1 * x2;
}

// Função para calcular o produto vetorial entre três pontos (p1-p2) × (p3-p2)
double crossProduct(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3) {
    double x1 = p1.first - p2.first;  // Vetor p1-p2 (componente x)
    double y1 = p1.second - p2.second; // Vetor p1-p2 (componente y)
    double x2 = p3.first - p2.first;   // Vetor p3-p2 (componente x)
    double y2 = p3.second - p2.second; // Vetor p3-p2 (componente y)
    
    return crossProduct(x1, y1, x2, y2);
}

// Verifica se os vértices de uma face estão em ordem anti-horária
bool isCounterClockwise(const vector<pair<int, int>>& verticesCoords, const vector<int>& faceVertices) {
    // Uma face precisa ter pelo menos 3 vértices para formar um polígono
    if (faceVertices.size() < 3) {
        return false;
    }
    
    // Calcular a soma dos produtos vetoriais para determinar a orientação
    // Se a soma é positiva, então a orientação é anti-horária
    double sum = 0.0;
    
    for (size_t i = 0; i < faceVertices.size(); i++) {
        size_t j = (i + 1) % faceVertices.size();
        size_t k = (i + 2) % faceVertices.size();
        
        // Obter as coordenadas dos vértices
        const pair<int, int>& v1 = verticesCoords[faceVertices[i]];
        const pair<int, int>& v2 = verticesCoords[faceVertices[j]];
        const pair<int, int>& v3 = verticesCoords[faceVertices[k]];
        
        // Calcular o produto vetorial e adicionar à soma
        sum += crossProduct(v1, v2, v3);
    }
    
    // Se a soma dos produtos vetoriais é positiva, a orientação é anti-horária
    return sum > 0;
}

// Funções para construção da DCEL
void createVertices(DCEL& dcel, const vector<pair<int, int>>& verticesCoords);
void createHalfEdges(DCEL& dcel, const vector<vector<int>>& faces);
void connectTwinHalfEdges(DCEL& dcel);
void identifyExternalFace(DCEL& dcel);
void updateVertexIncidentEdges(DCEL& dcel);

// Funções auxiliares para operações geométricas
double crossProduct(double x1, double y1, double x2, double y2);
double crossProduct(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3);
bool doSegmentsIntersect(const pair<int, int>& p1, const pair<int, int>& p2, 
                         const pair<int, int>& p3, const pair<int, int>& p4);
int orientation(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3);
bool onSegment(const pair<int, int>& p, const pair<int, int>& q, const pair<int, int>& r);

// Funções para saída do DCEL no formato requerido
string dcelToString(const DCEL& dcel);
int getEdgeCount(const DCEL& dcel);
vector<vector<int>> getHalfEdgeDescription(const DCEL& dcel);
vector<int> getFaceDescription(const DCEL& dcel);
vector<vector<int>> getVertexDescription(const DCEL& dcel);

// Funções de debug

// Implementação da função debugPrintMesh para depuração da malha
void debugPrintMesh(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces) {
    cout << "\n===== DEBUG: DETALHES DA MALHA =====\n";
    
    // Mostra informações sobre os vértices
    cout << "Vértices (total: " << verticesCoords.size() - 1 << "):\n";
    for (int i = 1; i < verticesCoords.size(); ++i) {
        cout << "  V" << i << ": (" << verticesCoords[i].first << ", " 
             << verticesCoords[i].second << ")\n";
    }
    
    // Mostra informações sobre as faces
    cout << "\nFaces (total: " << faces.size() << "):\n";
    int faceIndex = 0;
    for (const auto& face : faces) {
        cout << "  Face " << ++faceIndex << ": ";
        
        // Lista os vértices de cada face
        for (size_t i = 0; i < face.size(); ++i) {
            cout << face[i];
            if (i < face.size() - 1) cout << " → ";
        }
        // Adiciona o primeiro vértice novamente para completar o ciclo visualmente
        if (!face.empty()) {
            cout << " → " << face[0];
        }
        cout << "\n";
        
        // Verificação da orientação da face (sentido anti-horário)
        bool isCCW = isCounterClockwise(verticesCoords, face);
        cout << "    Orientação: " << (isCCW ? "Anti-horária ✓" : "Horária ✗") << "\n";
        
        // Listar as arestas da face
        cout << "    Arestas: ";
        for (size_t i = 0; i < face.size(); ++i) {
            int next = (i + 1) % face.size();
            cout << "(" << face[i] << "," << face[next] << ")";
            if (i < face.size() - 1) cout << ", ";
        }
        cout << "\n";
    }
    
    // Contagem e identificação de arestas
    map<pair<int, int>, int> edgeCount;
    for (const auto& face : faces) {
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
    for (const auto& edge : edgeCount) {
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
void debugPrintDCEL(const DCEL& dcel);

// Função para mapear índices entre a entrada e a DCEL
map<pair<int, int>, int> createEdgeToHalfEdgeMap(const DCEL& dcel);
int getHalfEdgeIndex(const DCEL& dcel, const shared_ptr<HalfEdge>& halfEdge);
int getVertexIndex(const DCEL& dcel, const shared_ptr<Vertex>& vertex);
int getFaceIndex(const DCEL& dcel, const shared_ptr<Face>& face);