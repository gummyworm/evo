CLIENT_LIBS = -lenet -lSDL -lGL -lGLU -lGLEW -ldl -lm  -lglib-2.0
SERVER_LIBS = -lenet

CFLAGS = `pkg-config --cflags --libs glib-2.0` -Wall

# component sources 
COMPONENT_SOURCES=$(wildcard Components/*.h)
COMPONENT_OBJECTS=$(addprefix obj/,$(notdir $(COMPONENT_SOURCES:.h=.o)))

#client source files
CLIENT_SOURCES=AppSpec/app.c client.c client_packet.c collision.c draw.c entity.c guilayout.c material.c matrix.c model.c modelgen.c scene.c util.c cJSON.c 
CLIENT_OBJECTS=$(CLIENT_SOURCES:.c=.o)

#server source files
SERVER_SOURCES=server/server.c
SERVER_OBJECTS=$(SERVER_SOURCES:.c=.o)

#execuables
CLIENT_EXECUTABLE=Client
SERVER_EXECUTABLE=Server

all: $(SOURCES) $(CLIENT_EXECUTABLE) $(SERVER_EXECUTABLE) 
    
$(CLIENT_EXECUTABLE): $(CLIENT_OBJECTS) $(COMPONENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJECTS) $(CLIENT_LIBS) 

$(SERVER_EXECUTABLE): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJECTS) $(SERVER_LIBS) 

.c.o:
	$(CC) $(CFLAGS) -UBUILD_COMPONENT -c $< -o $@ 

obj/%.o: Components/%.h
	$(CC) $(CFLAGS) -DBUILD_COMPONENT -c $< -o $@ 

clean:
	rm -f *.o
	rm -f obj/*.o
