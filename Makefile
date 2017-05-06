CC = g++
CFLAGS = -Wall 
LDFLAGS = -L/usr/local/lib -lzmq

EXEC = Ventilator #adding here name of the program
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LDFLAGS)
		
%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -rf $(EXEC) $(OBJECTS)
