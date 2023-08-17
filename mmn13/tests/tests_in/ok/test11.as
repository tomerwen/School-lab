; Testing string operations
.entry STRINGOPSTART
.extern STRFUNC
STRINGOPSTART:   lea STR8, @r6
             prn STR8
             stop
STR8:       .string "stroptest"
NUMVAL:     .data 12, 14, 16