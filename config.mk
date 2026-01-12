# Configure Makefile environment
ifndef pname
$(error Must provide variable: $$(pname))
endif
export TOP := $(CURDIR)
export build := $(TOP)/build
# output directories
export BINDEST := $(build)/bin
export LIBDEST := $(build)/lib
ifndef out
# updates src/_generated/sys_paths.h
out := $(build)
endif
export RESDEST := $(out)/share/$(pname)

# Default flags
CXXFLAGS_DEBUG := -g -ggdb -DDEBUG
CXXFLAGS_RELEASE := -O3 -Wall
# Global CXXFLAGS
export CXXFLAGS := -std=c++20 -Wall $(CXXFLAGS_DEBUG)
