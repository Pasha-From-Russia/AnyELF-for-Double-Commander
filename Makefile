CXX = g++
SDK = gtk
OBJ_PATH = ./obj
BIN_PATH = ./bin

CXXFLAGS = -fPIC -Wall -O2 

LIBS = stdc++

OBJECTS = 

ifeq ($(SDK),qt5)
LIBNAME = Qt5Widgets
LIBS += Qt5Core
LIBS += Qt5Widgets
LIBS += Qt5Gui
else
LIBNAME = gtk+-2.0
LIBS += gtk-x11-2.0
LIBS += gdk-x11-2.0
LIBS += gobject-2.0
LIBS += glib-2.0
LIBS += pango-1.0
endif

LDLIBS= $(foreach d, $(LIBS), -l$d)

INCLUDES += `pkg-config --cflags --libs $(LIBNAME)` -I../ELFIO

TARGETNAME = anyelf_$(SDK).wlx
OBJECTS = anyelf_$(SDK).o anyelfdump.o anyelf_common.o

HEADERS=$(wildcard *.h)

all: $(TARGETNAME)

%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJ_PATH)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $(OBJ_PATH)/$@ 

$(TARGETNAME): $(OBJECTS)
	@mkdir -p $(BIN_PATH)
	$(CXX) -shared -Wl,--no-undefined -o $(BIN_PATH)/$@ $(OBJ_PATH)/*.o $(LDLIBS) -static-libgcc

clean:
	rm -f *.o $(OBJ_PATH)/*.o $(BIN_PATH)/$(TARGETNAME)

strip:
	strip $(BIN_PATH)/$(TARGETNAME)
