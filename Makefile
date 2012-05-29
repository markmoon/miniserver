OBJS = smarthome.o net.o file_lock.o user_examine.o proc_packet.o controls.o alarm_server.o seri.o

CC = arm-linux-gcc

CFLAGS = -Wall -g -L.

markmoon : $(OBJS)
	$(CC) $^ -o $@ -lsqlite3

smarthome.o : smarthome.c types.h define.h net.h user_examine.h proc_packet.h alarm_server.h

net.o : net.c types.h define.h net.h

file_lock.o : file_lock.c file_lock.h 

user_examine.o : user_examine.c types.h define.h net.h user_examine.h file_lock.h

proc_packet.o : proc_packet.c proc_packet.h controls.h

alarm_server.o : alarm_server.c alarm_server.h

controls.o : controls.c controls.h 

seri.o : seri.c seri.h

clean :
	rm markmoon $(OBJS) 