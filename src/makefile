#######################
# metabolism makefile #
#######################

DEFINES = -DMEXP=132049 -DBLR_USELINUX -DHAVE_SSE2 -DGIT_TAG=`git describe --tags | sed "s/\(.*\)/\"\1\"/"`
FLAGS   = -pipe -O3 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH = -I. -I../SFMT
LIBS    = -L/usr/lib

all: metabolism

metabolism: LIBS+=-lncurses

no-ncurses: DEFINES+=-D_NO_NCURSES

metabolism no-ncurses: atom.o element.o main.o options.o reaction.o safecalls.o sim.o SFMT.o
	g++ -Wl,-O1 -o metabolism atom.o element.o main.o options.o reaction.o safecalls.o sim.o SFMT.o $(LIBS)

atom.o: atom.cpp atom.h \
		element.h
	g++ -c $(FLAGS) $(INCPATH) -o atom.o atom.cpp

element.o: element.cpp element.h
	g++ -c $(FLAGS) $(INCPATH) -o element.o element.cpp

main.o: main.cpp \
		options.h \
		safecalls.h \
		sim.h \
		atom.h \
		element.h \
		reaction.h
	g++ -c $(FLAGS) $(INCPATH) -o main.o main.cpp

options.o: options.cpp options.h \
		safecalls.h
	g++ -c $(FLAGS) $(INCPATH) -o options.o options.cpp

reaction.o: reaction.cpp reaction.h \
		element.h
	g++ -c $(FLAGS) $(INCPATH) -o reaction.o reaction.cpp

safecalls.o: safecalls.cpp safecalls.h
	g++ -c $(FLAGS) $(INCPATH) -o safecalls.o safecalls.cpp

sim.o: sim.cpp sim.h \
		safecalls.h \
		atom.h \
		element.h \
		options.h \
		reaction.h
	g++ -c $(FLAGS) $(INCPATH) -o sim.o sim.cpp

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
	gcc -c -msse2 $(FLAGS) $(INCPATH) -o SFMT.o ../SFMT/SFMT.c

clean:
	rm -f atom.o element.o main.o options.o reaction.o safecalls.o sim.o SFMT.o *~

# End
