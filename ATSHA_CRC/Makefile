CC = g++
CFLAGS = -I.
DEPS = 
OBJ = main.o
EXEC_FILE = bin

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC_FILE): $(OBJ)
	$(CC) -static -static-libgcc -o $(EXEC_FILE) $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXEC_FILE)
	rm -f *.o
	rm -f *~
