# $File: Makefile
# $Date: Wed Feb 02 21:01:04 2011 +0800

OBJ_DIR = obj
TARGET = graph-drawer

PKGCONFIG_LIBS = gtkmm-2.4 glibmm-2.4
INCLUDE_DIR = -I src/include -I src
DEFINES = -D_DEBUG_BUILD_
CXXFLAGS = -Wall -Wextra  \
		   $(shell pkg-config --cflags $(PKGCONFIG_LIBS)) $(INCLUDE_DIR) $(DEFINES) -g
LDFLAGS = $(shell pkg-config --libs $(PKGCONFIG_LIBS))

CXX = g++
CXXSOURCES = $(shell find src -name "*.cpp")
OBJS = $(addprefix $(OBJ_DIR)/,$(CXXSOURCES:.cpp=.o))
DEPFILES = $(OBJS:.o=.d)


.PHONY: all clean run hg

all: $(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@echo "[cxx] $< ..."
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

$(OBJ_DIR)/%.d: %.cpp
	@mkdir -pv $(dir $@)
	@echo "[dep] $< ..."
	@$(CXX) $(INCLUDE_DIR) $(DEFINES) -MM -MT "$(OBJ_DIR)/$(<:.cpp=.o) $(OBJ_DIR)/$(<:.cpp=.d)" "$<"  > "$@"

sinclude $(DEPFILES)

$(TARGET): $(OBJS)
	@echo "Linking ..."
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS)


clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

hg:
	hg addremove
	hg commit -u jiakai

