##############################################################
# metabolism makefile                                        #
# ========================================================== #
# Available targets:                                         #
#    metabolism                                              #
#    metabolism-qt                                           #
#    metabolism-ncurses                                      #
#    metabolism-minimal                                      #
#    metabolism-debug                                        #
#    all                                                     #
#    bless                                                   #
#    check                                                   #
#    profile                                                 #
#    clean                                                   #
##############################################################

# List of executables that can be built
APPS = metabolism \
		 metabolism-qt \
		 metabolism-ncurses \
		 metabolism-minimal \
		 metabolism-debug


# When a target is not specified, the default executable is
# built
default: metabolism


# Specifying the 'all' target will build every executable
all: $(APPS)



ifndef OBJDIR
##############################################################
# FIRST PASS:                                                #
# When 'make' is run from the command line, OBJDIR should    #
# not be defined, and the makefile will determine where      #
# object files should be placed before running 'make' again  #
# with OBJDIR defined                                        #
##############################################################

# Name of the main build directory
BUILD = build


# Set OBJDIR to a unique path for the executable that is
# being compiled, create the build directory if necessary,
# and run 'make' again with OBJDIR defined
$(APPS): OBJDIR=$(BUILD)/$@
$(APPS): FORCE
	-mkdir -p $(OBJDIR)
	make $@ OBJDIR=$(OBJDIR)

FORCE:


# File locations used for 'make bless' and 'make check'
CHECK_LOAD      = ../load/mm.load
CHECK_CONFIG    = ../check/config.check.out
CHECK_CENSUS    = ../check/census.check.out
CHECK_DIFFUSION = ../check/diffusion.check.out
CHECK_RAND      = ../check/rand.check.out


# Target for creating output files that should be considered
# "correct" and saved for later reference; should only be
# run by the code maintainer when a part of the simulation
# algorithm or data dumping procedures has been changed such
# that one would expect it to affect the results
bless: metabolism
	./$< --gui-off --load $(CHECK_LOAD) --iters 100 -x 128 -y 128 --seed 42 --shuffle \
		--files $(CHECK_CONFIG) $(CHECK_CENSUS) $(CHECK_DIFFUSION) $(CHECK_RAND)


# Target for running the same settings as 'make bless' and
# comparing the results on a different machine; zero
# deviations should be found if the simulation is producing
# identical results across tested systems
check: metabolism-minimal
	./$< --load $(CHECK_CONFIG)
	@echo "config.out:   " `diff -I version config.out $(CHECK_CONFIG) | wc -l` "deviations"
	@echo "census.out:   " `diff census.out $(CHECK_CENSUS) | wc -l` "deviations"
	@echo "diffusion.out:" `diff diffusion.out $(CHECK_DIFFUSION) | wc -l` "deviations"
	@echo "rand.out:     " `diff rand.out $(CHECK_RAND) | wc -l` "deviations"


# Target for running a simulation and analyzing profiling
# data to assist with optimization
profile: metabolism-debug
	./$< --iters 50 -x 1000 -y 1000
	gprof $< > gprof.out
	cat gprof.out | less


# Target for removing all files and directories created by
# 'make'
clean:
	rm -rf metabolism* $(BUILD)/ qt-makefile* *.out *.pdf *.png *.tex *~



else
##############################################################
# SECOND PASS:                                               #
# OBJDIR will be defined when 'make' is called a second time #
# (from the makefile during the first pass) and compilation  #
# can begin                                                  #
##############################################################

# List source code files used, separating Qt-dependent files
# from Qt-independent files
HEADERS    = atom.h \
				 boost-devices.h \
			 	 element.h \
			 	 options.h \
			 	 reaction.h \
			 	 safecalls.h \
			 	 sim.h
QT_HEADERS = plot.h \
				 viewer.h \
				 window.h
SOURCES    = atom.cpp \
			    element.cpp \
				 main.cpp \
			 	 options.cpp \
			 	 reaction.cpp \
			 	 safecalls.cpp \
			 	 ../SFMT/SFMT.c \
			 	 sim-engine.cpp \
			 	 sim-io.cpp
QT_SOURCES = plot.cpp \
				 viewer.cpp \
				 window.cpp


# Create a list of object files that will be built and
# linked together to construct an execuatable for
# Qt-independent targets
OBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))


# Specify the project file used when compiling with Qt
QT_PROJECT = qt-project.pro


# Define various compilation flags used by all targets;
# GIT_TAG stores the simulation version number, and MEXP is
# used by the random number generator
GIT_TAG := $(shell git describe --tags)
DEFINES = MEXP=132049
FLAGS   = -pipe -O3 -Wall -W
LIBS    =
INCPATH = ..


# Determine details about the system and define OS-specific
# compilation flags
OS        := $(shell sh -c 'uname -s 2>/dev/null || echo not')
PROCESSOR := $(shell sh -c 'uname -p 2>/dev/null || echo not')
ifeq ($(OS),Linux)             # Linux
  DEFINES += BLR_USELINUX HAVE_SSE2
  LFLAGS = -Wl,-O1
  SPEC = linux-g++
endif
ifeq ($(OS),Darwin)            # Mac
  DEFINES += BLR_USEMAC
  SPEC = macx-g++
  ifneq ($(PROCESSOR),powerpc) # Intel Mac
    MACTARGET = intel
    DEFINES += HAVE_SSE2
  else                         # PowerPC Mac
    MACTARGET = ppc
  endif
