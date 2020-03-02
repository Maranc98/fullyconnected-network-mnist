CC = g++

SRC = src
BUILD = build
CONF = conf
BIN = bin

CFLAGS = -Wall -I$(CONF) -I/usr/include/allegro5 -L/usr/lib -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf

OBJ_FILENAMES = main.o graphics.o neuron.o layers.o network.o read_mnist.o
OBJ = $(patsubst %,$(BUILD)/%,$(OBJ_FILENAMES))

CONF_FILENAMES = conf.h
CONF_FILES = $(patsubst %,$(CONF)/%,$(CONF_FILENAMES))

# Object files

$(BUILD)/%.o: $(SRC)/%.cpp $(CONF_FILES)
	$(CC) $(CFLAGS) -I$(CONF) -c -o $@ $<

# Executables

$(BIN)/main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Cleaning

.PHONY: clean

clean:
	rm -f $(BUILD)/*
	rm -f $(BIN)/*
