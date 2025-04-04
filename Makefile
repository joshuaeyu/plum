# RPATH
LIB_ASSIMP := ./external/assimp/lib/
LIB_GLFW := ./external/glfw/lib/
LIB_GLM := ./external/glm/lib/
# Includes
INCL_ASSIMP := ./external/assimp/include/
INCL_GLAD := ./external/glad/include/
INCL_GLFW := ./external/glfw/include/
INCL_GLM := ./external/glm/include/
# Directories
SRCDIR := ./plum
EXTDIR := ./external
OBJDIR := ./obj

# Flags			
CFLAGS := 	-fdiagnostics-color=always \
			-g	\
			-I$(INCL_GLAD)
CXXFLAGS := 	$(CFLAGS)	\
				-I./	\
				-I$(EXTDIR)	\
				-I$(INCL_GLFW)	\
				-I$(INCL_GLM)	\
				-I$(INCL_ASSIMP) \
				-std=c++17
# Linker-only flags
# -L same as -Wl,-L,
LDFLAGS := 	-L$(LIB_GLFW) -lglfw3	\
			-L$(LIB_GLM) -lglm	\
			-L$(LIB_ASSIMP)	-Wl,-rpath,$(LIB_ASSIMP) -lassimp	\
			-framework Cocoa 	\
			-framework IOKit

# Source files
sources := $(SRCDIR)/main.cpp $(wildcard $(SRCDIR)/*/*.c*) 
externs := $(wildcard $(EXTDIR)/*/*.c*) $(wildcard $(EXTDIR)/*/*/*.c*)
SOURCES := $(sources) $(externs)
# Source header files
HEADERS := $(wildcard $(SRCDIR)/*/*.h*)
# Object files
objects := $(SOURCES:.cpp=.o)
objects := $(objects:.c=.o)
OBJECTS := $(addprefix $(OBJDIR)/,$(objects))

# Targets
all: objdirs main
objdirs:
	@mkdir -p $(dir $(OBJECTS))

# Link
main: $(OBJECTS)
	@echo Linking $@
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Compile
$(OBJDIR)/%.o: %.cpp %.hpp
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: %.cpp
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $< -c -o $@
$(OBJDIR)/%.o: %.c %.h
	@echo Compiling $@
	@$(CC) $(CFLAGS) $< -c -o $@
$(OBJDIR)/%.o: %.c
	@echo Compiling $@
	@$(CC) $(CFLAGS) $< -c -o $@

# Clean
.PHONY: clean
clean:
	@rm -fr $(OBJDIR)
	@rm -f main
	@rm -f imgui.ini