.entry MISCSTART
.extern MISCFUNC
MISCOP:     red @r1
             jsr STR9
             stop
MISCSTART:   inc NUM9
             rts
STR9:       .string "misctest"
NUM9:       .data 18, 19, 20