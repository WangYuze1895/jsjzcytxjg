#include "trap.h"
   .set noat
   .globl main
   .text
main:
   
   li $at, 0x10101010           #$at = 0x10101010
   li $v0, 0x01011111           #$v0 = 0x01011111
   and $t0, $at, $v0            #$t0 = 0x00001010
   andi $v1,$v0,0x1011          #$v1 = 0x00001011
   addi $v0,$v1,0x8000          #$v0 = 0xFFFF9011 (Y)
   addiu $v0,$t0,0x8000         #$v0 = 0xFFFF9010
   slti  $t1,$v0,0x8000         #$t1 = 1 
   sltiu  $t1,$v0,0x8000        #$t1 = 0 
   xori  $s1,$at,0x1110         #$s1 = 0x10100100

   add   $t1,$t0,$v1            #$t1 = 0x00002021 (Y)
   addu  $t1,$t0,$v1            #$t1 = 0x00002021
   li    $t3, 0x00010101        #$t3 = 0x00010101
   sub   $t1,$v1,$t0            #$t1 = 1          (Y)
   subu  $t1,$v1,$t0            #$t1 = 1  
   slt   $t2,$v0,$v1            #$t2 = 1 
   sltu  $t2,$v0,$v1            #$t2 = 0 
   or    $t3,$t3,$s1            #$t3 = 0x10110101
   li    $t4, 0x10100101        #$t4 = 0x10100101
   nor   $t3,$t4,$s1            #$t3 = 0xefeefefe
   li    $t5, 0x10100101        #$t5 = 0x10100101
   xor   $t3,$t5,$v1            #$t3 = 0x10101110

   sll   $t2,$t5,0x1            #$t2 = 0x20200202
   li    $s2, 0xFFFFFFFF        #$s2 = 0xFFFFFFFF
   srl   $t2,$s2,0x1            #$t2 = 0x7FFFFFFF
   sra   $t2,$s2,0x1            #$t2 = 0xFFFFFFFF
   sllv  $t2,$s2,$t1            #$t2 = 0xFFFFFFFE
   srlv  $t2,$s2,$t1            #$t2 = 0x7FFFFFFF
   srav  $t2,$s2,$t1            #$t2 = 0xFFFFFFFF


   HIT_GOOD_TRAP		#stop temu
