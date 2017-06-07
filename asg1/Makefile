MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
GMAKE       = ${MAKE} --no-print-directory

CPP 	    = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEPS	    = g++ -MM -std=gnu++14

CPPSOURCE   = main.cpp  auxlib.cpp string_set.cpp
CPPHEADER   = auxlib.h string_set.h
EXECBIN     = oc
OBJECTS     = ${CPPSOURCE:.cpp=.o}
ALLSOURCES  = ${CPPHEADER} ${CPPSOURCE} ${OTHERS}
LISTING     = Listing.ps
OTHERS	    = ${MKFILE} README PARTNER

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${CPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${CPP} -c $<

ci : ${ALLSOURCES}
	cid + ${ALLSOURCES}
	- checksource ${ALLSOURCES}

lis : ${ALLSOURCES}
	mkpspdf ${LISTING} ${ALLSOURCES} ${DEPFILE}

clean :
	- rm ${OBJECTS} ${DEPFILE} core ${EXECBIN}.errs

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}

again :
	${GMAKE} spotless dep ci all lis

deps : $(CPPSOURCE)
	@ echo "# ${DEPFILE} created 'date' by ${MAKE}" >${DEPFILE}
	${MKDEPS} ${CPPSOURCE} >>${DEPFILE}	
submit :
	submit cmps104a-wm.s17 asg1 ${CPPSOURCE} ${CPPHEADER} ${OTHERS}
