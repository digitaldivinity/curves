curves.o: curves.cpp
	g++ -Wall -ansi -c curves.cpp
curves : curves.o
	g++ -Wall -ansi curves.o -o curves -lsfml-graphics -lsfml-window -lsfml-system
run: curves
	./curves
