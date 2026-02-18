CC = g++

BUILTDIR = build
CFLAGS = -c -I include -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

EXECUTABLE = program 
SOURCES = main.cpp tree.cpp TreeGraphDump.cpp RerursiveDescentReadTree.cpp differentiator.cpp ConfolutionOfConstants.cpp latexDump.cpp

SRC = $(addprefix Akinator/source/,$(SOURCES))
OBJECTS = $(addprefix $(BUILTDIR)/,$(SOURCES:.cpp=.o))

all: $(EXECUTABLE)

$(EXECUTABLE): $(BUILTDIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE)

$(BUILTDIR):
	mkdir $(BUILTDIR)

$(BUILTDIR)/%.o: source/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	del build\*.o
	del .\program.exe