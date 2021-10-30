#include "trap.h"
   .globl main
   .text
main:
   
   li    $a0, 0x00FFFFFF        #$a0 = 0x00FFFFFF
   li    $a1, 0x0000FFFF        #$a0 = 0x0000FFFF
   li    $a2, 0x8000FFFF        #$a0 = 0x0000FFFF
   mult  $a0,$a2                #$hi = 0x800000ff, $lo = 0xfeff0001
   multu $a0,$a1                #$hi = 0xff, $lo = 0xfeff0001 
   div   $a0,$a2
   divu  $a0,$a1
   mfhi  $t8
   mflo  $t9
   mthi  $a1
   mtlo  $a2


   HIT_GOOD_TRAP		#stop temu
