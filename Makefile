# RPATH
# LIB_ASSIMP := ./external/assimp/lib/
LIB_GLFW := ./external/glfw/lib/
LIB_GLM := ./external/glm/lib/
# Includes
# INCL_ASSIMP := ./external/assimp/include/
INCL_GLAD := ./external/glad/include/
INCL_GLFW := ./external/glfw/include/
INCL_GLM := ./external/glm/include/
# Directories
OBJDIR := ./obj
SRCDIR := ./plum
EXTDIR := ./external

# Flags			
CFLAGS := 	-fdiagnostics-color=always \
			-g	\
			-I$(INCL_GLAD)
CXXFLAGS := 	$(CFLAGS)	\
			-I./	\
			-I$(EXTDIR)	\
			-I$(INCL_ASSIMP)	\
			-I$(INCL_GLFW)	\
			-I$(INCL_GLM)	\
			-std=c++17
# Linker-only flags
# -L same as -Wl,-L,
LDFLAGS := 	-L$(LIB_GLFW)	\
			-lglfw3	\
			-L$(LIB_GLM)	\
			-lglm	\
			-framework Cocoa 	\
			-framework OpenGL	\
			-framework IOKit
# -L$(LIB_ASSIMP)	\
# -Wl,-rpath,$(LIB_ASSIMP)	\
# -lassimp	\

# Source files
sources := $(SRCDIR)/main.cpp $(wildcard $(SRCDIR)/*/*.c*) 
externs := $(filter-out $(EXTDIR)/imgui%,$(wildcard $(EXTDIR)/*/*.c*) $(wildcard $(EXTDIR)/*/*/*.c*))
SOURCES := $(sources) $(externs)
# Object files
objects := $(SOURCES:.cpp=.o)
objects := $(objects:.c=.o)
OBJECTS := $(addprefix $(OBJDIR)/,$(objects))

# ImGui resources
# IMGUI_CORE_CPP := $(wildcard $(EXTDIR)/imgui/imgui*.cpp)
# IMGUI_CORE_O := $(patsubst $(EXTDIR)/imgui/imgui%.cpp, $(OBJDIR)/imgui%.o, $(IMGUI_CORE_CPP))
# IMGUI_MISC_O := $(OBJDIR)/imgui_stdlib.o
# IMGUI_BACKENDS_O := $(OBJDIR)/imgui_impl_glfw.o $(OBJDIR)/imgui_impl_opengl3.o

# test:
# 	@echo $(objects)
# Targets
all: objdirs main
objdirs:
	@mkdir -p $(dir $(OBJECTS))

# main: $(OBJDIR)/main.o $(SRCOBJS) $(SRCOBJS_CONTEXT) $(SRCOBJS_COMPONENT) $(SRCOBJS_RENDERER) $(SRCOBJS_UTIL) $(OBJDIR)/gl.o $(OBJDIR)/stb_image.o $(IMGUI_CORE_O) $(IMGUI_MISC_O) $(IMGUI_BACKENDS_O)
main: $(OBJECTS)
	@echo Linking $@
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Source objects
# $(OBJDIR)/main.o: $(SRCDIR)/main.cpp
# 	@echo Compiling $@
# 	@$(CXX) $(CXXFLAGS) $< -c -o $@
# $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.hpp
# 	@echo Compiling $@
# 	@$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: %.cpp
	echo Compiling $@
	$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: %.cpp %.hpp
	echo Compiling $@
	$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: %.c
	@echo Compiling $@
	@$(CC) $(CFLAGS) $< -c -o $@
$(OBJDIR)/%.o: %.c %.h
	@echo Compiling $@
	@$(CC) $(CFLAGS) $< -c -o $@
	
# # # External objects
# $(OBJDIR)/gl.o: $(EXTDIR)/glad/src/gl.c
# 	@echo Compiling $@
# 	@$(CC) $(CFLAGS) $< -c -o $@
# $(OBJDIR)/%.o: $(EXTDIR)/*/%.cpp
# 	@echo Compiling $@
# 	@$(CXX) $(CXXFLAGS) $< -c -o $@
# $(OBJDIR)/%.o: $(EXTDIR)/*/%.c
# 	@echo Compiling $@
# 	@$(CC) $(CFLAGS) $^ -c -o $@

.PHONY: clean
clean:
	@rm -fr $(OBJDIR)
	@rm -f main2
# @rm -f imgui.ini