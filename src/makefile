#######################
# metabolism makefile #
#######################

HEADERS = atom.h \
			 element.h \
			 options.h \
			 reaction.h \
			 safecalls.h \
			 sim.h
SOURCES = atom.cpp \
			 element.cpp \
			 main.cpp \
			 options.cpp \
			 reaction.cpp \
			 safecalls.cpp \
			 sim-engine.cpp \
			 sim-io.cpp \
			 ../SFMT/SFMT.c
OBJECTS = atom.o \
			 element.o \
			 main.o \
			 options.o \
			 reaction.o \
			 safecalls.o \
			 sim-engine.o \
			 sim-io.o \
			 SFMT.o

DEFINES = -D_REENTRANT -DMEXP=132049 -DGIT_TAG=`git describe --tags | sed "s/\(.*\)/\"\1\"/"`
FLAGS   = -pipe -O3 -Wall -W
INCPATH = -I. -I../SFMT
LIBS    = -L/usr/lib

# OS-specific flags
OS        := $(shell sh -c 'uname -s 2>/dev/null || echo not')
PROCESSOR := $(shell sh -c 'uname -p 2>/dev/null || echo not')
ifeq ($(OS),Linux) # Linux
  DEFINES += -DBLR_USELINUX -DHAVE_SSE2
  LFLAGS = -Wl,-O1
endif
ifeq ($(OS),Darwin) # Mac
  DEFINES += -DBLR_USEMAC
  ifneq ($(PROCESSOR),powerpc) # Not PPC
    DEFINES += -DHAVE_SSE2
  endif
endif

# File locations used for make bless and make check
CHECK_LOAD      = ../load/mm.load
CHECK_CONFIG    = ../check/config.check.out
CHECK_CENSUS    = ../check/census.check.out
CHECK_DIFFUSION = ../check/diffusion.check.out
CHECK_RAND      = ../check/rand.check.out



default: metabolism

metabolism: qt-makefile $(SOURCES) $(HEADERS)
	make -f $<
metabolism-no-ncurses: qt-makefile-no-ncurses $(SOURCES) $(HEADERS)
	make -f $<
metabolism-no-qt: DEFINES+=-DHAVE_NCURSES
metabolism-no-qt: LIBS+=-lncurses
metabolism-no-qt metabolism-no-qt-ncurses: $(OBJECTS) 
	g++ $(LFLAGS) $(LIBS) -o $@ $(OBJECTS)


bless: clean metabolism
	./metabolism --gui-off --load $(CHECK_LOAD) --iters 100 -x 128 -y 128 --atoms 500 --seed 42 --shuffle \
		--files $(CHECK_CONFIG) $(CHECK_CENSUS) $(CHECK_DIFFUSION) $(CHECK_RAND)

check: clean metabolism-no-qt-ncurses
	./metabolism-no-qt-ncurses --load $(CHECK_CONFIG)
	diff census.out $(CHECK_CENSUS) | wc -l
	diff diffusion.out $(CHECK_DIFFUSION) | wc -l
	diff rand.out $(CHECK_RAND) | wc -l

clean:
	rm -f metabolism metabolism-no-* qt-makefile* *.o *.out *.pdf *~


qt-makefile: qt-project.pro $(SOURCES) $(HEADERS)
	qmake -o $@
qt-makefile-no-ncurses: qt-project.pro $(SOURCES) $(HEADERS)
	qmake -o $@ "CONFIG += no-ncurses"


atom.o: atom.cpp atom.h \
		element.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

element.o: element.cpp element.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

main.o: main.cpp \
		options.h \
		safecalls.h \
		sim.h \
		atom.h \
		element.h \
		reaction.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

options.o: options.cpp options.h \
		safecalls.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

reaction.o: reaction.cpp reaction.h \
		element.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

safecalls.o: safecalls.cpp safecalls.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

sim-engine.o: sim-engine.cpp sim.h \
		safecalls.h \
		atom.h \
		element.h \
		options.h \
		reaction.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

sim-io.o: sim-io.cpp sim.h \
		safecalls.h \
		atom.h \
		element.h \
		options.h \
		reaction.h
	g++ -c $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

SFMT.o: ../SFMT/SFMT.c ../SFMT/SFMT.h \
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
	gcc -c -msse2 $(FLAGS) $(DEFINES) $(INCPATH) -o $@ $<

# End

