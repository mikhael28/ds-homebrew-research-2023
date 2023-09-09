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
	; r0 ヒープベース
	; r1 スタックベース（スタック領域の最上位アドレス）
	; r2 ヒープリミット
	; r3 スタックリミット（スタック領域の最下位アドレス）
	
	IMPORT bottom_of_heap ; defined in heap.s

	ldr r0,=bottom_of_heap
;	ldr r2,=0x023ff000 ; ヒープリミットをIPCの直前までに
	ldr r2,=0x023ff000-0x12000-0x21800-0x10000-0x1a000 ; ヒープリミットをOverlayRegionの直前までに
	ldr r1,=0x02803f00
	ldr r3,=0x02800000 ; スタックリミットチェックはしない
	
	bx lr

    END
    
