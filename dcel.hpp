#ifndef DCEL_HPP
#define DCEL_HPP

#include <iostream>
#include <memory>
#include <vector>

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

#endif // DCEL_HPP
