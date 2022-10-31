functions.o: functions.h functions.c
	gcc -c functions.c

shell.o: functions.h shell.c
	gcc -c shell.c 

shell: functions.o shell.o
	gcc shell.o functions.o -o cs345sh
all: shell

clean:
	rm *.o
	rm cs345sh
