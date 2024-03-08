.ppc_stub:
	addis	11,2,-1
	addi	11,11,26328
	ld	12,32(11)
	mtctr	12
	ld	12, PACA_THREAD_INFO(13)
	ld	11, TI_livepatch_sp(12)
	ld	2,-24(11)
	std	2,24(1)
	b	32
