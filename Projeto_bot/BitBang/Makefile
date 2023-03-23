CC = g++
CFLAGS = -I.
DEPS = 
OBJ = main.o
EXEC_FILE = BitBang

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC_FILE): $(OBJ)
	$(CC) -o $(EXEC_FILE) $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXEC_FILE)
	rm -f *.o
	rm -f *~
