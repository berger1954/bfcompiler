# bfcompiler
## A transpiler for BrainFuck
bfcompiler is a transpiler for the esoteric language Brainfuck targeting x86 assembly. It is entirely written in C and its code generation outputs assembly that I've written.

## About

### Optimizations:

In its current form, this program performs very few optimizations on the code before outputing assembly. It only checks for repeated incrementations and condensces them into a single addition. There is no assignment of registers, so everything is read from and written to RAM.

### Implementation:

Brainfuck is a simple language that can be compiled symbol by symbol while using automatically generated labels to serve as targets for jump instructions. As such, it does not implement an AST and instead uses a lookup to convert each symbol to the equivalent ASM. If an AST were used, there could be more advanced optimizations, but wasn't necessary at the time.
