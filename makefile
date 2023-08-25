CXX = gcc
LXX ?= lex
YXX ?= yacc

# path #
SRC_PATH = src
LEX_PATH = src/syntax
YACC_PATH = src/syntax
BUILD_PATH = build
# BUILD_LEX_PATH = build
# BUILD_YACC_PATH = build
BIN_PATH = bin
DOC_PATH = doc/html

TESTLOG_PATH = testLog
TESTPRECOMPILED_PATH = testPreCompiled
TESTCOMPILED_PATH = testCompiled
TESTRESULT_PATH = tmpResultTest

# executable # 
BIN_NAME = C2MP

# extensions #
SRC_EXT = c
LEX_EXT = l
LEX_C_EXT = yy.c
LEX_OUTPUT_C = $(BUILD_PATH)/syntax.yy.c
LEX_OUTPUT_OBJECT = $(BUILD_PATH)/syntax.yy.o
YACC_EXT = y
YACC_OUTPUT_OBJECT = $(BUILD_PATH)/y.tab.o

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
# FIND LEX FILES
SYNTAX_FILE = syntax
# SYNTAX_FILE_2 = expr
LEX = $(LEX_PATH)/$(SYNTAX_FILE).$(LEX_EXT)
# FIND YACC FILES
#TEST = ${shell echo `find $(YACC_PATH) -name '*.$(YACC_EXT)' | sort -k 1nr | cut -f2-`}
YACC = $(LEX_PATH)/$(SYNTAX_FILE).$(YACC_EXT)


# FIND LEX FILES
LEX_C = $(BUILD_LEX_PATH)/$(SYNTAX_FILE).$(LEX_C_EXT)
# FIND YACC FILES
# YACC_C = $(BUILD_YACC_PATH)/y.tab.c
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o) $(LEX_OUTPUT_OBJECT) $(YACC_OUTPUT_OBJECT)

# Set the dependency files that will be used to add header dependencies
# DEPS = $(OBJECTS:.o=.d)

# flags #
COMPILE_FLAGS = -Wall -Wextra -g -std=gnu99 -DYY_TYPEDEF_YY_SIZE_T -Dyy_size_t=ssize_t
LEX_FLAGS = 
YACC_FLAGS = -d -v
INCLUDES = -I include/ -I /usr/local/include
# Space-separated pkg-config libraries used by this project
LIBS = -ly -lfl

.PHONY: default_target
default_target: release

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(BUILD_PATH)
	@mkdir -p $(BIN_PATH)


# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)# $(YACCS_C) $(LEXS_C)
	@echo "Linking: $@"
	$(CXX) $(OBJECTS) $(LIBS) -o $@

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT) $(YACC_OUTPUT_OBJECT)
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@


$(LEX_OUTPUT_OBJECT): $(YACC_OUTPUT_OBJECT) $(LEX)
	$(LXX) $(LEX_FLAGS) -o $(LEX_OUTPUT_C) $(LEX) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(LEX_OUTPUT_C) -o $(LEX_OUTPUT_OBJECT)

$(YACC_OUTPUT_OBJECT): $(YACC)
	$(YXX) $(YACC_FLAGS) $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c y.tab.c -o $(YACC_OUTPUT_OBJECT)
	@mv y.tab.c y.tab.h $(BUILD_PATH)



.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)
	@echo "Deleting documentation"
	@$(RM) -r $(DOC_PATH)
	if test -e y.output; \
	then rm y.output; \
	fi
	if test -d doc; \
	then rm -r doc; \
	fi
	if test -e C2MP_result.c; \
	then rm C2MP_result.c; \
	fi
	if test -d $(TESTLOG_PATH); \
	then rm -r $(TESTLOG_PATH); \
	fi
	if test -d $(TESTPRECOMPILED_PATH); \
	then rm -r $(TESTPRECOMPILED_PATH); \
	fi
	if test -d $(TESTCOMPILED_PATH); \
	then rm -r $(TESTCOMPILED_PATH); \
	fi
	if test -d $(TESTRESULT_PATH); \
	then rm -r $(TESTRESULT_PATH); \
	fi
none:
	@echo "les obj"
	@echo $(OBJECTS)
	@echo "les lexs l"
	@echo $(LEXS)
	@echo "les yaccs y"
	@echo $(YACCS)
	@echo "les lexs c"
	@echo $(LEXS_C)
	@echo "les yaccs c"
	@echo $(YACCS_C)
.PHONY: doc 
doc:
	doxygen Doxyfile

.PHONY: run
run:
	./test_campaign.sh

.PHONY: archive
archive: 
	tar zcvf C2MP_DIVRIOTIS_FLINT_JUNG_VETRIVEL.tar.gz makefile src/*.c src/syntax/* Doxyfile LICENSE README.md test_campaign.sh tests/* include/*



