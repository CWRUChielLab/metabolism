#######################
# metabolism makefile #
#######################

APPS = metabolism \
		 metabolism-qt \
		 metabolism-ncurses \
		 metabolism-minimal \
		 metabolism-debug

default: metabolism

all: $(APPS)



ifndef OBJDIR

BUILD = build

$(APPS): OBJDIR=$(BUILD)/$@
$(APPS): FORCE
	-mkdir -p $(OBJDIR)
	make $@ OBJDIR=$(OBJDIR)

FORCE:

# File locations used for make bless and make check
CHECK_LOAD      = ../load/mm.load
CHECK_CONFIG    = ../check/config.check.out
CHECK_CENSUS    = ../check/census.check.out
CHECK_DIFFUSION = ../check/diffusion.check.out
CHECK_RAND      = ../check/rand.check.out

bless: metabolism
	./metabolism --gui-off --load $(CHECK_LOAD) --iters 100 -x 128 -y 128 --atoms 500 --seed 42 --shuffle \
		--files $(CHECK_CONFIG) $(CHECK_CENSUS) $(CHECK_DIFFUSION) $(CHECK_RAND)

check: metabolism-minimal
	./metabolism-minimal --load $(CHECK_CONFIG)
	@echo
	@echo "census.out:" `diff census.out $(CHECK_CENSUS) | wc -l` "deviations"
	@echo "diffusion.out:" `diff diffusion.out $(CHECK_DIFFUSION) | wc -l` "deviations"
	@echo "rand.out:" `diff rand.out $(CHECK_RAND) | wc -l` "deviations"

profile: metabolism-debug
	./$< --iters 50 --atoms 25000 -x 1000 -y 1000
	gprof $< > profile.out
	cat profile.out | less

clean:
	rm -rf $(APPS) $(BUILD)/ qt-makefile* *.out *.pdf *~



else

HEADERS    = atom.h \
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
OBJECTS    = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
QT_PROJECT = qt-project.pro

GIT_TAG = $(shell git describe --tags)
DEFINES = MEXP=132049
FLAGS   = -pipe -O3 -Wall -W
LIBS    = -L/usr/lib
INCPATH = -I. -I../SFMT

# OS-specific flags
OS        := $(shell sh -c 'uname -s 2>/dev/null || echo not')
PROCESSOR := $(shell sh -c 'uname -p 2>/dev/null || echo not')
ifeq ($(OS),Linux) # Linux
  DEFINES += BLR_USELINUX HAVE_SSE2
  LFLAGS = -Wl,-O1
  SPEC = linux-g++
endif
ifeq ($(OS),Darwin) # Mac
  DEFINES += BLR_USEMAC
  SPEC = macx-g++
  ifneq ($(PROCESSOR),powerpc) # Intel Mac
    MACTARGET = intel
    DEFINES += HAVE_SSE2
  else                         # PowerPC Mac
    MACTARGET = ppc
  endif
endif



$(OBJDIR)/%.o: %.cpp
	g++ -c $(FLAGS) $(addprefix -D, $(DEFINES)) $(INCPATH) -o $@ $<


metabolism: DEFINES+=HAVE_QT HAVE_NCURSES GIT_TAG=\\\\\\\"$(GIT_TAG)\\\\\\\"
metabolism: LIBS+=-lncurses

metabolism-qt: DEFINES+=HAVE_QT GIT_TAG=\\\\\\\"$(GIT_TAG)\\\\\\\"

metabolism-ncurses: DEFINES+=HAVE_NCURSES GIT_TAG=\"$(GIT_TAG)\"
metabolism-ncurses: LIBS+=-lncurses

metabolism-minimal: DEFINES+=GIT_TAG=\"$(GIT_TAG)\"

metabolism-debug: DEFINES+=GIT_TAG=\"$(GIT_TAG)\"
metabolism-debug: FLAGS+=-g -pg
metabolism-debug: LFLAGS+=-g -pg


metabolism: qt-makefile $(SOURCES) $(QT_SOURCES) $(HEADERS) $(QT_HEADERS)
	make -f $<
metabolism-qt: qt-makefile-no-ncurses $(SOURCES) $(QT_SOURCES) $(HEADERS) $(QT_HEADERS)
	make -f $<
metabolism-ncurses metabolism-minimal metabolism-debug: $(OBJECTS)
	g++ $(LFLAGS) $(LIBS) -o $@ $^


qt-makefile: $(QT_PROJECT) $(SOURCES) $(QT_SOURCES) $(HEADERS) $(QT_HEADERS)
	qmake -o $@ -spec $(SPEC) \
		"TARGET=metabolism" \
		"DEFINES+=$(DEFINES)" \
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
		"LIBS+=$(LIBS)" \
		"MACTARGET=$(MACTARGET)" \
		"OBJECTS_DIR=$(OBJDIR)" \
		"MOC_DIR=$(OBJDIR)" \
		"HEADERS=$(HEADERS) $(QT_HEADERS)" \
		"SOURCES=$(SOURCES) $(QT_SOURCES)"


$(OBJDIR)/atom.o: atom.cpp atom.h \
		element.h

$(OBJDIR)/element.o: element.cpp element.h

$(OBJDIR)/main.o: main.cpp \
		atom.h \
		element.h \
		options.h \
		reaction.h \
		safecalls.h \
		sim.h \
		viewer.h \
		window.h

$(OBJDIR)/options.o: options.cpp options.h \
		safecalls.h

$(OBJDIR)/reaction.o: reaction.cpp reaction.h \
		element.h

$(OBJDIR)/safecalls.o: safecalls.cpp safecalls.h

$(OBJDIR)/SFMT.o: ../SFMT/SFMT.c ../SFMT/SFMT.h \
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
	gcc -c -msse2 $(FLAGS) $(addprefix -D, $(DEFINES)) $(INCPATH) -o $@ $<

$(OBJDIR)/sim-engine.o: sim-engine.cpp sim.h \
		atom.h \
		element.h \
		options.h \
		reaction.h \
		safecalls.h

$(OBJDIR)/sim-io.o: sim-io.cpp sim.h \
		atom.h \
		element.h \
		options.h \
		reaction.h \
		safecalls.h

endif

# End

