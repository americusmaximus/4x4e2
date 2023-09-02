vs.1.1
dcl_position v0
dcl_normal v3
dcl_texcoord v7
mov r2, c0 /* initialize register r2 */
mov r1, c0 /* initialize register r1 */
mov r0, c0 /* initialize register r0 */
mov oD0, c0 /* initialize output register oD0 */
mov oT2, c0 /* initialize output register oT2 */
mov oT1, c0 /* initialize output register oT1 */
mov oT0, c0 /* initialize output register oT0 */
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
dp3 r1.x, v3, c4
max r1, r1.x, c5.x
mul r1, r1, c8
add r1, r1, c7
mov r1.w, c5.w
mov oD0, r1
mov oT0, v7
mov oT1, v7
add r1, c9, -v0
rsq r2, r1
mul r2, r1, r2
add r0, v3, v3
dp3 r0.x, r0, r2
mul r1, r0.x, v3
add r1, r1, r2
dp3 r0.x, r1, c12
dp3 r0.y, r1, c13
dp3 r0.z, r1, c14
mov oT2, r0
dp4 r0.z, v0, c2
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r0.z, r0.z, c5.y
add oFog /* removed swizzle */, c5.y, -r0.z