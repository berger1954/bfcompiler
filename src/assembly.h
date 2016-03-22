#ifndef _ASSEMBLYCODE
#define _ASSEMBLYCODE

#include "source_list.h"
#include <stdio.h>
#include <stdlib.h>

char* introassembly;

char* exitassembly;

int writeassembly(list* program, FILE* fd);

#endif