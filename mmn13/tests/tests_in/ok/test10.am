; Jumping and branching tests
.entry JUMPSTART
.extern EXTERNJUMP
JUMPMAIN:   mov @r2, JUMPDATA
JUMPLOOP:   jmp L2
             jsr EXTERNJUMP
JUMPSTART:   prn -3
             bne L3
L2:          dec DATAARR
             rts
L3:          inc KVAL
             stop
STRJMP:     .string "jmptest"
JUMPDATA:   .data 15, -10, 20
DATAARR:    .data 30, 40, 50
KVAL:       .data 10