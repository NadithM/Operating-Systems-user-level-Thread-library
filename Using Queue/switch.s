.text
.global machine_switch
.global remove_this

machine_switch:
	# address of the new sp is arg1
	# address of the current tcb is arg2
	# need to store all required registered for old tcb
	# restore all required registred from the new tcb
	# then when you return, you should get to the new thread 

	push %rbp
	push %rbx
	push %rsi
	push %rdi
	push %r12
	push %r13
	push %r14
	push %r15
	mov %rsp, (%rsi)

remove_this:
	mov (%rdi), %rsp
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rdi
	pop %rsi
	pop %rbx
	pop %rbp
	ret

