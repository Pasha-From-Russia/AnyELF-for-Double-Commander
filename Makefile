CC = g++
DIRECTIVE =
OBJ_PATH = ./obj
BIN_PATH = ./bin

CFLAGS = -fPIC -Wall -Wno-unused-result -O2 -g3 $(DIRECTIVE)

LIBS = -lstdc++ -static-libgcc
INCLUDES += `pkg-config --cflags --libs gtk+-2.0` -I../ELFIO

TARGETNAME = anyelf_gtk.wlx

objects = anyelf_gtk.o anyelfdump.o

all: $(TARGETNAME)

anyelfdump.o: anyelfdump.cpp
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(OBJ_PATH)/$@ 

anyelf_gtk.o: anyelf_gtk.cpp anyelf_gtk.h common.h
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(OBJ_PATH)/$@

$(TARGETNAME): $(objects)
	@mkdir -p $(BIN_PATH)
	$(CC) $(LIBS) -shared -o $(BIN_PATH)/$@ $(OBJ_PATH)/*.o

clean:
	rm -f *.o $(OBJ_PATH)/*.o $(TARGETNAME)

strip:
	strip $(BIN_PATH)/$(TARGETNAME)
