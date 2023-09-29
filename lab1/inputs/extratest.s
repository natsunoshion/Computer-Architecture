.text 
main:

  addi $t0, $zero, 1     # $t0 = 1
  addi $t1, $zero, 2     # $t1 = 2

  slti $t2, $t0, 5       # $t2 = 1, $t0 < 5
  bne $t2, $zero, Label1 # 跳转执行,因为$t2!=0

  addi $t0, $t0, 3       # $t0 = 4
  sltiu $t3, $t0, 5      # $t3 = 0, $t0 unsigned >= 5
  beq $t3, $zero, Label2 # 跳转执行,因为$t3==0

Label2:
  andi $t4, $t0, 1       # $t4 = 0, $t0和1按位与
  beq $t4, $zero, Label3 # 跳转执行,因为$t4==0
  
Label3:  
  ori $t5, $t0, 1        # $t5 = 5, $t0和1按位或

  xor $t6, $t5, $t0      # $t6 = 1, $t5和$t0按位异或
  bne $t6, $zero, Label4 # 跳转执行,因为$t6!=0

Label4:
  lui $t7, 0x1234        # $t7 = 0x12340000
  
  lb $t8, 0($t1)         # 加载字节
  lbu $t9, 0($t1)        # 无符号加载字节

  lh $s0, 0($t1)         # 加载半字
  lhu $s1, 0($t1)        # 无符号加载半字

  lw $s2, 0($t1)         # 加载字

  sb $t8, 0($t1)         # 存储字节
  sh $s0, 0($t1)         # 存储半字
  sw $s2, 0($t1)         # 存储字
  
  bltzal $t0, Label5     # 如果$t0<0调用Label5
  nop                    # 延迟槽
  addi $t0, $t0, 1       # $t0加1  

Label5: 
  jal Label6             # 跳转连接调用Label6
  nop                    # 延迟槽

Label6:
  jr $ra                 # 返回 

  mult $t0, $t1          # 乘法
  mflo $t2               # 取乘法结果  
  mfhi $t3               # 取乘法高位结果

  div $t2, $t0           # 除法
  mflo $t4               # 取除法结果

  add $t5, $t0, $t1      # 加法 
  addu $t6, $t0, $t1     # 无溢出加法

  sub $t7, $t0, $t1      # 减法
  subu $t8, $t0, $t1     # 无溢出减法

  sllv $t9, $t0, $t1     # 逻辑左移变址
  srlv $s0, $t0, $t1     # 逻辑右移变址 
  srav $s1, $t0, $t1     # 算术右移变址

  Label1:
  b Label7               # 无条件跳转

Label7:  

syscall