# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -g3 -Wall -fmessage-length=0 -Iheaders -Iimgui -Iimgui-SFML -I"D:/Documents/DEV/4_libs/SFML/include"
LDFLAGS = -L"D:/Documents/DEV/4_libs/SFML/lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lopengl32 -limm32 -lgdi32 -ldinput8

# Directories
SRCDIR = sources
OBJDIR = obj
BINDIR = bin
IMGUIDIR = imgui

# Source and object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
IMGUI_SOURCES = $(wildcard $(IMGUIDIR)/*.cpp) imgui-SFML/imgui-SFML.cpp
IMGUI_OBJECTS = $(patsubst %.cpp,%.o,$(IMGUI_SOURCES))

# Executable name
EXECUTABLE = $(BINDIR)/imgui_proto.exe

# Rules
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(IMGUI_OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) $(IMGUI_OBJECTS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINDIR) $(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR) $(BINDIR) $(IMGUI_OBJECTS)

.PHONY: all clean