endif


# Define target-specific compilation flags; the crazy
# backslash escape sequences used with GIT_TAG are needed
# when compiling with Qt because in these cases the string
# will pass through additional parsers, such as 'qmake', and
# the quotation marks must be preserved
metabolism: DEFINES+=HAVE_QT HAVE_NCURSES GIT_TAG=\\\\\\\"$(GIT_TAG)\\\\\\\"
metabolism: LIBS+=-lncurses

metabolism-qt: DEFINES+=HAVE_QT GIT_TAG=\\\\\\\"$(GIT_TAG)\\\\\\\"

metabolism-ncurses: DEFINES+=HAVE_NCURSES GIT_TAG=\"$(GIT_TAG)\"
metabolism-ncurses: LIBS+=-lncurses

metabolism-minimal: DEFINES+=GIT_TAG=\"$(GIT_TAG)\"

metabolism-debug: DEFINES+=GIT_TAG=\"$(GIT_TAG)\"
metabolism-debug: FLAGS+=-g -pg
metabolism-debug: LFLAGS+=-g -pg


# Specify the dependencies and build rules for the
# executables; Qt-dependent executables are built by running
# 'make' on a makefile generated by 'qmake'; Qt-independent
# executables build object files and link them together
# below
metabolism: qt-makefile $(SOURCES) $(QT_SOURCES) $(HEADERS) $(QT_HEADERS)
	make -f $<
metabolism-qt: qt-makefile-no-ncurses $(SOURCES) $(QT_SOURCES) $(HEADERS) $(QT_HEADERS)
	make -f $<
metabolism-ncurses metabolism-minimal metabolism-debug: $(OBJECTS)
	g++ $(LFLAGS) $(LIBS) -o $@ $^


# Specify the dependencies and build rules for the makefiles
# generated by 'qmake' for Qt-dependent executables
qt-makefile: $(QT_PROJECT) $(SOURCES) $(QT_SOURCES) $(HEADERS) $(QT_HEADERS)
	qmake -o $@ -spec $(SPEC) \
		"TARGET=metabolism" \
		"DEFINES+=$(DEFINES)" \
		"INCLUDEPATH+=$(INCPATH)" \
		"LIBS+=$(LIBS)" \
		"MACTARGET=$(MACTARGET)" \
		"OBJECTS_DIR=$(OBJDIR)" \
		"MOC_DIR=$(OBJDIR)" \
		"HEADERS=$(HEADERS) $(QT_HEADERS)" \
		"SOURCES=$(SOURCES) $(QT_SOURCES)"
qt-makefile-no-ncurses: $(QT_PROJECT) $(SOURCES) $(QT_SOURCES) $(HEADERS) $(QT_HEADERS)
	qmake -o $@ -spec $(SPEC) \
		"TARGET=metabolism-qt" \
		"DEFINES+=$(DEFINES)" \
		"INCLUDEPATH+=$(INCPATH)" \
		"LIBS+=$(LIBS)" \
		"MACTARGET=$(MACTARGET)" \
		"OBJECTS_DIR=$(OBJDIR)" \
		"MOC_DIR=$(OBJDIR)" \
		"HEADERS=$(HEADERS) $(QT_HEADERS)" \
		"SOURCES=$(SOURCES) $(QT_SOURCES)"


# Define the generic rule for building .o object files from
# .cpp source files for Qt-independent executables
$(OBJDIR)/%.o: %.cpp
	g++ -c $(FLAGS) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCPATH)) -o $@ $<


# Specify dependencies for all object files and include a
# rule for compiling the .c source file
$(OBJDIR)/atom.o: atom.cpp \
		atom.h \
		element.h

$(OBJDIR)/element.o: element.cpp \
		element.h

$(OBJDIR)/main.o: main.cpp \
		atom.h \
		element.h \
		options.h \
		plot.h \
		reaction.h \
		sim.h \
		viewer.h \
		window.h

$(OBJDIR)/options.o: options.cpp \
		options.h \
		safecalls.h

$(OBJDIR)/reaction.o: reaction.cpp \
		element.h \
		reaction.h

$(OBJDIR)/safecalls.o: safecalls.cpp \
		safecalls.h

$(OBJDIR)/SFMT.o: ../SFMT/SFMT.c \
		../SFMT/SFMT.h \
		../SFMT/SFMT-params.h \
		../SFMT/SFMT-params607.h \
		../SFMT/SFMT-params1279.h \
		../SFMT/SFMT-params2281.h \
		../SFMT/SFMT-params4253.h \
		../SFMT/SFMT-params11213.h \
		../SFMT/SFMT-params19937.h \
		../SFMT/SFMT-params44497.h \
		../SFMT/SFMT-params86243.h \
		../SFMT/SFMT-params132049.h \
		../SFMT/SFMT-params216091.h \
		../SFMT/SFMT-alti.h \
		../SFMT/SFMT-sse2.h
	gcc -c -msse2 $(FLAGS) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCPATH)) -o $@ $<

$(OBJDIR)/sim-engine.o: sim-engine.cpp \
		atom.h \
		element.h \
		options.h \
		reaction.h \
		sim.h

$(OBJDIR)/sim-io.o: sim-io.cpp \
		atom.h \
		boost-devices.h \
		element.h \
		options.h \
		reaction.h \
		sim.h

endif

# End

