#!/usr/bin/env python3

import matplotlib.pyplot as plt
from random import uniform, seed
import sys
import os

class Point:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Uso: python3 printFigure.py <arquivo_entrada>")
        sys.exit(1)

    entrada_path = sys.argv[1]
    nome_base = os.path.splitext(os.path.basename(entrada_path))[0]
    output_dir = "imgs"
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, f"{nome_base}.png")

    with open(entrada_path, "r") as f:
        lines = f.readlines()

    seed()
    vList = []
    fList = []

    n, f = map(int, lines[0].split())

    for i in range(1, n + 1):
        x, y = map(int, lines[i].split())
        vList.append(Point(x, y))

    for i in range(n + 1, n + f + 1):
        face = list(map(int, lines[i].split()))
        fList.append(face)

    fig, ax = plt.subplots()

    # Desenhar as faces
    for face in fList:
        xCoord = [vList[idx - 1].x for idx in face]
        yCoord = [vList[idx - 1].y for idx in face]
        ax.fill(xCoord, yCoord, color=(uniform(0.5, 1), uniform(0.5, 1), uniform(0.5, 1), 0.8))

        # Também desenhar arestas
        for i in range(len(face)):
            x1 = vList[face[i - 1] - 1].x
            y1 = vList[face[i - 1] - 1].y
            x2 = vList[face[i] - 1].x
            y2 = vList[face[i] - 1].y
            ax.plot([x1, x2], [y1, y2], color="#000", lw=1)

    plt.axis('scaled')
    plt.savefig(output_path)
    print(f"Imagem salva em: {output_path}")
