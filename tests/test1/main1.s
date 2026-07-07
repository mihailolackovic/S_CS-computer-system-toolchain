.global a
.extern help
.global a
.global a, b
cool1:
.global a, c, d , e
.section text
.equ [%r1 + 0x10]
.equ [%r1 + a1239812398]
  st %r10, [%r1 + a]
  halt
  st  %r10, %r1 + 0xD
  halt
  st %r10, [%r1]
  st %r10, $0xFFF
  st %r10, 0xFFF
  st %r10, %r1
  ld [%r1 + a], %r10
  halt
  ld %r1 + 0xD, %r10
  halt
  ld [%r1], %r10
  ld $0xFFF, %r10
  ld 0xFFF, %r10
  ld %r1, %r10
  push %r1
  beq %r1, %r2, a
  jmp 0x099
  jmp 0x9999
  jmp 999
  jmp a
  jmp labelyea
  not %r1
  and %r1, %r2
  or %r1, %r2
  xor %r1, %r2
  shl %r1, %r2
  shr %r1, %r2
  add %r1, %r2
cool:
  sub %r1, %r2
  mul %r1, %r2
  div %r1, %r2
  iret
  ret
  iret
  ret
.extern help2
  halt
  halt
  halt
  halt
.global u
.global uU
  halt
  halt
.section bss
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
.global w
.global p, l

  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
  halt
.section data
.global t
.global bb
.section q
.global a
.global c
