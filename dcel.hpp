// dcel.hpp
#ifndef DCEL_HPP
#define DCEL_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <cmath>
#include <utility>
#include <stdexcept>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

class Vertex;
class HalfEdge;
class Face;

// ---------- Classe Vertex ----------
class Vertex {
public:
  double x, y;
  shared_ptr<HalfEdge> incidentEdge;

  Vertex(double x = 0, double y = 0) : x(x), y(y) {}

  void print() const {
    cout << "Vertex(" << x << ", " << y << ")";
    if (incidentEdge)
      cout << " -> incidentEdge exists";
    else
      cout << " -> no incidentEdge";
    cout << endl;
  }
};

// ---------- Classe HalfEdge ----------
class HalfEdge {
public:
  shared_ptr<Vertex> origin;
  shared_ptr<HalfEdge> twin;
  shared_ptr<HalfEdge> next;
  shared_ptr<HalfEdge> prev;
  shared_ptr<Face> incidentFace;

  void print() const {
    cout << "HalfEdge from ";
    if (origin)
      cout << "(" << origin->x << ", " << origin->y << ")";
    else
      cout << "NULL";
    cout << " to ";
    if (next && next->origin)
      cout << "(" << next->origin->x << ", " << next->origin->y << ")";
    else
      cout << "NULL";
    cout << endl;
  }
};

// ---------- Classe Face ----------
class Face {
public:
  shared_ptr<HalfEdge> outerComponent;
  vector<shared_ptr<HalfEdge>> innerComponents;

  void print() const {
    cout << "Face: outerComponent starts at ";
    if (outerComponent && outerComponent->origin)
      cout << "(" << outerComponent->origin->x << ", "
           << outerComponent->origin->y << ")";
    else
      cout << "NULL";
    cout << ", with " << innerComponents.size() << " inner components." << endl;
  }
};

// ---------- Classe DCEL ----------
class DCEL {
public:
  vector<shared_ptr<Vertex>> vertices;
  vector<shared_ptr<HalfEdge>> halfEdges;
  vector<shared_ptr<Face>> faces;

  DCEL() = default;

  void buildFromMesh(const vector<pair<int, int>>& vertexCoords, const vector<vector<int>>& faceVertices) {
    // 1. Criar vértices a partir das coordenadas
    createVertices(vertexCoords);
    
    // 2. Criar semi-arestas e faces a partir da lista de vértices das faces
    createHalfEdgesAndFaces(faceVertices);
    
    // 3. Configurar arestas incidentes para os vértices
    setupVertexIncidentEdges();
  }

private:
// ======================================================================================================================= //

  void createVertices(const vector<pair<int, int>>& vertexCoords) {
    // Cria objetos vértice a partir dos pares de coordenadas
    for (const auto& coord : vertexCoords) {
      auto vertex = make_shared<Vertex>(coord.first, coord.second);
      vertices.push_back(vertex);
    }
  }

// ======================================================================================================================= //

  void createHalfEdgesAndFaces(const vector<vector<int>>& faceVertices) {
    // Mapa para controlar arestas e encontrar gêmeas (v1,v2) -> semi-aresta
    map<pair<int, int>, shared_ptr<HalfEdge>> edgeMap;
    
    // Processa cada face da entrada
    for (const auto& faceVerts : faceVertices) {
      auto face = make_shared<Face>();
      faces.push_back(face);
      
      vector<shared_ptr<HalfEdge>> faceEdges;
      int n = faceVerts.size();
      
      // Cria semi-arestas para cada aresta da face
      for (int i = 0; i < n; i++) {
        int v1 = faceVerts[i];     // Índice do vértice inicial
        int v2 = faceVerts[(i + 1) % n]; // Índice do vértice final
        
        // Cria nova semi-aresta
        auto he = make_shared<HalfEdge>();
        he->origin = vertices[v1];  // Define vértice de origem
        he->incidentFace = face;    // Define face incidente
        
        halfEdges.push_back(he);
        faceEdges.push_back(he);
        
        // Verifica se existe aresta gêmea no mapa
        auto twinKey = make_pair(v2, v1);
        if (edgeMap.count(twinKey)) {
          // Conecta as gêmeas se encontrada
          he->twin = edgeMap[twinKey];
          edgeMap[twinKey]->twin = he;
          edgeMap.erase(twinKey);
        } else {
          // Armazena no mapa se não encontrou gêmea ainda
          edgeMap[make_pair(v1, v2)] = he;
        }
      }
        
      // Conecta ponteiros next e prev para as semi-arestas da face
      for (int i = 0; i < n; i++) {
        faceEdges[i]->next = faceEdges[(i + 1) % n];
        faceEdges[i]->prev = faceEdges[(i - 1 + n) % n];
      }
      
      // Define componente externa da face como a primeira semi-aresta
      face->outerComponent = faceEdges[0];
    }
    
    // Verifica arestas não pareadas (não deve acontecer em malha válida)
    if (!edgeMap.empty()) {
        cerr << "Aviso: " << edgeMap.size() << " arestas não pareadas!" << endl;
    }
  }

