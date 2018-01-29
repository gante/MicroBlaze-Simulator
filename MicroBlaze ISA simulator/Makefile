GCC=mingw32-gcc.exe
CFLAGS=-Wall -O2

all: main cleanup
 
main: device.o instruction_set.o main.o menu.o
	$(GCC) -o MicroBlaze_ISA_simulator.exe device.o instruction_set.o main.o menu.o
	
menu.o: menu.c general_includes.h menu.h
	$(GCC) $(CFLAGS) -c menu.c
	
device.o: device.c general_includes.h menu.h device.h
	$(GCC) $(CFLAGS) -c device.c 
	
instruction_set.o: instruction_set.c general_includes.h instruction_set.h
	$(GCC) $(CFLAGS) -c instruction_set.c 
	
main.o: main.c general_includes.h menu.h device.h instruction_set.h
	$(GCC) $(CFLAGS) -c main.c 
	
cleanup:
	del *.o