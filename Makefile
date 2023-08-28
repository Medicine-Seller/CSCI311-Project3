Makefile:

all: Client ServerC ServerG

Client: Client.cpp
	g++ Client.cpp game.cpp soc.cpp -o Client

ServerC: ServerC.cpp
	g++ -lpthread ServerC.cpp soc.cpp -o ServerC

ServerG: ServerG.cpp
	g++ ServerG.cpp game.cpp soc.cpp -o ServerG

clean:
	rm -f Client
	rm -f ServerC
	rm -f ServerG