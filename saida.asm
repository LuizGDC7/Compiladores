.data
newline_str: .asciiz "\n"
var_1: .word 0 # a (line 4)
var_2: .word 0 # b (line 4)
var_3: .word 0 # res (line 4)
var_4: .word 0 # letter (line 4)
str_4: .asciiz "Falha"
str_3: .asciiz "Sucesso: Resultado > 20"
str_2: .asciiz "Caractere: "
str_1: .asciiz "Resultado: "

.text
.globl main
main:
    li $t0, 15
    sw $t0, var_1 # a
    li $t0, 5
    sw $t0, var_2 # b
    lw $t0, var_1 # a
    subu $sp, $sp, 4
    sw $t0, 0($sp)
    lw $t0, var_2 # b
    subu $sp, $sp, 4
    sw $t0, 0($sp)
    li $t0, 2
    lw $t1, 0($sp)
    addu $sp, $sp, 4
    mul $t0, $t1, $t0
    lw $t1, 0($sp)
    addu $sp, $sp, 4
    add $t0, $t1, $t0
    sw $t0, var_3 # res
    la $a0, str_1
    li $v0, 4
    syscall
    lw $t0, var_3 # res
    move $a0, $t0
    li $v0, 1
    syscall
    la $a0, newline_str
    li $v0, 4
    syscall
    li $t0, 75 # 'K'
    sw $t0, var_4 # letter
    la $a0, str_2
    li $v0, 4
    syscall
    lw $t0, var_4 # letter
    move $a0, $t0
    li $v0, 11
    syscall
    la $a0, newline_str
    li $v0, 4
    syscall
    lw $t0, var_3 # res
    subu $sp, $sp, 4
    sw $t0, 0($sp)
    li $t0, 20
    lw $t1, 0($sp)
    addu $sp, $sp, 4
    sgt $t0, $t1, $t0
    beqz $t0, L_else_1
    la $a0, str_3
    li $v0, 4
    syscall
    la $a0, newline_str
    li $v0, 4
    syscall
    j L_end_2
L_else_1:
    la $a0, str_4
    li $v0, 4
    syscall
    la $a0, newline_str
    li $v0, 4
    syscall
L_end_2:
    li $v0, 10
    syscall
