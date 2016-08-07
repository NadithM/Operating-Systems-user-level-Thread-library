.text
.global machine_switch
.global switch_to

machine_switch:
	# address of the new sp is arg1
	# address of the current tcb is arg2
	# need to store all required registered for old tcb
	# restore all required registred from the new tcb
	# then when you return, you should get to the new threa

	push %rbp
	push %rax 
	push %rbx 
	push %rcx 
	push %rdx 
	push %rdi 
	push %rsi 
	push %r8 
	push %r9 
	push %r10 
	push %r11 
	push %r12 
	push %r13 
	push %r14 
	push %r15
	mov %rsp,(%rsi) 





	# finally put stack pointer of that thread value of that thread address.
	#when come again this is the address starts 

switch_to:

	mov (%rdi), %rsp

	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rsi
	pop %rdi
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax
	pop %rbp
	ret
