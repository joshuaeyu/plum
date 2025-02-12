# RPATH
LIB_ASSIMP = ./external/assimp/lib/
LIB_GLFW = ./external/glfw/lib/
LIB_GLM = ./external/glm/lib/
# Includes
INCL_ASSIMP = ./external/assimp/include/
INCL_GLAD = ./external/glad/include/
INCL_GLFW = ./external/glfw/include/
INCL_GLM = ./external/glm/include/
# Directories
OBJDIR = ./obj
SRCDIR = ./plum
EXTDIR = ./external

# Flags			
CFLAGS = 	-fdiagnostics-color=always \
			-g	\
			-I$(INCL_GLAD)
CXXFLAGS = 	$(CFLAGS)	\
			-I./	\
			-I$(EXTDIR)	\
			-I$(INCL_ASSIMP)	\
			-I$(INCL_GLFW)	\
			-I$(INCL_GLM)	\
			-std=c++17
# Linker-only flags
# -L same as -Wl,-L,
LFLAGS = 	-L$(LIB_GLFW)	\
			-lglfw3	\
			-L$(LIB_GLM)	\
			-lglm	\
			-L$(LIB_ASSIMP)	\
			-Wl,-rpath,$(LIB_ASSIMP)	\
			-lassimp	\
			-framework Cocoa 	\
			-framework OpenGL	\
			-framework IOKit

# Object files
SRCOBJS = $(OBJDIR)/camera.o $(OBJDIR)/engine.o $(OBJDIR)/interface.o $(OBJDIR)/light.o $(OBJDIR)/scene.o $(OBJDIR)/mesh.o $(OBJDIR)/model.o $(OBJDIR)/resources.o $(OBJDIR)/scenenode.o $(OBJDIR)/shader.o $(OBJDIR)/shape.o $(OBJDIR)/texture.o

SRCOBJS_CONTEXT = $(OBJDIR)/context/window.o $(OBJDIR)/context/context.o
SRCOBJS_COMPONENT = $(OBJDIR)/component/core.o $(OBJDIR)/component/vertex.o

# ImGui resources
IMGUI_CORE_CPP = $(wildcard $(EXTDIR)/imgui/imgui*.cpp)
IMGUI_CORE_O = $(patsubst $(EXTDIR)/imgui/imgui%.cpp, $(OBJDIR)/imgui%.o, $(IMGUI_CORE_CPP))
IMGUI_MISC_O = $(OBJDIR)/imgui_stdlib.o
IMGUI_BACKENDS_O = $(OBJDIR)/imgui_impl_glfw.o $(OBJDIR)/imgui_impl_opengl3.o

# Targets
all: obj main
obj:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/context
	@mkdir -p $(OBJDIR)/component
main: $(OBJDIR)/main.o $(SRCOBJS) $(SRCOBJS_CONTEXT) $(SRCOBJS_COMPONENT) $(OBJDIR)/gl.o $(OBJDIR)/stb_image.o $(IMGUI_CORE_O) $(IMGUI_MISC_O) $(IMGUI_BACKENDS_O)
	@echo Linking $@
	@$(CXX) $(CXXFLAGS) $(LFLAGS) $^ -o $@

# Source objects
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.hpp
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $< -c -o $@
	
# # External objects
$(OBJDIR)/gl.o: $(EXTDIR)/glad/src/gl.c
	@echo Compiling $@
	@$(CC) $(CFLAGS) $< -c -o $@
$(OBJDIR)/%.o: $(EXTDIR)/*/%.cpp
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: $(EXTDIR)/*/%.c
	@echo Compiling $@
	@$(CC) $(CFLAGS) $^ -c -o $@

.PHONY: clean
clean:
	@rm -fr $(OBJDIR)
	@rm -f main
	@rm -f imgui.ini