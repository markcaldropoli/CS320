CFLAGS = -Wall -Wextra -DDEBUG -g -pedantic
P = predictors

default: $(P)

$(P): $(P).o
	g++ $(CFLAGS) $(P).o -o $(P)

$(P).o: $(P).cpp
	g++ $(CFLAGS) -c $(P).cpp -o $(P).o

clean:
	rm -f *o $(P)
