Tutorial SmallThink
===================

What is SmallThink ?
--------------------
SmallThink is :
	- a toy language.
	- a script programming language.
	- (a bit) an ASM like language.
SmallThink is not :
	- a good language to start with (in my opinion).
	- a low level language.
	- a performances-oriented language.

SmallThink file extension
-------------------------
SmallThink has no special file extension.
Here are some suggestions :
	- .st
	- .small
	- .smallthink
	- .sthink
	- .sth

Grammar of SmallThink
---------------------
Here is a line of SmallThink :
mov A, 2.0

It is made of opcode ("mov"), the first argument ("A"), a coma (",") and the second argument ("2.0").
The arguments depends of the opcode. There can be two, one or no arguments.
Write only one instruction per line.

The semicolon is used to start a comment. A comment has to be alone on the line. 
; This is a comment

This is not a correct SmallThink line :
mov A, 0.0 ; Initialize A to 0.0

Opcodes
-------
Here is all the opcodes availables :
	- mov [variable name], [value or variable name]
	- add [variable name], [value or variable name]
	- mul [variable name (number)], [value or variable name (number)]
	- cmp_eq [variable name], [value or variable name]
	- cmp_gt [variable name], [value or variable name]
	- cmp_lt [variable name], [value or variable name]

	- neg [variable name (number)]
	- out [value or variable name]
	- in [variable name]
	- get [variable name]
	- num [variable name]
	- str [variable name]
	- num_int [variable name]

	- stop
	- flush
	- seed_random

Please notice theses specials opcodes :
	- label [label name]
	- jmp [label name]
	- jz [label name]
	- jnz [label name]

Description of the opcodes :
	- mov is used to set a variable to a given value.
	- add is used to add two variables or a variable and a value (result stocked in the first variable).
	- mul is used to multiply two variables or a variable and a value (result stocked in the first variable).
	- cmp_eq is used to compare two variables or a variable and a value. IF the two args are equivalents, cmp_register is set to 1, else it is set to 0.
	- cmp_gt is used to compare two variables or a variable and a value. IF the first arg is greater than the second arg, cmp_register is set to 1, else it is set to 0.
	- cmp_lt is used to compare two variables or a variable and a value. IF the first arg is less than the second arg, cmp_register is set to 1, else it is set to 0.

	- neg is used to negative a variable (result stocked in the variable). ex : neg 9 = -9
	- out is used to print a variable or a value.
	- in is used to get an input and stock it in the variable.
	- get is used to get one character from the user and stock it in the variable.
	- num is used to convert a variable into a numeric variable (floating).
	- str is used to convert a variable into a string variable.
	- num_int is used to convert a variable into a numeric variable (integer).

	- stop is used to stop the program.
	- flush is used to flush the standard input.
	- seed_random is used to change the value of specials variables "random_int" and "random_num".

	- label is used to create a new label. A label name is made of a string with no spaces and no quotes. ex : this_is_my_label
	- jmp is used to go to the given label name.
	- jnz is used to go to the given label name, IF variable "cmp_register" is different than 0.
	- jz is used to go to the given label name, IF variable "cmp_register" equals 0.

Arguments
---------
An argument can be of 3 types :
	- A number, in decimal notation : 1, 2.0, -3.0, -4...
	- A string : "Hello world !"
	- A variable name

Congratulations
---------------
That's it, you know how to code in SmallThink ! \o/
Enjoy !
