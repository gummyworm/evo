INCLUDE = /usr/include/lua5.2
LIBS = -lenet -lSDL -lGL -lGLU -lGLEW -llua -ltolua -ldl -lm  -lglib-2.0
SERVERLIBS = -lenet

CFLAGS = `pkg-config --cflags --libs glib-2.0` 

all: Client Server
    
Client: client.o draw.o luainterface.o toluabindings.o model.o client_packet.o matrix.o collision.o guilayout.o modelgen.o
	$(CC) $(CFLAGS) -o $@ client.o matrix.o draw.o luainterface.o toluabindings.o model.o client_packet.o collision.o guilayout.o modelgen.o $(LIBS)

Server: server.o 
	$(CC) $(CFLAGS) -o $@ server.o $(SERVERLIBS) -I $(INCLUDE)

client.o: client.c draw.c luainterface.c toluabindings.c model.c client_packet.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

server.o: server/server.c 
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

draw.o: draw.c model.c guilayout.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

guilayout.o: guilayout.c 
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

collision.o: collision.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

luainterface.o: luainterface.c 
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

model.o: model.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

modelgen.o: modelgen.c model.c draw.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

luabindings.o: toluabindings.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

matrix.o: matrix.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)

client_packet.o: client_packet.c
	$(CC) -c $(CFLAGS) $? -I $(INCLUDE)



all: Client Server

clean:
	rm -f *.o

