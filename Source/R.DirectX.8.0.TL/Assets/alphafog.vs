vs.1.0
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
mov oT0, v7
dp4 r0.z, v0, c2
mov r1, c10
add r0.z, r0.z, -c6.x
mul r0.z, r0.z, c6.y
max r0.z, r0.z, c5.x
min r1.w, r0.z, c5.y
mov oD0, r1
mov oFog.x, c5.y