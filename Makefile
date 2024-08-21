# RPATH
RPATH = /usr/local/lib/
# Directories
OBJDIR = ./obj
SRCDIR = ./plum
EXTDIR = ./external

# Flags			
CFLAGS = 	-fdiagnostics-color=always \
			-g	\
			-Wall
CXXFLAGS = 	$(CFLAGS)	\
			-I./	\
			-std=c++17
# Linker-only flags
LFLAGS = 	-lglfw.3	\
			-lassimp	\
			-Wl,-rpath,$(RPATH)	\
			-framework Cocoa 	\
			-framework OpenGL	\
			-framework IOKit

# Headers
HEADERS = $(SRCDIR)/camera.hpp $(SRCDIR)/engine.hpp $(SRCDIR)/interface.hpp $(SRCDIR)/light.hpp $(SRCDIR)/mesh.hpp $(SRCDIR)/model.hpp $(SRCDIR)/resources.hpp $(SRCDIR)/scene.hpp $(SRCDIR)/scenenode.hpp $(SRCDIR)/shader.hpp $(SRCDIR)/shape.hpp $(SRCDIR)/texture.hpp

# ImGui resources
IMGUI_CORE_CPP = $(wildcard $(EXTDIR)/imgui/imgui*.cpp)
IMGUI_CORE_O = $(patsubst $(EXTDIR)/imgui/imgui%.cpp, $(OBJDIR)/imgui%.o, $(IMGUI_CORE_CPP))
IMGUI_MISC_O = $(OBJDIR)/imgui_stdlib.o
IMGUI_BACKENDS_O = $(OBJDIR)/imgui_impl_glfw.o $(OBJDIR)/imgui_impl_opengl3.o

# Targets
main: $(OBJDIR)/main.o $(OBJDIR)/glad.o $(OBJDIR)/stb_image.o $(IMGUI_CORE_O) $(IMGUI_MISC_O) $(IMGUI_BACKENDS_O)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $^ -o $@

# .o targets
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: $(EXTDIR)/*/%.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: $(EXTDIR)/*/%.c
	$(CC) $(CFLAGS) $^ -c -o $@
$(OBJDIR)/%.o: $(SRCDIR)/*/%.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: $(SRCDIR)/*/%.c
	$(CC) $(CFLAGS) $^ -c -o $@

.PHONY: clean
clean:
	rm $(OBJDIR)/*.o
	rm main
	rm imgui.ini