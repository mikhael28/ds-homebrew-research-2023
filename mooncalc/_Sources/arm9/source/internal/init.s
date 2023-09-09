;    IMPORT __use_no_semihosting
    IMPORT __use_two_region_memory
    
    EXPORT _sys_exit
;    EXPORT _ttywrch
    EXPORT __user_initial_stackheap

    AREA     globals,CODE,READONLY
    
    
_sys_exit
	b {pc}
	
_ttywrch
	bx lr
	
__user_initial_stackheap
	; r0 �q�[�v�x�[�X
	; r1 �X�^�b�N�x�[�X�i�X�^�b�N�̈�̍ŏ�ʃA�h���X�j
	; r2 �q�[�v���~�b�g
	; r3 �X�^�b�N���~�b�g�i�X�^�b�N�̈�̍ŉ��ʃA�h���X�j
	
	IMPORT bottom_of_heap ; defined in heap.s

	ldr r0,=bottom_of_heap
;	ldr r2,=0x023ff000 ; �q�[�v���~�b�g��IPC�̒��O�܂ł�
	ldr r2,=0x023ff000-0x12000-0x21800-0x10000-0x1a000 ; �q�[�v���~�b�g��OverlayRegion�̒��O�܂ł�
	ldr r1,=0x02803f00
	ldr r3,=0x02800000 ; �X�^�b�N���~�b�g�`�F�b�N�͂��Ȃ�
	
	bx lr

    END
    
