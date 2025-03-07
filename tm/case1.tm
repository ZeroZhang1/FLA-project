#Q = {q0, input_a, input_b, back, write, wb, accept, clear, clear_back, reject, reject2, reject3, reject4, reject5, reject6, reject7, reject8, reject9, reject10, reject11, reject12, reject13, reject14}
#S = {a, b}
#G = {a, b, c, X, Y, Z, _, i, l, e, g, n, p, u, t}
#q0 = q0
#B = _
#F = {accept}
#N = 3

; the transition functions

q0 a__ ZX_ rr* input_a
q0 b__ ZXY l** clear_back
q0 ___ ZXY *** clear_back
q0 _** ZXY r** clear_back
; 向右读取a。读到一个a，第二条纸带写入一个X
input_a a__ ZX_ rr* input_a
input_a ___ _XY l** clear_back
; 向右读取b，第三条纸带写入一个Y
input_a b__ Z_Y r*r input_b 
;
input_b b__ Z_Y r*r input_b 
input_b a__ ZXY l** clear_back
; 清空第一条纸带，将二三纸带的读写头移动到起始位置
input_b ___ ___ lll back
back Z** _** lll back
back Z_* __* l*l back
back Z*_ _*_ ll* back
back Z__ ___ l** back
back ___ ___ rrr write

write ___ ___ rrr write
write _XY cXY r*r write
write _X_ _X_ *rl wb
wb _XY _XY **l wb
wb __* __* *** accept
wb _*_ _*_ **r write

; clear清空第一条纸带的内容
clear_back *** *** l** clear_back
clear_back _** _** r** clear
clear *** _** r** clear
clear _** _** r** reject

; reject输出illegal input
reject _** i** r** reject2
reject2 _** l** r** reject3
reject3 _** l** r** reject4
reject4 _** e** r** reject5
reject5 _** g** r** reject6
reject6 _** a** r** reject7
reject7 _** l** r** reject8
reject8 _** _** r** reject9
reject9 _** i** r** reject10
reject10 _** n** r** reject11
reject11 _** p** r** reject12
reject12 _** u** r** reject13
reject13 _** t** r** reject14

reject *** i** r** reject2
reject2 *** l** r** reject3
reject3 *** l** r** reject4
reject4 *** e** r** reject5
reject5 *** g** r** reject6
reject6 *** a** r** reject7
reject7 *** l** r** reject8
reject8 *** _** r** reject9
reject9 *** i** r** reject10
reject10 *** n** r** reject11
reject11 *** p** r** reject12
reject12 *** u** r** reject13
reject13 *** t** r** reject14