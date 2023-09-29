    .text
test1:	
		addiu $a0, $zero, 20          # $a0 = 20
		addiu $a1, $a0, 30            # $a1 = $a0 + 30 = 50
		addiu $a2, $a1, 40            # $a2 = $a1 + 40 = 90
		addu   $a3, $a2, $a0          # $a3 = $a2 + $a0 = 110
		or     $t0, $a3, $a1          # $t0 = $a3 | $a1
		andi   $t1, $t0, 255          # $t1 = $t0 & 255
		sll    $t2, $t1, 3            # $t2 = $t1 << 3
		sub    $t3, $zero, $t2        # $t3 = -$t2
		xor    $t4, $t3, $t0          # $t4 = $t3 ^ $t0
		addiu  $v0, $zero, 0x4        # $v0 = 4
		syscall
