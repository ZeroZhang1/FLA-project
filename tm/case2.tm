; 可以利用恒等式 1+3+5+...+(2n-1) = n^2进行判断
#Q = {init, start, back, sub, add, acc, acc1, acc2, acc3, acc4, reject, r1, r2, r3, r4, r5}
#S = {1} 
#G = {1, _, X, t, r, u, e, f, a, l, s} 
#q0 = init 
#B = _
#F = {acc} 
#N = 2

; 转移函数
init __ _X ** reject

init 1_ 1X ** back
back *X *X *l back
back *_ *_ *r sub

sub 1X _X rr sub
sub 1_ 1X *r add
add 1_ 1X ** back

sub __ __ ** acc
sub _X _X ** reject 

acc __ t_ r* acc1
acc1 __ r_ r* acc2
acc2 __ u_ r* acc3
acc3 __ e_ r* acc4

reject _X fX r* r1
r1 _X aX r* r2
r2 _X lX r* r3
r3 _X sX r* r4
r4 _X eX r* r5