BIN=bin

OUTPUT=lr

CFLAGS=-std=c++11 -Wall
CFLAGS_DEBUG=$(CFLAGS) -g
CFLAGS_RELEASE=$(CFLAGS) -D NDEBUG -O3

debug:
	@echo "====================----- DEBUG BUILD -----===================="
	mkdir -p $(BIN)
	g++ $(CFLAGS_DEBUG) *.cpp -o $(BIN)/$(OUTPUT)
	@echo "==============================================================="

release:
	@echo "====================----- RELEASE BUILD -----===================="
	mkdir -p $(BIN)
	g++ $(CFLAGS_RELEASE) *.cpp -o $(BIN)/$(OUTPUT)
	@echo "================================================================="

clean:
	rm -rf --preserve-root $(BIN)
