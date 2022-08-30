all:main


main: main.o
	g++ -std=c++11 main.o -o PhonePricePrediction.out -lpthread

main.o: main.cpp
	g++ -std=c++11 -c main.cpp -lpthread

PHONY: clean

clean:
	rm *.o 
	rm *.out
