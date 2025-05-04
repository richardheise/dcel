# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Nome do executável
TARGET = malha

# Fontes e objetos
SRCS = main.cpp dcel.cpp
OBJS = $(SRCS:.cpp=.o)

# Regra padrão
all: $(TARGET)

# Compilação do executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Limpeza
clean:
	rm -f $(OBJS) $(TARGET)

# Recompilação
rebuild: clean all

.PHONY: all clean rebuild