  void setupVertexIncidentEdges() {
    // Define aresta incidente para cada vértice (primeira semi-aresta onde o vértice é origem)
    for (const auto& v : vertices) {
      for (const auto& he : halfEdges) {
        if (he->origin == v) {
          v->incidentEdge = he;
          break;
        }
      }
    }
  }

// ======================================================================================================================= //

public:
  void printDCELOutput() const {
    // Cria mapeamentos de índices baseados em 1 para todos os elementos
    unordered_map<shared_ptr<Vertex>, size_t> vertexIndices;
    unordered_map<shared_ptr<HalfEdge>, size_t> halfEdgeIndices;
    unordered_map<shared_ptr<Face>, size_t> faceIndices;
    
    // Preenche os mapeamentos de índices na ordem de armazenamento
    for (size_t i = 0; i < vertices.size(); i++) {
      vertexIndices[vertices[i]] = i + 1;
    }
    for (size_t i = 0; i < faces.size(); i++) {
      faceIndices[faces[i]] = i + 1;
    }
    for (size_t i = 0; i < halfEdges.size(); i++) {
      halfEdgeIndices[halfEdges[i]] = i + 1;
    }
    
    // 1. Primeira linha: contagem de vértices, arestas (semi-arestas/2), faces
    cout << vertices.size() << " " << (halfEdges.size() / 2) << " " << faces.size() << endl;
    
    // 2. Seção de vértices: x y índice_da_semi-aresta_incidente
    for (const auto& v : vertices) {
      cout << static_cast<int>(v->x) << " " << static_cast<int>(v->y) << " ";
      cout << (v->incidentEdge ? halfEdgeIndices.at(v->incidentEdge) : 0) << endl;
    }
    
    // 3. Seção de faces: índice_da_semi-aresta_do_componente_externo
    for (const auto& f : faces) {
      cout << (f->outerComponent ? halfEdgeIndices.at(f->outerComponent) : 0) << endl;
    }
    
    // 4. Seção de semi-arestas: origem gêmea face_esquerda próxima anterior
    for (const auto& he : halfEdges) {
      cout << vertexIndices.at(he->origin) << " ";
      cout << (he->twin ? halfEdgeIndices.at(he->twin) : 0) << " ";
      cout << faceIndices.at(he->incidentFace) << " ";
      cout << (he->next ? halfEdgeIndices.at(he->next) : 0) << " ";
      cout << (he->prev ? halfEdgeIndices.at(he->prev) : 0) << endl;
    }
  }
};

// ======================================================================================================================= //

/**
 * Verifica se a malha é topologicamente válida (bem formada)
 * @param verticesCoords Vetor de coordenadas (x,y) dos vértices
 * @param faces Vetor de faces, cada face é um vetor de índices de vértices
 * @param errorMessage Mensagem de erro de saída caso a malha seja inválida
 * @return true se a malha for válida, false caso contrário
 */
bool checkMesh(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces, string &errorMessage);

/**
 * Verifica se há faces contidas dentro de outras faces
 * @param verticesCoords Vetor de coordenadas (x,y) dos vértices
 * @param faces Vetor de faces, cada face é um vetor de índices de vértices
 * @param errorMessage Mensagem de erro de saída caso encontre sobreposição
 * @return true se não houver faces contidas em outras, false caso contrário
 */
bool checkInside(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces, string &errorMessage);

/**
 * Verifica se um ponto está exatamente na borda de um polígono
 * @param pt Ponto a ser verificado
 * @param polygon Vetor de pontos que formam o polígono
 * @return true se o ponto está na borda, false caso contrário
 */
bool isPointOnPolygonBorder(const pair<int, int> &pt, const vector<pair<int, int>> &polygon);

/**
 * Verifica se um ponto está dentro de um polígono (usando ray casting)
 * @param pt Ponto a ser verificado
 * @param polygon Vetor de pontos que formam o polígono
 * @return true se o ponto está dentro, false caso contrário
 */
bool isPointInsidePolygon(const pair<int, int> &pt, const vector<pair<int, int>> &polygon);

/**
 * Valida as arestas da malha verificando consistência topológica
 * @param faces Vetor de faces da malha
 * @param errorMessage Mensagem de erro de saída caso encontre problemas
 * @return 1 se válido, -1 para malha aberta, -2 para não-subdivisão planar
 */
int ValidateEdges(const vector<vector<int>> &faces, string &errorMessage);

/**
 * Calcula a orientação de três pontos (clockwise/counter-clockwise/collinear)
 * @param p Primeiro ponto
 * @param q Segundo ponto
 * @param r Terceiro ponto
 * @return 0 para colineares, 1 para clockwise, 2 para counter-clockwise
 */
int orientation(const pair<int, int> &p, const pair<int, int> &q, const pair<int, int> &r);

/**
 * Verifica se um ponto está em um segmento de reta
 * @param a Ponto inicial do segmento
 * @param b Ponto final do segmento
 * @param p Ponto a ser verificado
 * @return true se o ponto está no segmento, false caso contrário
 */
bool onSegment(const pair<int, int> &a, const pair<int, int> &b, const pair<int, int> &p);

/**
 * Calcula o produto vetorial entre três pontos
 * @param p1 Primeiro ponto
 * @param p2 Segundo ponto
 * @param p3 Terceiro ponto
 * @return Valor do produto vetorial (determinante)
 */
double crossProduct(const pair<int, int> &p1, const pair<int, int> &p2, const pair<int, int> &p3);

/**
 * Verifica se há sobreposição entre faces da malha
 * @param verticesCoords Vetor de coordenadas dos vértices
 * @param faces Vetor de faces da malha
 * @param errorMessage Mensagem de erro de saída caso encontre sobreposição
 * @return true se houver sobreposição, false caso contrário
 */
bool isOverlapping(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces, string &errorMessage);

/**
 * Verifica se os vértices de uma face estão em ordem counter-clockwise
 * @param face Vetor de coordenadas dos vértices da face
 * @return true se estiver em CCW, false caso contrário
 */
bool isCounterClockwise(const vector<pair<int, int>>& face);

/**
 * Imprime a malha para fins de depuração
 * @param verticesCoords Vetor de coordenadas dos vértices
 * @param faces Vetor de faces da malha
 */
void debugPrintMesh(const vector<pair<int, int>> &verticesCoords, const vector<vector<int>> &faces);

#endif // DCEL_HPP
