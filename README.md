# Trabalho 2 de Geometria Computacional

## Objetivo

O objetivo deste projeto é implementar estruturas de dados para representação de **polígonos** e **pontos** em um plano bidimensional, bem como desenvolver algoritmos para:

- Verificar se um polígono é **simples** (isto é, sem interseções internas).
- Determinar se um polígono é **convexo**.
- Classificar um conjunto de **pontos** como internos, na borda ou externos ao polígono.

## Estrutura do Código

O projeto foi desenvolvido em **C++**. A organização dos arquivos é a seguinte:

- `main.cpp`: Responsável pela leitura da entrada e impressão da saída, conforme especificado no PDF de descrição do trabalho.
- `polygons.cpp`: Contém a implementação dos algoritmos e a manipulação das estruturas de dados dos polígonos e pontos.
- `polygons.hpp`: Contém a implementação dos cabeçalhos das funções e declarações das classes `Dot` (ponto) e `Polygon` (polígono), bem como seus contrutores e interface.
- `Makefile`: Arquivo Make para compilar o código.

Dentro da pasta tests há a seguinte estrutura:

- `generator.cpp`: Arquivo que gera testes aleatórios.
- `Makefile`: Arquivo Make para compilar o generator.cpp.
- `printer.py`: Arquivo python para ler as entradas na pasta `in/` e transformá-las em imagens na pasta `imgs/`.
- `run_tests.py`: Arquivo que compila o fonte `main.cpp` da pasta anterior e executa o programa com as entradas de `in/`, escrevendo as saídas em `out/` e as comparando com as respostas em `answers/`.

## Algoritmos Implementados

### 1. Verificação de Simplicidade

Para identificar se um polígono é **simples**, foi utilizado um algoritmo de força bruta que verifica se há interseção entre quaisquer duas arestas não adjacentes. A verificação ignora arestas consecutivas, pois estas compartilham um vértice e não constituem interseção inválida.

### 2. Verificação de Convexidade

A verificação de **convexidade** considera que os vértices dos polígonos são fornecidos em **sentido anti-horário**, o que permite uma verificação eficiente. O algoritmo percorre os vértices e verifica se todos os ângulos (via produto vetorial) estão na mesma direção. A verificação de simplicidade é realizada antes, para garantir que não haja autointerseções que possam invalidar a análise.

### 3. Classificação de Pontos

A classificação dos pontos quanto à sua posição em relação ao polígono foi realizada usando o **algoritmo do Ray Casting**:

- Uma reta (raio) é projetada a partir do ponto em questão.
- Conta-se o número de intersecções desse raio com as arestas do polígono.
- Se o número de intersecções for **ímpar**, o ponto está **dentro** do polígono.
- Se for **par**, o ponto está **fora**.

#### Casos Especiais Tratados

- Se o ponto estiver **sobre uma aresta**, ele é considerado **interno**.
- Se o ponto coincidir com um **vértice**, também é considerado **interno**.
- Se o ponto estiver **alinhado com um vértice**, conta-se a interseção apenas uma vez, evitando duplicidade.

### Interseções

As verificações de interseção de segmentos são feitas com base em **produtos vetoriais**, permitindo identificar orientação relativa entre pontos e segmentos.

### Testes

Os arquivos de testes estão dentro da pasta `tests/in/` e consistem de diversos testes de sanidade sobre os polígonos e pontos. A maioria autoexplicativa de acordo com o nome do arquivo.
