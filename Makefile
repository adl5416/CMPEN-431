.PHONY: clean

all: 431project.cpp 431project.h 431projectUtils.cpp YOURCODEHERE.cpp
	rm -rf DSE
	g++ -std=c++0x -O3 431project.cpp 431projectUtils.cpp YOURCODEHERE.cpp -lm -o DSE

DSE: 431project.cpp 431project.h 431projectUtils.cpp YOURCODEHERE.cpp
	g++ -std=c++0x -O3 -std=c++11 431project.cpp 431projectUtils.cpp YOURCODEHERE.cpp -lm -o DSE

clean:
	rm -rf DSE

