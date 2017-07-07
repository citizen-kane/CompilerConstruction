CC = g++
OPT = -g
WARN = -Wall
ERR = -Werror
CPP_ELEVEN = -std=c++0x
INC = ./inc
SRC = ./src

CFLAGS = $(OPT) $(WARN) $(ERR) $(CPP_ELEVEN) -I$(INC) 

CACHE_SRC = $(SRC)/main.cc $(SRC)/scanner.cc $(SRC)/parser.cc $(SRC)/codegen.cc
TARGET = codegen

all: $(TARGET)
	@echo "Compilation done!!!"
$(TARGET):
	$(CC) $(CFLAGS) $(CACHE_SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
