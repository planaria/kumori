CXX ?= clang++

CFLAGS = -std=c++14 -MMD -MP -Wall -Wextra
CFLAGS_DEBUG = -g -O0
CFLAGS_RELEASE = -O3
LDFLAGS = -lpthread -lssl -lcrypto -lboost_system -lboost_filesystem -lboost_context -lboost_date_time

buildtype := release

ifeq ($(buildtype), debug)
	CFLAGS += $(CFLAGS_DEBUG)
else ifeq ($(buildtype), release)
	CFLAGS += $(CFLAGS_RELEASE)
else
	$(error buildtype must be debug or release)
endif

LIBS = 
INCLUDE = -I./include -I./ext/cb_tree/include

TARGETDIR = ./bin/$(buildtype)
TARGET = $(TARGETDIR)/kumori
SRCDIR = ./src

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR = ./obj/$(buildtype)
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS = $(OBJECTS:.o=.d)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS) $(LIBS)
	-mkdir -p $(TARGETDIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.PHONY: clean
clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
