#include <stdio.h>
#include "shell.h"

// R 型指令操作码：0
#define OP_RTYPE 0x00

// I 型指令
#define OP_BEQ 0x04
#define OP_BNE 0x05
#define OP_BLEZ 0x06
#define OP_BGTZ 0x07
#define OP_ADDI 0x08
#define OP_ADDIU 0x09
#define OP_SLTI 0x0A
#define OP_SLTIU 0x0B
#define OP_ANDI 0x0C
#define OP_ORI 0x0D
#define OP_XORI 0x0E
#define OP_LUI 0x0F
#define OP_LB 0x20
#define OP_LH 0x21
#define OP_LW 0x23
#define OP_LBU 0x24
#define OP_LHU 0x25
#define OP_SB 0x28
#define OP_SH 0x29
#define OP_SW 0x2B

// BLTZ BGEZ BLTZAL BGEZAL 这几个指令特殊，它们也没法通过 opcode 区分
#define OP_BSPECIAL 0x01

// J 型指令
#define OP_J 0x02
#define OP_JAL 0x03

// R 型指令：funct 段
#define FUNCT_SLL 0x00
#define FUNCT_SRL 0x02
#define FUNCT_SRA 0x03
#define FUNCT_SLLV 0x04
#define FUNCT_SRLV 0x06
#define FUNCT_SRAV 0x07
#define FUNCT_JR 0x08
#define FUNCT_JALR 0x09
#define FUNCT_ADD 0x20
#define FUNCT_ADDU 0x21
#define FUNCT_SUB 0x22
#define FUNCT_SUBU 0x23
#define FUNCT_AND 0x24
#define FUNCT_OR 0x25
#define FUNCT_XOR 0x26
#define FUNCT_NOR 0x27
#define FUNCT_SLT 0x2A
#define FUNCT_SLTU 0x2B
#define FUNCT_MULT 0x18
#define FUNCT_MFHI 0x10
#define FUNCT_MFLO 0x12
#define FUNCT_MTHI 0x11
#define FUNCT_MTLO 0x13
#define FUNCT_MULTU 0x19
#define FUNCT_DIV 0x1A
#define FUNCT_DIVU 0x1B
#define FUNCT_JR 0x08
#define FUNCT_JALR 0x09
#define FUNCT_SYSCALL 0x0C

// 特殊的跳转指令：rt 字段
#define RT_BLTZ 0x00
#define RT_BGEZ 0x01
#define RT_BLTZAL 0x10
#define RT_BGEZAL 0x11

// 读取字节
int8_t mem_read_8(uint32_t addr)
{
    uint32_t value = mem_read_32(addr);
    return (int8_t)(value & 0xff);
}

// 读取半字
int16_t mem_read_16(uint32_t addr)
{
    uint32_t value = mem_read_32(addr);
    return (int16_t)(value & 0xffff);
}

// LBU 无符号加载字节
int32_t mem_read_8u(uint32_t addr)
{
    uint32_t value = mem_read_32(addr);
    return (uint32_t)(value & 0xff);
}

// LHU 加载半字作为无符号数
int32_t mem_read_16u(uint32_t addr)
{
    uint32_t value = mem_read_32(addr);
    return (uint32_t)(value & 0xffff);
}

// 写字节
void mem_write_8(uint32_t addr, uint32_t value)
{
    uint32_t original = mem_read_32(addr);
    uint32_t updated = (original & 0xffffff00) | (value & 0xff);
    mem_write_32(addr, updated);
}

