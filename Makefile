SRC = $(wildcard *.cpp)
OBJ_RELEASE = $(SRC:.cpp=.o)
OBJ_DEBUG   = $(SRC:.cpp=.debug.o)

LIBS = SDL3 SDL3_ttf
LDFLAGS = $(addprefix -l,$(LIBS))

LDFLAGS_RELEASE = -static-libgcc -static-libstdc++ -mwindows
# LDFLAGS_DEBUG   = 

CXXFLAGS_RELEASE = -O2
CXXFLAGS_DEBUG   = -g -DDEBUG

EXECS = emulator.exe debug.exe

all: $(EXECS)

emulator.exe: $(OBJ_RELEASE)
	g++ -o $@ $(OBJ_RELEASE) $(LDFLAGS) $(LDFLAGS_RELEASE) $(CXXFLAGS_RELEASE)

debug.exe: $(OBJ_DEBUG)
	g++ -o $@ $(OBJ_DEBUG) $(LDFLAGS) $(LDFLAGS_DEBUG) $(CXXFLAGS_DEBUG)

%.o: %.cpp
	g++ $(CXXFLAGS_RELEASE) -c $< -o $@

%.debug.o: %.cpp
	g++ $(CXXFLAGS_DEBUG) -c $< -o $@

clean:
	del /f /q $(EXECS) $(OBJ_DEBUG) $(OBJ_RELEASE)

.PHONY: all clean