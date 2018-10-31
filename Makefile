all:chatserv chatcli

chatserv.o:chatserv.c
	gcc -c -g chatserv.c -o chatserv.o
chatcli.o:chatcli.c
	gcc -c -g chatcli.c -o chatcli.o
chatserv:chatserv.o 
	gcc chatserv.o -g -o chatserv
chatcli:chatcli.o 
	gcc chatcli.o -g -o chatcli

clean:
	@rm *.o
	@rm chatserv
	@rm chatcli
