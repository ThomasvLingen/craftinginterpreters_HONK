SILENT = @

# Compiler
CC = g++
LD = $(CC)

# Compiler flags
# Wall = all warnings
# Wextra = extra warnings
# g = debug symbols
# std=c++1z = C++17
INCLUDE_PATHS = -Isrc/
COMPILER_FLAGS = -Wall -Wextra -g -std=c++1z $(INCLUDE_PATHS)

# Clang tidy
CLANG_TIDY = clang-tidy

# Files to compile
SRC_PATH = src/
SRC_FILES = $(shell find $(SRC_PATH) -name '*.cpp')
HPP_FILES = $(shell find $(SRC_PATH) -name '*.hpp')
ALL_FILES = $(SRC_FILES) $(HPP_FILES)
OBJ_FILES = $(SRC_FILES:.cpp=.o)

LIBS_PATH =
LIBS = 
LINKER_FLAGS = -g $(LIBS)

# Path for the resulting file
BUILD_PATH = build/
PROGRAM_NAME = honk
EXEC = $(BUILD_PATH)$(PROGRAM_NAME)

LINT_FILES = $(shell find src/ -name '*.cpp' -o -name '*.hpp')

# Compile stuff
all : $(EXEC)

%.o : %.cpp
	@echo CC $<
	$(SILENT) $(CC) -c $< $(COMPILER_FLAGS) -o $@

$(EXEC) : $(OBJ_FILES) $(BUILD_PATH)
	@echo
	@echo LD $@
	$(SILENT) $(LD) $(COMPILER_FLAGS) $(OBJ_FILES) $(LINKER_FLAGS) -o $(EXEC)

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

lint : $(SRC_FILES)
	$(CLANG_TIDY) $(LINT_FILES) $(HPP_FILES) -- $(COMPILER_FLAGS)

run : $(EXEC)
	$(EXEC)

debug: $(EXEC)
	gdb -tui $(EXEC)

memory_leaks: $(EXEC) 
	valgrind --tool=memcheck --leak-check=full $(EXEC)

profiler: $(EXEC) 
	valgrind --tool=callgrind $(EXEC)

search_for_tabs: $(SRC_PATH)
	grep -r "	" $(SRC_PATH)

tabs_to_spaces: $(ALL_FILES)
	sed -i 's/	/    /g' $(ALL_FILES)

.PHONY: clean
clean:
	@echo "Cleaning build"
	rm -rf $(shell find $(SRC_PATH) -name '*.o')
	rm -rf $(BUILD_PATH)*
