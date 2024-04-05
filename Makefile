																																						
exec: shell.o 
	gcc -o exec shell.o  
	
shell.o :projet.c  
	gcc -o shell.o -c projet.c 
clean:
	rm -rf *.o
