.entry LENGTH
.extern 100
MAIN: mov LENGTH ,1
LOOP: jmp L1 , @r1
prn 
bne 1
sub @r1, @r4
bne 100
L1: inc 100
.entry LOOP   22
jmp W  @r1
END: stop , @r2
STR: .string "abcdef"
LENGTH: .data 10,-100,1000
K: .data 22
.extern L3