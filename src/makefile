all: stack sourcelist compiler

stack: stack.c stack.h
	gcc stack.c -c -o stack.o

sourcelist: source_list.c source_list.h
	gcc source_list.c -c -o source_list.o

compiler: stack sourcelist
	gcc -std=gnu99 compiler.c stack.o source_list.o -o bf

clean:
	rm stack.o source_list.o compiler.exe
