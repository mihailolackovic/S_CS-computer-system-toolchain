.section main

  ld [%r1 + %r1] , %r1
  st %r1 , [%r1 + %r1]

  add %r1 , %r2 >> 3
  add %r1 , %r2 << 3

.end
