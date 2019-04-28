CFLAGS = -Wall -Wextra -DDEBUG -g -pedantic
C = cache-sim

default: $(C)

$(C): $(C).o
	g++ $(CFLAGS) $(C).o -o $(C)

$(C).o: $(C).cpp
	g++ $(CFLAGS) -c $(C).cpp -o $(C).o

clean:
	rm -f *o $(C)
