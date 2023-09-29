    .text
test2:   
        addiu   $t0, $zero, 500       # $t0 = 500
        addu    $t1, $t0, $t0         # $t1 = $t0 + $t0 = 1000
        or      $t2, $t1, $t0         # $t2 = $t1 | $t0 = 1500
        sll     $t3, $t2, 1           # $t3 = $t2 << 1 = 3000
        subu    $t4, $t3, $t1         # $t4 = $t3 - $t1 = 2000
        xor     $t5, $t4, $t2         # $t5 = $t4 ^ $t2
        xori    $t6, $t5, 100         # $t6 = $t5 ^ 100
        srl     $t7, $t6, 2           # $t7 = $t6 >> 2
        sra     $t8, $t7, 1           # $t8 = $t7 >> 1 (arithmetic shift)
        and     $t9, $t8, $t6         # $t9 = $t8 & $t6
        lui     $s0, 50               # $s0 = 50 << 16
        addiu   $v0, $zero, 0x5       # $v0 = 5
        syscall