// 写半字
void mem_write_16(uint32_t addr, uint32_t value)
{
    uint32_t original = mem_read_32(addr);
    uint32_t updated = (original & 0xffff0000) | (value & 0xffff);
    mem_write_32(addr, updated);
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);

    // 计算字段 op rs rt rd shamt funct imm target 等
    uint8_t op = instruction >> 26 & 0x3F;
    uint8_t rs = instruction >> 21 & 0x1F;
    uint8_t rt = instruction >> 16 & 0x1F;
    uint8_t rd = instruction >> 11 & 0x1F;
    uint8_t shamt = instruction >> 6 & 0x1F;
    uint8_t funct = instruction & 0x3F;
    int16_t imm = instruction & 0xFFFF; // 需要以有符号的形式读入
    uint32_t target = (instruction & 0x3FFFFFF) << 2;
    int32_t extended_imm = (int32_t)imm;
    int32_t offset = extended_imm << 2; // 这里的符号有点坑，直接用 (instruction & 0xFFFF) << 2 是不行的
    uint32_t address = CURRENT_STATE.REGS[rs] + offset;
    // printf("%x ", offset);

    // printf("%x\n", instruction);

    /* 更新 PC，就连 SYSCALL 也一定会增加 4
     * "No registers are modified in either case,
     * except that PC is incremented to the next instruction as usual." */
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    // 按 opcode 区分
    switch (op)
    {

    case OP_RTYPE:

        uint32_t funct = instruction & 0x3F;
        // printf("rtype: %d\n", instruction);

        // 按 funct 字段区分
        switch (funct)
        {
        case FUNCT_SLL:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
            break;

        case FUNCT_SRL:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
            break;

        case FUNCT_SRA:
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> shamt;
            break;

        case FUNCT_SLLV:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << CURRENT_STATE.REGS[rs];
            break;

        case FUNCT_SRLV:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> CURRENT_STATE.REGS[rs];
            break;

        case FUNCT_SRAV:
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> CURRENT_STATE.REGS[rs];
            break;

        // 不考虑溢出异常时，ADD 和 ADDU 等价，SUB 同理
        case FUNCT_ADD:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_ADDU:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_SUB:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_SUBU:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_AND:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_OR:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_XOR:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_NOR:
            NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
            break;

        case FUNCT_SLT:
            if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt])
            {
                NEXT_STATE.REGS[rd] = 1;
            }
            else
            {
                NEXT_STATE.REGS[rd] = 0;
            }
            break;

        case FUNCT_SLTU:
            if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt])
            {
                NEXT_STATE.REGS[rd] = 1;
            }
            else
            {
                NEXT_STATE.REGS[rd] = 0;
            }
            break;

        case FUNCT_MULT:
        { // 这里不打大括号，result 会和下面的重名，无语了
            int64_t result = (int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = result >> 32;
            NEXT_STATE.LO = (int32_t)result;
            break;
        }

        case FUNCT_MFHI:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
            break;

        case FUNCT_MFLO:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
            break;

        case FUNCT_MTHI:
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
            break;

        case FUNCT_MTLO:
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
            break;

        // 虽然写法和 MULT 不同，但其实两者等价
        case FUNCT_MULTU:
        {
            uint64_t result = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = result >> 32;
            NEXT_STATE.LO = (uint32_t)result;
            break;
        }

        case FUNCT_DIV:
            NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
            break;

        // 也是等价的
        case FUNCT_DIVU:
            NEXT_STATE.HI = (uint32_t)CURRENT_STATE.REGS[rs] / (uint32_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (uint32_t)CURRENT_STATE.REGS[rs] % (uint32_t)CURRENT_STATE.REGS[rt];
            break;

        case FUNCT_JR:
            // Jump register
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            break;

        case FUNCT_JALR:
            // Jump and link register
            NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            break;

        case FUNCT_SYSCALL:
            // printf("$v0: %d\n", CURRENT_STATE.REGS[2]);
            if (CURRENT_STATE.REGS[2] == 0x0A)
            {
                // 终止仿真循环
                RUN_BIT = 0;
            }
            break;
        }
        break;

    // J 型的
    case OP_J:
        // Jump
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | target;
        break;

    case OP_JAL:
        // Jump and link
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4; // ra = PC + 4，而不是 PC + 8
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | target;
        break;

    // I 型的
    case OP_ADDI: // 都会进行符号位扩展，不考虑溢出异常时，ADDI 和 ADDIU 等价
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + extended_imm;
        break;

    case OP_ADDIU:
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + extended_imm;
        // printf("%x %x: %x\n", CURRENT_STATE.REGS[rs], extended_imm, NEXT_STATE.REGS[rt]);
        break;

    case OP_SLTI:
        if (CURRENT_STATE.REGS[rs] < extended_imm)
        {
            NEXT_STATE.REGS[rt] = 1;
        }
        else
        {
            NEXT_STATE.REGS[rt] = 0;
        }
        break;

    case OP_SLTIU:
        if (CURRENT_STATE.REGS[rs] < extended_imm)
        {
            NEXT_STATE.REGS[rt] = 1;
        }
        else
        {
            NEXT_STATE.REGS[rt] = 0;
        }
        break;

    case OP_ANDI:
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & extended_imm;
        break;

    case OP_ORI:
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | extended_imm;
        break;

    case OP_XORI:
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ extended_imm;
        break;

    case OP_LUI:
        NEXT_STATE.REGS[rt] = imm << 16;
        break;

    case OP_LB:
        int8_t value_8 = (int8_t)mem_read_8(address);
        NEXT_STATE.REGS[rt] = value_8;
        break;

    case OP_LH:
        int16_t value_16 = (int16_t)mem_read_16(address);
        NEXT_STATE.REGS[rt] = value_16;
        break;

    case OP_LW:
        // Load word from memory
        NEXT_STATE.REGS[rt] = mem_read_32(address);
        break;

    case OP_LBU:
        NEXT_STATE.REGS[rt] = mem_read_8u(address);
        break;

    case OP_LHU:
        NEXT_STATE.REGS[rt] = mem_read_16u(address);
        break;

    case OP_SB:
        mem_write_8(address, CURRENT_STATE.REGS[rt]);
        break;

    case OP_SH:
        mem_write_16(address, CURRENT_STATE.REGS[rt]);
        break;

    case OP_SW:
        // Store word to memory
        mem_write_32(address, CURRENT_STATE.REGS[rt]);
        break;

    case OP_BEQ:
        // Branch on equal
        if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC += offset;
        }
        break;

    case OP_BNE:
        // Branch on not equal
        if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC += offset;
        }
        break;

    case OP_BLEZ:
        // Branch on less than or equal to zero
        if ((int32_t)CURRENT_STATE.REGS[rs] <= 0)
        {
            NEXT_STATE.PC += offset;
        }
        break;

    case OP_BGTZ:
        // Branch on greater than zero
        if ((int32_t)CURRENT_STATE.REGS[rs] > 0)
        {
            NEXT_STATE.PC += offset;
        }
        break;

    // 特殊的跳转指令
    case OP_BSPECIAL:
        switch (rt)
        {
        case RT_BLTZ:
            // Branch on less than zero
            if ((int32_t)CURRENT_STATE.REGS[rs] < 0)
            {
                NEXT_STATE.PC += offset;
            }
            break;

        case RT_BGEZ:
            // Branch on greater than or equal to zero
            // printf("%d: %x\n", rs, CURRENT_STATE.REGS[rs]);
            if ((int32_t)CURRENT_STATE.REGS[rs] >= 0)
            {
                NEXT_STATE.PC += offset;
            }
            break;

        case RT_BLTZAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            if ((int32_t)CURRENT_STATE.REGS[rs] < 0)
            {
                NEXT_STATE.PC += offset;
            }
            break;

        case RT_BGEZAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            if ((int32_t)CURRENT_STATE.REGS[rs] >= 0)
            {
                NEXT_STATE.PC += offset;
            }
            break;
        }
        break;

    default:
        // 非法指令，什么也不做
    }
}
