CC = g++
CFLAGS = -Wall 
LDFLAGS = -L/usr/local/lib -lzmq

EXEC_1 = depl_unit #adding here the name of the program
SOURCES_1 = $(wildcard src/deployment_unit/*.cpp)
OBJECTS_1 = $(SOURCES_1:.cpp=.o)

EXEC_2 = server #adding here the name of the program
SOURCES_2 = $(wildcard src/server/*.cpp)
OBJECTS_2 = $(SOURCES_2:.cpp=.o)

EXEC_3 = broker #adding here the name of the program
SOURCES_3 = $(wildcard src/broker/*.cpp)
OBJECTS_3 = $(SOURCES_3:.cpp=.o)

all: $(EXEC_1) $(EXEC_2) $(EXEC_3)

$(EXEC_1): $(OBJECTS_1)
	$(CC) $(OBJECTS_1) -o $(EXEC_1) $(LDFLAGS)

$(EXEC_2): $(OBJECTS_2)
	$(CC) $(OBJECTS_2) -o $(EXEC_2) $(LDFLAGS)

$(EXEC_3): $(OBJECTS_3)
	$(CC) $(OBJECTS_3) -o $(EXEC_3) $(LDFLAGS)
		
%.o: %.cpp %.hpp
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -rf $(EXEC_1) $(OBJECTS_1)
	rm -rf $(EXEC_2) $(OBJECTS_2)
	rm -rf $(EXEC_3) $(OBJECTS_3)

clean_$(EXEC_1):
	rm -rf $(EXEC_1) $(OBJECTS_1)
	
clean_$(EXEC_2):
	rm -rf $(EXEC_3) $(OBJECTS_3)