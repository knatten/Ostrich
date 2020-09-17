mov rax 0xfedcba9876543210
push rax
pop rbx
mov rbx 0x111
add rax rbx
push rax
dec rbx
