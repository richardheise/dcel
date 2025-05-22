#!/usr/bin/env python3

import matplotlib.pyplot as plt
from random import seed
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

    fig, ax = plt.subplots(figsize=(10, 10))

    # Configuração do plano cartesiano
    ax.axhline(0, color='gray', linewidth=0.5, linestyle='--')  # Eixo X
    ax.axvline(0, color='gray', linewidth=0.5, linestyle='--')  # Eixo Y
    ax.grid(True, color='lightgray', linestyle='--', linewidth=0.5)
    
    # Desenhar as faces (arestas mais grossas)
    for face in fList:
        for i in range(len(face)):
            idx1 = face[i - 1] - 1
            idx2 = face[i] - 1
            x1, y1 = vList[idx1].x, vList[idx1].y
            x2, y2 = vList[idx2].x, vList[idx2].y
            ax.plot([x1, x2], [y1, y2], color="#333333", linewidth=3, zorder=1)

    # Plotar os vértices com cores variadas e rótulos maiores
    colors = plt.cm.tab10.colors
    for i, p in enumerate(vList):
        color = colors[i % len(colors)]
        ax.scatter(p.x, p.y, color=color, s=100, zorder=2, edgecolor='black', linewidth=1)
        # Rótulos maiores e com fundo branco para melhor legibilidade
        ax.text(p.x, p.y, f" {i+1}", 
                fontsize=12,  # Aumentei de 8 para 12
                fontweight='bold',  # Texto em negrito
                verticalalignment='center',
                bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', boxstyle='round,pad=0.2'))

    # Configurações finais do gráfico
    plt.axis('scaled')
    plt.title(f"Visualização: {nome_base}", pad=20, fontsize=14)
    plt.tight_layout()
    plt.savefig(output_path, bbox_inches='tight', dpi=300)
    print(f"Imagem salva em: {output_path}")