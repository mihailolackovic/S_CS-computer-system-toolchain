.equ abs, 10
.section data
string:
.ascii "0123456789"
b:
.section text
ld $12, %r1
ld 12, %r1
ld $a, %r1
ld a, %r1
ld $b, %r1
ld b, %r1
c:
ld $c, %r1
ld c , %r1
st %r1 , $12 
st %r1 , 12
st %r1,$a  
st %r1, a
st %r1, $b
st %r1, b
st %r1, $c
st %r1, c

a:
.end

