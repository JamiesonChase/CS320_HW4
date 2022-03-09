#Antonio Maniscalco, Josh Sedig, Chase Jamieson
#CS460 
#Instructor: Dr. Xuechen Zhang
#Makefile

TARGET = prog
FILES = main.c parseArguments.c Threads.c helperFunctions.c 
LDLIBS = -pthread -lm -lrt
CFLAGS = -Wall -Werror 
RM = rm  
JUNK = *.o *~ *dSYM
	
.PHONY: all clean
prog: $(FILES)
	gcc -o $(TARGET) $(FILES) $(CFLAGS) $(LDLIBS)

clean:
	$(RM) $(TARGET)


