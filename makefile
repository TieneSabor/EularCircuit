PROJNAME := Eular_Circuit_Test
CC := g++
SRC := $(wildcard src/*.cpp)
NOD := $(notdir $(SRC)) #SRC w/o dir.
OBJ := $(patsubst %.cpp,bin/%.o,$(NOD))
INC := -I inc/
CFLAGS := -g -Wa,--warn -ffunction-sections -fdata-sections
LINKFLAGS := -Wl,--gc-sections
MATHFLAGS := -lm
PREPRO := -D BASIC_LINEAR

prt:
	@echo $(OBJ)

all: $(OBJ)
	$(CC) $(PREPRO) -o bin/$(PROJNAME) $(OBJ) $(LINKFLAGS) $(MATHFLAGS)

bin/%.o: src/%.cpp
	$(CC) $(PREPRO) $(CFLAGS) $(INC) -c $< -o $@ $(MATHFLAGS)

clean:
	rm bin/*