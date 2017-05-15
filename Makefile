CC = g++
CFLAGS = -Wall -std=gnu++11 -g
LDFLAGS = -L/usr/local/lib -lzmq 

EXEC_1 = depl_unit #adding here the name of the program
SOURCES_1 = $(wildcard src/deployment_unit/*.cpp)
PATH_1 = src/deployment_unit/
OBJECTS_1 = $(SOURCES_1:.cpp=.o)

EXEC_2 = server #adding here the name of the program
SOURCES_2 = $(wildcard src/server/*.cpp)
PATH_2 = src/server/
OBJECTS_2 = $(SOURCES_2:.cpp=.o)

EXEC_3 = broker #adding here the name of the program
SOURCES_3 = $(wildcard src/broker/*.cpp)
PATH_3 = src/broker/
OBJECTS_3 = $(SOURCES_3:.cpp=.o)

EXEC_4 = client #adding here the name of the program
SOURCES_4 = $(wildcard src/client/*.cpp)
PATH_4 = src/client/
OBJECTS_4 = $(SOURCES_4:.cpp=.o)

all: $(EXEC_1) $(EXEC_2) $(EXEC_3) $(EXEC_4) 

$(EXEC_1): $(OBJECTS_1) 
	$(CC) $(OBJECTS_1) -o $(EXEC_1) $(LDFLAGS) 

$(EXEC_2): $(OBJECTS_2)
	$(CC) $(OBJECTS_2) -o $(EXEC_2) $(LDFLAGS) 

$(EXEC_3): $(OBJECTS_3) 
	$(CC) $(OBJECTS_3) -o $(EXEC_3) $(LDFLAGS) 

$(EXEC_4): $(OBJECTS_4)
	$(CC) $(OBJECTS_4) -o $(EXEC_4) $(LDFLAGS) 
		
$(PATH_1)%.o: $(PATH_1)%.cpp 
	$(CC) -c $(CFLAGS) $< -o $@

$(PATH_2)%.o: $(PATH_2)%.cpp 
	$(CC) -c $(CFLAGS) $< -o $@

$(PATH_3)%.o: $(PATH_3)%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(PATH_4)%.o: $(PATH_4)%.cpp 
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -rf $(EXEC_1) $(OBJECTS_1)
	rm -rf $(EXEC_2) $(OBJECTS_2)
	rm -rf $(EXEC_3) $(OBJECTS_3)
	rm -rf $(EXEC_4) $(OBJECTS_4)

clean_$(EXEC_1):
	rm -rf $(EXEC_1) $(OBJECTS_1)
	
clean_$(EXEC_2):
	rm -rf $(EXEC_3) $(OBJECTS_3)

clean_$(EXEC_3):
	rm -rf $(EXEC_3) $(OBJECTS_3)

clean_$(EXEC_4):
	rm -rf $(EXEC_4) $(OBJECTS_4)