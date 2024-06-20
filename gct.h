#pragma once

#define GCT_WRITE_8(ADDRESS, VALUE) \
	.long 0x00000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long (VALUE & 0xFF)

#define GCT_FILL_8(ADDRESS, VALUE, COUNT) \
	.long 0x00000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long (VALUE & 0xFF) | ((COUNT + 1) << 16)

#define GCT_WRITE_16(ADDRESS, VALUE) \
	.long 0x02000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long (VALUE & 0xFFFF)

#define GCT_FILL_16(ADDRESS, VALUE, COUNT) \
	.long 0x02000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long (VALUE & 0xFFFF) | ((COUNT + 1) << 16)

#define GCT_WRITE_32(ADDRESS, VALUE) \
	.long 0x04000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long VALUE

#define GCT_WRITE_INSTR(ADDRESS, ...) \
	.long 0x04000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	__VA_ARGS__

#define GCT_WRITE_STRING(ADDRESS, STRING) \
	.long 0x06000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long string_##ADDRESS##_size; \
string_##ADDRESS:; \
	.string STRING; \
	.set string_##ADDRESS##_size, . - string_##ADDRESS; \
	.align 3

#define GCT_STRING(ADDRESS, NAME) \
	.long 0x06000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long NAME##_size; \
NAME:;
#define GCT_STRING_END(NAME) \
	.set NAME##_size, . - NAME; \
	.align 3

#define GCT_STRING_B_CALL(ADDRESS, NAME, DEST) \
	b DEST - (PORT(ADDRESS) + (. - NAME))

#define GCT_STRING_BL_CALL(ADDRESS, NAME, DEST) \
	bl DEST - (PORT(ADDRESS) + (. - NAME))

#define GCT_STRING_BC_CALL(ADDRESS, NAME, FIELD, CONDITION, DEST) \
	b##CONDITION- FIELD, DEST - (PORT(ADDRESS) + (. - NAME))

#define GCT_STRING_BCL_CALL(ADDRESS, NAME, FIELD, CONDITION, DEST) \
	b##CONDITION##l- FIELD, DEST - (PORT(ADDRESS) + (. - NAME))

#define GCT_STRING_PTR(ADDRESS, NAME, REG, DEST) \
	lis REG, (PORT(ADDRESS) + (DEST - NAME))@ha; \
	addi REG, REG, (PORT(ADDRESS) + (DEST - NAME))@l

#define GCT_STRING_PTR_LWZ(ADDRESS, NAME, REG, DEST) \
	lis REG, (PORT(ADDRESS) + (DEST - NAME))@ha; \
	lwz REG, (PORT(ADDRESS) + (DEST - NAME))@l(REG)

#define GCT_STRING_PTR_LBZ(ADDRESS, NAME, REG, DEST) \
	lis REG, (PORT(ADDRESS) + (DEST - NAME))@ha; \
	lbz REG, (PORT(ADDRESS) + (DEST - NAME))@l(REG)

#define GCT_STRING_PTR_LWZU(ADDRESS, NAME, REG, TMP_REG, DEST) \
	lis TMP_REG, (PORT(ADDRESS) + (DEST - NAME))@ha; \
	lwzu REG, (PORT(ADDRESS) + (DEST - NAME))@l(TMP_REG)

#define GCT_STRING_PTR_LBZU(ADDRESS, NAME, REG, TMP_REG, DEST) \
	lis TMP_REG, (PORT(ADDRESS) + (DEST - NAME))@ha; \
	lbzu REG, (PORT(ADDRESS) + (DEST - NAME))@l(TMP_REG)

#define GCT_STRING_PTR_STW(ADDRESS, NAME, REG, TMP_REG, DEST) \
	lis TMP_REG, (PORT(ADDRESS) + (DEST - NAME))@ha; \
	stw REG, (PORT(ADDRESS) + (DEST - NAME))@l(TMP_REG)

#define GCT_STRING_PTR_STWU(ADDRESS, NAME, REG, TMP_REG, DEST) \
	lis TMP_REG, (PORT(ADDRESS) + (DEST - NAME))@ha; \
	stwu REG, (PORT(ADDRESS) + (DEST - NAME))@l(TMP_REG)

#define GCT_IF_EQUAL_32(ADDRESS, VALUE) \
	.long 0x20000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long VALUE

#define GCT_IF_NOT_EQUAL_32(ADDRESS, VALUE) \
	.long 0x22000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long VALUE

#define GCT_IF_EQUAL_INST(ADDRESS, INST) \
	.long 0x20000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	INST

#define GCT_IF_NOT_EQUAL_INST(ADDRESS, INST) \
	.long 0x22000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	INST

#define GCT_ENDIF(COUNT) \
	.long 0xE2000000 | ((COUNT) & 0xFF); \
	.long 0x00000000

#define GCT_ELSE(COUNT) \
	.long 0xE2100000 | ((COUNT) & 0xFF); \
	.long 0x00000000

