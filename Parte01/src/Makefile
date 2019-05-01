#Makefile para la compilación del simulador de Cache
CC = g++
CFLAGS = -o
SOURCE = main.cpp L1cache.cpp debug_utilities.cpp
OBJ = $(SOURCE:.cpp = .o)
TARGET = cache
#Argumentos
TCACHE = 64
TBLOQUE = 8
ASOC4 = 4
ASOC2 = 2
ASOC1 = 1
RP1 = LRU
RP2 = RRIP


$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(TARGET) $(OBJ)

run:
	gunzip -c mcf.trace.gz | ./$(TARGET) -a $(ASOC4) -t $(TCACHE) -l $(TBLOQUE) -rp $(RP2)

run1:
	gunzip -c mcf.trace.gz | ./$(TARGET) -a $(ASOC2) -t $(TCACHE) -l $(TBLOQUE) -rp $(RP2)

run2:
	./$(TARGET) -a $(ASOC2) -t $(TCACHE) -l $(TBLOQUE) -rp $(RP1)

run4:
	./$(TARGET) -a $(ASOC4) -t $(TCACHE) -l $(TBLOQUE) -rp $(RP1)

clean:
	rm -f $(TARGET) ./*.o ./*.gch
