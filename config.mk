# Configure Makefile environment
ifndef pname
$(error Must provide variable: $$(pname))
endif
export TOP := $(CURDIR)
export build := $(TOP)/build
ifndef out
	out := $(build)
endif
# resource destination, gets hard-coded into binary
export RESDEST := $(out)/share/$(pname)

# Default flags
export MAKE := $(MAKE) --no-print-directory
CXXFLAGS_DEBUG := -g -ggdb -DDEBUG
CXXFLAGS_RELEASE := -O3 -Wall
# Global CXXFLAGS
export CXXFLAGS := -std=c++20 -Wall $(CXXFLAGS_DEBUG)
