# Another Simple Bytecode
A simple bytecode language I designed with an included assembler and virtual machine interpreter.

Features include:
* 10 32-bit free use registers (r0-r9)
* Ability to input/output characters and numbers to the console as well as print a string to the console
* Most standard operations
* Ability to access memory to store/load variables

Available opcodes:
nop
load constant (lc)
load constant upper (lcu)
load variable (lv)
store variable (str)
push to stack (push)
pop from stack (pop)
branch if not equal (bin)
branch if less than (bil)
branch if equal (bie)
branch if greater (big)
branch without condition (b)
compare (cmp)
syscall (sys)
increment counter (inc)
decrement counter (dec)
copy register (copy)
swap register (swap)
add (add)
subtract (sub)
multiply (mult)
divide (div)
modulus (mod)
or (or)
and (and)
xor (xor)
not (not)
left shift (ls)
right shift (rs)

Available registers:
r0-r9
acc
br  (branch conditional is stored here, cmp has less than = -1, greater than = 1, equal to = 0)
c   (counter)
ss  (stack start)
se  (stack end)

Available syscalls:
0 - pop value and output it to console
1 - pop char and output it to console
2 - input value from console and push it
3 - input char from console and push it
4 - print string from stack
