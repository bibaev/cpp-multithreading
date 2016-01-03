BIN = ./bin/
SRC = ./src/
OBJS = \
	$(BIN)main.o
TARGET = ./bin/str_ptr
CXXFLAGS = -Wall -Werror -O2 -ggdb -std=c++11

all: bin build

run: build
	$(TARGET)

build: $(OBJS) 
	g++ $(OBJS) $(CXXFLAGS) -o $(TARGET)

$(BIN)%.o: $(SRC)%.cpp
	g++ -c $< $(CXXFLAGS) -o $@

bin:
	mkdir -p bin

clean:
	rm -r $(BIN)*
	rm $(TARGET)
	