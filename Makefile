all: clean proj4
clean:
	rm -f proj4 *.o core*
proj4:
	g++ -g -Wall -pthread project4.cpp -o proj4
