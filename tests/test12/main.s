.global a
# .weak a

.section main

ld a, %r1
halt

a:
.word 0

.end
