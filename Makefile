all:
	g++ -std=c++11 main.cpp TextureManager.cpp -lglut -lGLU -lGL -lfreeimage -o terreno.o
	./terreno.o
	
