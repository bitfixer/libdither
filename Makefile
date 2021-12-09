SRC_DIR := src
BIN_DIR := bin

$(BIN_DIR)/dither:
	mkdir -p $(BIN_DIR)
	g++ -o $(BIN_DIR)/dither -std=c++11 $(SRC_DIR)/Image.cpp $(SRC_DIR)/Ditherer.cpp $(SRC_DIR)/dither.cpp

clean:
	rm -f $(BIN_DIR)/*