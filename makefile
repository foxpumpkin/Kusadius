-include objects.mk 
#CPP = g++-4.9 -Wall -g -O2 -std=c++11
CPP = clang++ -Wall -g -O2 -std=c++11
SRC = src
INC = include
MINELIB = ./lib/*.a
MINEINC = -I$(INC)

MAINLS = $(shell ls *.cpp)
MAINLIST = $(MAINLS:%.cpp=%.out)
SRCLS = $(shell ls src/*.cpp)
SRCLIST = $(SRCLS:src/%.cpp=lib/lib%.a)

all: $(SRCLIST) $(MAINLIST)

$(MAINLIST): FORCE
	@echo " * build executable file : $(MAINLIST)"
	$(CPP) $(MINEINC) $(MAINLS) $(LIBS) $(SRCLIST) -o $@
	@echo " "

lib/lib%.a: $(SRC)/%.cpp $(INC)/%.hpp
	@echo " * build library : $@"
	$(CPP) -c $< $(MINEINC) -o $@
	@echo " "

clean:
	@echo " * clean up"
	rm -rf lib/*.a *.out*
	@echo " "

FORCE:
.FHONY: all clean FORCE
