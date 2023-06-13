SRC_DIR := src
BIN_DIR := bin

SOURCES := $(SRC_DIR)/Image.cpp $(SRC_DIR)/Ditherer.cpp $(SRC_DIR)/dither.cpp upng/upng.c BMP-C/src/bmpfile.c

$(BIN_DIR)/dither: $(SOURCES)
	mkdir -p $(BIN_DIR)
	g++ -o $(BIN_DIR)/dither -std=c++11 $(SOURCES)

upng/upng.c:
	git submodule update --init --recursive

clean:
	rm -f $(BIN_DIR)/*