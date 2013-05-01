BIN=bin

OUTPUT=lr

all:
	mkdir -p $(BIN)
	g++ -std=c++11 -g -Wall *.cpp -o $(BIN)/$(OUTPUT)

clean:
	rm -rf --preserve-root $(BIN)
