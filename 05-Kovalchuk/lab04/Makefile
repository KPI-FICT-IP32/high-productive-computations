CC = mpicc
CFLAGS = -Wall
LDFLAGS =
SRCFILES = $(wildcard src/*.c)
OBJFILES = $(SRCFILES:src/%.c=build/%.o)
TARGET = bin/lab04
SRC = src
NODES ?= 4 

.PHONY: all
all: $(TARGET)


clean:
	rm -rf build/ bin/

build/%.o: src/%.c
	mkdir -p build/
	$(CC) -I$(SRC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p bin/

$(TARGET): bin $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

.PHONY: run
run: $(TARGET)
	mpirun -np $(NODES) --allow-run-as-root $(TARGET)
