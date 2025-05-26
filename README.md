# DCEL Implementation for Planar Subdivision

Computational Geometry Project  
Richard Fernando Heise Ferreira  
Federal University of Paraná – Graduate Program in Informatics

## Summary

This project implements a **Doubly Connected Edge List (DCEL)** for representing planar subdivisions. The system is capable of:

- Reading a planar mesh description;
- Verifying its topological validity;
- Generating the corresponding DCEL structure.

Algorithms were implemented to detect:

- Open meshes;
- Non-planar subdivisions;
- Faces with overlapping regions.

## Objectives

The **DCEL** data structure is widely used in computer graphics, geographic information systems, and geometric modeling. This implementation aims to:

- Provide a compact representation of vertices, edges, and faces;
- Enable efficient navigation among adjacent elements;
- Verify topological properties;
- Support complex geometric operations.

---

## Implemented Algorithms

### Mesh Openness and Non-Planar Subdivision Check

- Checks if the mesh is topologically closed and valid;
- Ensures each edge has a twin edge;
- Validates that each pair of half-edges appears exactly twice;
- If an edge appears fewer or more times, the mesh is considered open or invalid, respectively.

### Overlapping Detection

- Detects self-intersections and invalid vertex positioning in faces;
- Checks if non-adjacent edges within the same face intersect using vector orientations and special collinearity cases;
- For each counterclockwise-oriented face, verifies if any external vertex lies on its boundary or inside the face using collinearity and ray casting techniques;
- Any detected overlap or misplaced vertex invalidates the mesh.

### DCEL Construction

The DCEL construction follows three main steps:

1. Creation of vertices from input coordinates in the given order;
2. Construction of half-edges and linking twin edges;
3. Establishing adjacency relations (next and previous edges).

---

## Code Structure

The project is organized as follows:

```plain  text
├── dcel.cpp
├── dcel.hpp
├── main.cpp
├── Makefile
├── README.md
├── run_tests.sh
├── tests
│ ├── generateImgs.sh
│ ├── imgs
│ │ └── *.png (test images)
│ ├── inputs
│ │ └── *.in (test input files)
│ ├── outputs
│ │ └── *.out (expected test outputs)
│ ├── printFigure.py
│ └── validator
│ ├── printDCEL.py
│ ├── run_validator.sh
│ └── valid
│ └── *.png (validated DCEL images)
└── trabalho2.pdf
```

### Main Classes

- `Vertex`: Stores coordinates and a pointer to an incident edge.
- `HalfEdge`: Represents half-edges with references to origin vertex, face, next, previous, and twin half-edges.
- `Face`: Contains a pointer to its outer component.
- `DCEL`: Main structure aggregating all elements.

### Key Functions

- `buildFromMesh`: Builds the DCEL from input data, preserving the input order.
- `createHalfEdgesAndFaces`: Implements the algorithm to construct edges and faces.
- `printDCELOutput`: Outputs the DCEL data in the specified format.
- `ValidateEdges`: Performs topological validity checks.

---

## Testing and Validation

The system was validated using multiple test cases, including:

- Valid meshes of various complexities;
- Open meshes;
- Non-planar subdivisions;
- Faces with self-intersections.

Test files are located in the `tests` folder:

- `inputs`: test inputs;
- `outputs`: expected outputs for comparison;
- `imgs`: visual representations of tests;
- `generateImgs.sh`: script to generate test images;
- `validator`: scripts to validate DCEL outputs and generate images from valid DCEL structures.

The script `run_tests.sh` automates running all tests from the project root, generating outputs and performing validations.

