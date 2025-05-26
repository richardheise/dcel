#!/usr/bin/env python3

# Feito por Matheus de Moraes Cavazotti (github.com/mcavazotti)
# Modificado para ler de arquivo especificado como argumento e salvar em valid/

import matplotlib.pyplot as plt
from random import uniform, seed
import sys
import os

# Definição de Ponto
class Point:
    def __init__(self, idx: int, x: int, y: int, edge: int) -> None:
        self.idx = idx
        self.x = x
        self.y = y
        self.edge = edge

    def __str__(self) -> str:
        return "[{}]({},{}){}".format(self.idx, self.x, self.y, self.edge)

    def __repr__(self) -> str:
        return self.__str__()

# Definição de Semi-Aresta
class HalfEdge:
    def __init__(self, idx: int, point: int, twin: int, face: int, next: int, prev: int) -> None:
        self.idx = idx
        self.point = point
        self.twin = twin
        self.face = face
        self.next = next
        self.prev = prev

    def __str__(self) -> str:
        return "[{}][o {},s {},f {},n {},p {}]".format(self.idx, self.point, self.twin, self.face, self.next, self.prev)

    def __repr__(self) -> str:
        return self.__str__()

# Definição de Face
class Face:
    def __init__(self, idx: int, edge: int) -> None:
        self.idx = idx
        self.edge = edge

    def __str__(self) -> str:
        return "[{}]{}".format(self.idx, self.edge)

    def __repr__(self) -> str:
        return self.__str__()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: ./draw.py <arquivo_de_entrada>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    input_name = os.path.splitext(os.path.basename(input_file))[0]
    
    seed()
    vList = []
    fList = []
    eList = []

    # Ler entrada do arquivo
    with open(input_file, 'r') as f:
        # Ler primeira linha (contagens)
        first_line = f.readline()
        while first_line.strip() == '':
            first_line = f.readline()  # Pular linhas vazias no início
        v, a, f_count = map(int, first_line.split())
        
        # Ler vértices
        for i in range(1, v+1):
            line = f.readline()
            while line.strip() == '':
                line = f.readline()  # Pular linhas vazias
            x, y, e = map(int, line.split())
            vList.append(Point(i, x, y, e))

        # Ler faces
        for i in range(1, f_count+1):
            line = f.readline()
            while line.strip() == '':
                line = f.readline()  # Pular linhas vazias
            e = int(line)
            fList.append(Face(i, e))

        # Ler semi-arestas
        for i in range(1, 2*a+1):
            line = f.readline()
            while line.strip() == '':
                line = f.readline()  # Pular linhas vazias
            v_idx, t, f_idx, n, p = map(int, line.split())
            eList.append(HalfEdge(i, v_idx, t, f_idx, n, p))

    # Inicializar figura
    fig, ax = plt.subplots()

    xCoord = []
    yCoord = []
    for face in fList:
        # Preencher faces
        edge = eList[face.edge - 1]
        tmpEdge = eList[edge.next -1]

        xCoord.clear()
        yCoord.clear()

        xCoord.append(vList[edge.point -1].x)
        yCoord.append(vList[edge.point -1].y)

        while tmpEdge.idx != edge.idx:
            xCoord.append(vList[tmpEdge.point -1].x)
            yCoord.append(vList[tmpEdge.point -1].y)
            tmpEdge = eList[tmpEdge.next - 1]
        
        ax.fill(xCoord, yCoord, color=(uniform(0.5, 1), uniform(0.5, 1), uniform(0.5, 1), 0.8))

    # Plotar arestas
    for e in eList:
        x1 = vList[e.point - 1].x
        y1 = vList[e.point - 1].y
        x2 = vList[eList[e.next - 1].point - 1].x
        y2 = vList[eList[e.next - 1].point - 1].y
        ax.plot([x1, x2], [y1, y2], color="#000", lw=1)

    # Ajustar a escala para não ter distorção
    plt.axis('scaled')

    # Criar diretório valid se não existir
    if not os.path.exists('valid'):
        os.makedirs('valid')

    # Salvar figura em arquivo
    output_path = f"valid/{input_name}.png"
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()  # Fechar a figura para liberar memória