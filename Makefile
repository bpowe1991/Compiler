CC=g++
CFLAGS= -g
OBJS=main.o scanner.o staticSem.o parser.o

comp: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.cpp token.h scanner.h parser.h
	$(CC) $(CFLAGS) -c main.cpp

scanner.o: scanner.cpp token.h scanner.h
	$(CC) $(CFLAGS) -c scanner.cpp

parser.o: parser.cpp parser.h
	$(CC) $(CFLAGS) -c parser.cpp

staticSem.o: staticSem.cpp staticSem.h
	$(CC) $(CFLAGS) -c staticSem.cpp

clean:
	-rm -f *.o *.out comp
