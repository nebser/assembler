CC=/usr/bin/g++
FLAGS=-std=c++11
PATH=./h

main:	
	$(CC) ./cpp/*.cpp $(FLAGS) -o assembler.out -I$(PATH)

clean:	
	rm assembler.out