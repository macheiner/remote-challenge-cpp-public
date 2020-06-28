CXX      	 := g++
CXXFLAGS 	 := -MD -Wall -Wextra -std=c++17 -Wall -Wextra -Wshadow -Wconversion -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wsign-conversion -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2 -Woverloaded-virtual -Wno-null-dereference -pedantic -Wswitch-enum

LDFLAGS  	 :=
BUILD    	 := ./build
TARGET   	 := challenge

OBJ_DIR_DBG  := $(BUILD)/$(TARGET)-debug/objects
OBJ_DIR_REL  := $(BUILD)/$(TARGET)-release/objects

APP_DIR  	 := ./bin
INCLUDE  	 := -iquote include -isystem external_header
SRC      	 := $(wildcard src/*.cpp)

OBJECTS_REL := $(SRC:%.cpp=$(OBJ_DIR_REL)/%.o)
OBJECTS_DBG := $(SRC:%.cpp=$(OBJ_DIR_DBG)/%.o)

all: release debug

build_binary_release: build $(APP_DIR)/$(TARGET)-release
build_binary_debug: build $(APP_DIR)/$(TARGET)-debug

$(OBJ_DIR_REL)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJ_DIR_DBG)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET)-release: $(OBJECTS_REL)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(APP_DIR)/$(TARGET)-task1-release $(OBJ_DIR_REL)/src/task1.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(APP_DIR)/$(TARGET)-task2-release $(OBJ_DIR_REL)/src/task2.o $(LDFLAGS)

$(APP_DIR)/$(TARGET)-debug: $(OBJECTS_DBG)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(APP_DIR)/$(TARGET)-task1-debug $(OBJ_DIR_DBG)/src/task1.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(APP_DIR)/$(TARGET)-task2-debug $(OBJ_DIR_DBG)/src/task2.o $(LDFLAGS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR_REL)
	@mkdir -p $(OBJ_DIR_DBG)

debug: CXXFLAGS += -g -ggdb
debug: build_binary_debug

release: CXXFLAGS += -O3 -DNDEBUG -Wno-unused-but-set-variable -Wno-unused-parameter
release: clean build_binary_release

clean:
	-@rm -rvf $(OBJ_DIR_REL)/*
	-@rm -rvf $(OBJ_DIR_DBG)/*
	-@rm -rvf $(APP_DIR)/*


-include $(OBJECTS_REL:.o=.d)
-include $(OBJECTS_DBG:.o=.d)
