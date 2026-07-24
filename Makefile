SRC = $(wildcard *.cpp)

LIBS = SDL3 SDL3_ttf
LDFLAGS = $(addprefix -l,$(LIBS))

EXECS = emulator.exe debug.exe

all: $(EXECS)

emulator.exe: $(SRC)
	g++ -O2 -o $@ $(SRC) $(LDFLAGS)

debug.exe: $(SRC)
	g++ -g -o $@ $(SRC) $(LDFLAGS) 

clean:
	del /f /q $(EXECS)