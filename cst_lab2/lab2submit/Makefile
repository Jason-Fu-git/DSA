main: hashtable
	g++ main.cpp hashtable.o -o main -g
hashtable:
	g++ hashtable.cpp -o hashtable.o -c -g

generate: hashtable
	g++ -std=c++11 generator.cpp hashtable.o -o generator -g
clean:
	del main.exe hashtable.o

