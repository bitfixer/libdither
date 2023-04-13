SRC_DIR := src
BIN_DIR := bin

SOURCES := $(SRC_DIR)/Image.cpp $(SRC_DIR)/Ditherer.cpp $(SRC_DIR)/dither.cpp upng/upng.c

$(BIN_DIR)/dither: $(SOURCES)
	mkdir -p $(BIN_DIR)
	g++ -o $(BIN_DIR)/dither -std=c++11 $(SOURCES)

clean:
	rm -f $(BIN_DIR)/*