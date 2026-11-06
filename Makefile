# Makefile for unix systems
# this requires GNU make

APPNAME=main
APPNAME2=main2

# Compiler flags
# enable all warnings in general
WARNFLAGS= -Wall -Wno-comment

SHAREDFLAGS = $(WARNFLAGS) -pthread

ifeq ($(RELEASE), 1)
CFLAGS =   $(SHAREDFLAGS) -ffast-math -s -O4 -DNDEBUG
CXXFLAGS = $(SHAREDFLAGS) -ffast-math -s -O4 -DNDEBUG
else
CFLAGS =   $(SHAREDFLAGS) -g
CXXFLAGS = $(SHAREDFLAGS) -g
endif

# include paths for the builtin libraries glad and glm. We do not link them,
# as we directly incorporated the source code into our project
CPPFLAGS += -I glm/

# Try to find the system's GLFW3 library via pkg-config
CPPFLAGS += $(shell pkg-config --cflags glfw3)
LDFLAGS += $(shell pkg-config --static --libs glfw3) 

# additional libraries
LDFLAGS += -lrt -lm -lvulkan

CFILES=$(wildcard *.c)
CPPFILES=$(wildcard *.cpp)
INCFILES=$(wildcard *.h)	
SRCFILES = $(CFILES) $(CPPFILES)
PRJFILES = Makefile $(wildcard *.vcxproj) $(wildcard *.sln)
ALLFILES = $(SRCFILES) $(INCFILES) $(PRJFILES)
COMMON_CPPFILES = $(filter-out main.cpp main2.cpp,$(CPPFILES))
COMMON_OBJECTS = $(patsubst %.cpp,%.o,$(COMMON_CPPFILES)) $(patsubst %.c,%.o,$(CFILES))
OBJECTS = $(patsubst %.cpp,%.o,$(CPPFILES)) $(patsubst %.c,%.o,$(CFILES))
	   
# build rules
.PHONY: all shaders
all:	shaders $(APPNAME) $(APPNAME2)

shaders: shaders/vert.spv shaders/frag.spv shaders/particle_comp.spv shaders/particle_vert.spv shaders/particle_frag.spv shaders/hypercube_vert.spv shaders/hypercube_frag.spv

shaders/vert.spv: shaders/26_shader_textures.vert
	glslc $< -o $@

shaders/frag.spv: shaders/26_shader_textures.frag
	glslc $< -o $@

shaders/particle_comp.spv: shaders/particle.comp
	glslc $< -o $@

shaders/particle_vert.spv: shaders/particle.vert
	glslc $< -o $@

shaders/particle_frag.spv: shaders/particle.frag
	glslc $< -o $@

shaders/hypercube_vert.spv: shaders/hypercube.vert
	glslc $< -o $@

shaders/hypercube_frag.spv: shaders/hypercube.frag
	glslc $< -o $@

# build and start with "make run" or "make run2"
.PHONY: run run2
run:	all
	./$(APPNAME)

run2:	all
	./$(APPNAME2)

# automatic dependency generation
# create $(DEPDIR) (and an empty file dir)
# create a .d file for every .c source file which contains
# 		   all dependencies for that file
DEPDIR   = ./dep
.PHONY: depend
depend:	$(DEPDIR)/dependencies
DEPFILES = $(patsubst %.c,$(DEPDIR)/%.d,$(CFILES)) $(patsubst %.cpp,$(DEPDIR)/%.d,$(CPPFILES))
$(DEPDIR)/dependencies: $(DEPDIR)/dir $(DEPFILES)
	@cat $(DEPFILES) > $(DEPDIR)/dependencies
$(DEPDIR)/dir:
	@mkdir -p $(DEPDIR)
	@mkdir -p $(DEPDIR)/glad/src
	@touch $(DEPDIR)/dir
$(DEPDIR)/%.d: %.c $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CC) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
$(DEPDIR)/%.d: %.cpp $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CXX) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
-include $(DEPDIR)/dependencies

# rules to build applications
$(APPNAME): main.o $(COMMON_OBJECTS) $(DEPDIR)/dependencies
	$(CXX) $(CFLAGS) main.o $(COMMON_OBJECTS) $(LDFLAGS) -o$(APPNAME)

$(APPNAME2): main2.o $(COMMON_OBJECTS) $(DEPDIR)/dependencies
	$(CXX) $(CFLAGS) main2.o $(COMMON_OBJECTS) $(LDFLAGS) -o$(APPNAME2)

# remove all unneeded files
.PHONY: clean
clean:
	@echo removing binaries: $(APPNAME) $(APPNAME2)
	@rm -f $(APPNAME) $(APPNAME2)
	@echo removing object files: $(OBJECTS)
	@rm -f $(OBJECTS)
	@echo removing dependency files
	@rm -rf $(DEPDIR)
	@echo removing tags
	@rm -f tags

# update the tags file
.PHONY: TAGS
TAGS:	tags

tags:	$(SRCFILES) $(INCFILES) 
	ctags $(SRCFILES) $(INCFILES)

# look for 'TODO' in all relevant files
.PHONY: todo
todo:
	-egrep -in "TODO" $(SRCFILES) $(INCFILES)