#define GCT_EXECUTE(NAME) \
	.long 0xC0000000; \
	.long (NAME##_size + 1) >> 3; \
NAME:
#define GCT_EXECUTE_END(NAME) \
	.align 3; \
	.set NAME##_size, . - NAME

#define GCT_INSERT(ADDRESS, NAME) \
	.long 0xC2000000 | (PORT(ADDRESS) & 0x1FFFFFF); \
	.long (NAME##_size + 1) >> 3; \
	.set NAME##_start, .
#define GCT_INSERT_END(NAME) \
	nop; \
	.align 3; \
	.set NAME##_size, . - NAME##_start

#define GCT_WRITE_BRANCH(SRC, DEST) \
	.long 0xC6000000 | (SRC & 0x1FFFFFF); \
	.long DEST

#ifndef NO_ASM_CALL_THUNK

#define B_CALL(ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	b asm_call_thunk;

#define BL_CALL(ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	bl asm_call_thunk;

#define BC_CALL(FIELD, CONDITION, ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	b##CONDITION- FIELD, asm_call_thunk;

#define BCL_CALL(FIELD, CONDITION, ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	b##CONDITION##l- FIELD, asm_call_thunk;

#else

#define B_CALL(ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	mtctr r12; \
	bctr;

#define BL_CALL(ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	mtctr r12; \
	bctrl;

#define BC_CALL(FIELD, CONDITION, ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	mtctr r12; \
	b##CONDITION##ctr- FIELD;

#define BCL_CALL(FIELD, CONDITION, ADDRESS) \
	lis r12, PORT(ADDRESS)@h; \
	ori r12, r12, PORT(ADDRESS)@l; \
	mtctr r12; \
	b##CONDITION##ctrl- FIELD;

#endif

#define STRING(REG, STRING, SYM) \
	bl SYM##_end; \
SYM:; \
	.string STRING; \
	.align 2; \
SYM##_end:; \
	mflr REG

#define SHORT_PTR(REG, SYM, DATA...) \
	bl short_##SYM##_end; \
short_##SYM:; \
	.short DATA; \
	.align 2; \
	short_##SYM##_end:; \
	mflr REG

#define FLOAT_PTR(REG, SYM, DATA...) \
	bl short_##SYM##_end; \
short_##SYM:; \
	.float DATA; \
	.align 2; \
	short_##SYM##_end:; \
	mflr REG

#define PTR(REG, NAME) \
	bl 4; \
	mflr REG; \
	addi REG, REG, (NAME) - (. - 4)

#define PTR_LWZ(REG, NAME) \
	bl 4; \
	mflr REG; \
	lwz REG, (NAME) - (. - 4)(REG)

#define PTR_LBZ(REG, NAME) \
	bl 4; \
	mflr REG; \
	lbz REG, (NAME) - (. - 4)(REG)

#define PTR_LWZU(REG, TMP_REG, NAME) \
	bl 4; \
	mflr TMP_REG; \
	lwzu REG, (NAME) - (. - 4)(TMP_REG)

#define PTR_LBZU(REG, TMP_REG, NAME) \
	bl 4; \
	mflr TMP_REG; \
	lbzu REG, (NAME) - (. - 4)(TMP_REG)

#define PTR_STW(REG, TMP_REG, NAME) \
	bl 4; \
	mflr TMP_REG; \
	stw REG, (NAME) - (. - 4)(TMP_REG)

#define PTR_STWU(REG, TMP_REG, NAME) \
	bl 4; \
	mflr TMP_REG; \
	stwu REG, (NAME) - (. - 4)(TMP_REG)


#define STACK_SIZE(REG_BASE, DATA_SIZE) \
    (((32 - (REG_BASE)) << 2) + 8 + (DATA_SIZE))

#define STACK_PUSH(REG_BASE, DATA_SIZE) \
    stwu sp, -STACK_SIZE(REG_BASE, DATA_SIZE)(sp); \
    STACK_SAVELR(REG_BASE, DATA_SIZE); \
    .if REG_BASE == 31; \
        stw REG_BASE, STACK_SIZE(REG_BASE, DATA_SIZE) - 4(sp); \
    .else; \
        stmw REG_BASE, STACK_SIZE(REG_BASE, DATA_SIZE) - ((32 - (REG_BASE)) << 2)(sp); \
    .endif

#define STACK_POP(REG_BASE, DATA_SIZE) \
    STACK_RESTLR(REG_BASE, DATA_SIZE); \
    .if REG_BASE == 31; \
        lwz REG_BASE, STACK_SIZE(REG_BASE, DATA_SIZE) - 4(sp); \
    .else; \
        lmw REG_BASE, STACK_SIZE(REG_BASE, DATA_SIZE) - ((32 - (REG_BASE)) << 2)(sp); \
    .endif; \
    addi sp, sp, STACK_SIZE(REG_BASE, DATA_SIZE)

#define STACK_SAVELR(REG_BASE, DATA_SIZE) \
    mflr r0; \
    stw r0, STACK_SIZE(REG_BASE, DATA_SIZE) + 4(sp)

#define STACK_RESTLR(REG_BASE, DATA_SIZE) \
    lwz r0, STACK_SIZE(REG_BASE, DATA_SIZE) + 4(sp); \
    mtlr r0
