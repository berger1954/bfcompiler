all: stack sourcelist assembly compiler

stack: stack.c stack.h
	gcc stack.c -c -o stack.o

sourcelist: source_list.c source_list.h
	gcc source_list.c -c -o source_list.o

assembly: assembly.c assembly.h
	gcc assembly.c -c -o assembly.o

compiler: stack sourcelist
	gcc -std=gnu99 main.c stack.o source_list.o assembly.o -o bf

clean:
	rm stack.o source_list.o assembly.o compiler.exe
