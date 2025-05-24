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

  void print() const {
    cout << "==== DCEL ====" << endl;

    cout << "\nVertices:" << endl;
    for (const auto &v : vertices)
      v->print();

    cout << "\nHalfEdges:" << endl;
    for (const auto &he : halfEdges)
      he->print();

    cout << "\nFaces:" << endl;
    for (const auto &f : faces)
      f->print();

    cout << "===============" << endl;
  }

   // Constrói a DCEL a partir dos dados de entrada
    void buildFromMesh(const vector<pair<int, int>>& vertexCoords, const vector<vector<int>>& faceVertices) {
        // 1. Criar vértices
        createVertices(vertexCoords);

        // 2. Criar semi-arestas e conectá-las  
        createHalfEdgesAndFaces(faceVertices);
        
        // 3. Configurar ponteiros incidentEdge nos vértices
        setupVertexIncidentEdges();
    }

private:
    // Cria os vértices a partir das coordenadas
    void createVertices(const vector<pair<int, int>>& vertexCoords) {
        for (const auto& coord : vertexCoords) {
            vertices.push_back(make_shared<Vertex>(coord.first, coord.second));
        }
    }

    // Cria semi-arestas e faces
    void createHalfEdgesAndFaces(const vector<vector<int>>& faceVertices) {
        // Mapeamento para encontrar arestas gêmeas: (v1, v2) -> half-edge
        map<pair<int, int>, shared_ptr<HalfEdge>> edgeMap;
        
        for (const auto& faceVerts : faceVertices) {
            auto face = make_shared<Face>();
            faces.push_back(face);
            
            vector<shared_ptr<HalfEdge>> faceEdges;
            int n = faceVerts.size();
            
            // Criar semi-arestas para esta face
            for (int i = 0; i < n; i++) {
                int v1 = faceVerts[i];
                int v2 = faceVerts[(i + 1) % n];
                
                auto he = make_shared<HalfEdge>();
                he->origin = vertices[v1];
                he->incidentFace = face;
                
                // Adicionar à lista de semi-arestas
                halfEdges.push_back(he);
                faceEdges.push_back(he);
                
                // Verificar se existe aresta gêmea
                auto twinKey = make_pair(v2, v1);
                if (edgeMap.count(twinKey)) {
                    he->twin = edgeMap[twinKey];
                    edgeMap[twinKey]->twin = he;
                    edgeMap.erase(twinKey); // Já encontramos o par
                } else {
                    edgeMap[make_pair(v1, v2)] = he;
                }
            }
            
            // Conectar next e prev para esta face
            for (int i = 0; i < n; i++) {
                faceEdges[i]->next = faceEdges[(i + 1) % n];
                faceEdges[i]->prev = faceEdges[(i - 1 + n) % n];
            }
            
            // Definir outerComponent da face como a primeira semi-aresta
            face->outerComponent = faceEdges[0];
        }
        
        // Verificar se todas as arestas foram pareadas (opcional, para debug)
        if (!edgeMap.empty()) {
            cerr << "Aviso: Algumas arestas não foram pareadas!" << endl;
        }
    }

    // Configura ponteiros incidentEdge nos vértices
    void setupVertexIncidentEdges() {
        for (const auto& he : halfEdges) {
            if (!he->origin->incidentEdge) {
                he->origin->incidentEdge = he;
            }
        }
    }

public:
    // Método para gerar a saída no formato especificado
    void printDCELOutput() const {
        // Números de vértices, arestas (não semi-arestas) e faces
        cout << vertices.size() << " " << (halfEdges.size() / 2) << " " << faces.size() << endl;
        
        // Vértices: x y incidentEdgeIndex
        for (size_t i = 0; i < vertices.size(); i++) {
            const auto& v = vertices[i];
            // Encontrar o índice da semi-aresta incidente
            int heIndex = -1;
            for (size_t j = 0; j < halfEdges.size(); j++) {
                if (halfEdges[j] == v->incidentEdge) {
                    heIndex = j + 1; // +1 porque os índices começam em 1
                    break;
                }
            }
            cout << v->x << " " << v->y << " " << heIndex << endl;
        }
        
        // Faces: outerComponentIndex (innerComponents não implementados neste exemplo)
        for (size_t i = 0; i < faces.size(); i++) {
            const auto& f = faces[i];
            // Encontrar o índice da semi-aresta outerComponent
            int heIndex = -1;
            for (size_t j = 0; j < halfEdges.size(); j++) {
                if (halfEdges[j] == f->outerComponent) {
                    heIndex = j + 1;
                    break;
                }
            }
            cout << heIndex << endl;
        }
        
        // Semi-arestas: origin twin leftFace next prev
        for (size_t i = 0; i < halfEdges.size(); i++) {
            const auto& he = halfEdges[i];
            
            // Índices (todos +1 porque começam em 1)
            int originIndex = -1, twinIndex = -1, faceIndex = -1, nextIndex = -1, prevIndex = -1;
            
            // Encontrar origin (vértice)
            for (size_t j = 0; j < vertices.size(); j++) {
                if (vertices[j] == he->origin) {
                    originIndex = j + 1;
                    break;
                }
            }
            
            // Encontrar twin (semi-aresta)
            for (size_t j = 0; j < halfEdges.size(); j++) {
                if (halfEdges[j] == he->twin) {
                    twinIndex = j + 1;
                    break;
                }
            }
            
            // Encontrar face
            for (size_t j = 0; j < faces.size(); j++) {
                if (faces[j] == he->incidentFace) {
                    faceIndex = j + 1;
                    break;
                }
            }
            
            // Encontrar next e prev
            for (size_t j = 0; j < halfEdges.size(); j++) {
                if (halfEdges[j] == he->next) {
                    nextIndex = j + 1;
                }
                if (halfEdges[j] == he->prev) {
                    prevIndex = j + 1;
                }
                if (nextIndex != -1 && prevIndex != -1) break;
            }
            
            cout << originIndex << " " << twinIndex << " " << faceIndex << " "
                 << nextIndex << " " << prevIndex << endl;
        }
    }
};

// Função principal para construir a DCEL a partir de vértices e faces
DCEL buildDCEL(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces);

// Funções de verificação da malha
bool checkMesh(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces, string& errorMessage);
int ValidateEdges(const vector<vector<int>>& faces, string& errorMessage);
bool isOverlapping(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces, string& errorMessage);

// Funções auxiliares para operações geométricas
double crossProduct(double x1, double y1, double x2, double y2);
double crossProduct(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3);
bool doSegmentsIntersect(const pair<int, int>& p1, const pair<int, int>& p2, 
                         const pair<int, int>& p3, const pair<int, int>& p4);
int orientation(const pair<int, int>& p1, const pair<int, int>& p2, const pair<int, int>& p3);
bool onSegment(const pair<int, int>& p, const pair<int, int>& q, const pair<int, int>& r);

// Funções de debug
void debugPrintMesh(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces);

bool checkOrientations(const vector<pair<int, int>> &verticesCoords,
                       const vector<vector<int>> &faces, string &errorMessage);

bool checkInside(const vector<pair<int, int>> &verticesCoords,
                const vector<vector<int>> &faces,
                string &errorMessage);

bool isPointInsidePolygon(const pair<int, int> &pt, const vector<pair<int, int>> &polygon);

bool isPointOnPolygonBorder(const pair<int, int> &pt, const vector<pair<int, int>> &polygon);

bool isCounterClockwise(const vector<pair<int, int>>& face);

#endif // DCEL_HPP
