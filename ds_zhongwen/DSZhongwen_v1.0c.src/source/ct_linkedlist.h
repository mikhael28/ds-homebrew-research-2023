/*
Copyright (c) 2007 Rodrigo Roman and Ramon Roman (rrc2soft)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of "rrc2soft" nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/* Linked List - A normal, basic linked list */
#ifndef _H_LINKEDLIST_
#define _H_LINKEDLIST_

typedef struct llistheader{
	int id;
	int nele;
	struct llist *next;
} LList;

/* Creates a basic Linked List */
LList *ct_linkedlist_init();

/* Destroys a basic Linked List */
void ct_linkedlist_finish(LList *ll);

/* Inserts an element, return id */
int ct_linkedlist_add(LList *ll, void *element);

/* Inserts an element in the end, return id */
int ct_linkedlist_addEnd(LList *ll, void *element);

/* Returns the element identified as id using a function pointer as comparison*/
void *ct_linkedlist_get(LList *ll, int id, int (*compare)(void *,int));

/* Returns the element number pos */
void *ct_linkedlist_getPos(LList *ll, int pos);

/* Returns numbers of elements in a linked list */
int ct_linkedlist_len(LList *ll);

#endif
