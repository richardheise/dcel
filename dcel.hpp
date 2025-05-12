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
};

// Função principal para construir a DCEL a partir de vértices e faces
DCEL buildDCEL(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces);

// Funções de verificação da malha
bool checkMesh(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces, string& errorMessage);
bool isOpen(const vector<vector<int>>& faces, string& errorMessage);
bool isNotPlanarSubdivision(const vector<vector<int>>& faces, string& errorMessage);
bool isOverlapping(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces, string& errorMessage);
bool isCounterClockwise(const vector<pair<int, int>>& verticesCoords, const vector<int>& faceVertices);

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
void debugPrintMesh(const vector<pair<int, int>>& verticesCoords, const vector<vector<int>>& faces);
void debugPrintDCEL(const DCEL& dcel);

// Função para mapear índices entre a entrada e a DCEL
map<pair<int, int>, int> createEdgeToHalfEdgeMap(const DCEL& dcel);
int getHalfEdgeIndex(const DCEL& dcel, const shared_ptr<HalfEdge>& halfEdge);
int getVertexIndex(const DCEL& dcel, const shared_ptr<Vertex>& vertex);
int getFaceIndex(const DCEL& dcel, const shared_ptr<Face>& face);

#endif // DCEL_HPP
