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


// Includes
#include <PA9.h>       // Include for PA_Lib
#include "ct_linkedlist.h"

typedef struct llist{
	int id;
	void *content;
	struct llist *next;
} nd;


/* Variables */

/* Functions */
///////////////

// Function: void *ct_linkedlist_init()
LList *ct_linkedlist_init()
{
  LList *ll = malloc(sizeof(LList));
  ll->id = 0;
  ll->nele = 0;
  ll->next = NULL;
  return ll;
} // end of void *ct_linkedlist_init()

// Function void ct_linkedlist_finish(void *ll)
void ct_linkedlist_finish(LList *ll)
{
	nd *node;
	while ((ll)->next != NULL) {
		node = (ll)->next;
		free(node->content);
		(ll)->next = node->next;
		free(node);
	}
	free(ll);
	ll = NULL;
} // end of void ct_linkedlist_finish(void *ll)

// Function ct_linkedlist_add(void *ll, void *element)
int ct_linkedlist_add(LList *ll, void *element)
{
	// Create
	nd *node = malloc(sizeof(nd));
	node->id = (ll)->id;
	node->content = element;
	node->next = NULL;
	
	// Add
	if ((ll)->next == NULL) {
		(ll)->next = node;
	} else {
		node->next = (ll)->next; // Get 1st node
		(ll)->next = node; // Becomes 1st node
	}
	
	// Post
	(ll)->id++;
	(ll)->nele++;
	return node->id;
} // end of ct_linkedlist_add(void *ll, void *element)

// Function ct_linkedlist_addEnd(void *ll, void *element)
int ct_linkedlist_addEnd(LList *ll, void *element)
{
   nd *last;
	// Create
	nd *node = malloc(sizeof(nd));	
	node->id = (ll)->id;
	node->content = element;
	node->next = NULL;
	
	// Add
	if ((ll)->next == NULL) {
		(ll)->next = node;
	} else {
	   last = (ll)->next;
	   while (last->next != NULL)
	   	last = last->next; // Get last node
		last->next = node; // Put in last node
	}
	
	// Post
	(ll)->id++;
	(ll)->nele++;
	return node->id;
} // end of ct_linkedlist_addEnd(void *ll, void *element)


// Function void *ct_linkedlist_get(void *ll, int id)
void *ct_linkedlist_get(LList *ll, int id, int (*compare)(void *,int))
{
	int eq = 0;
	nd *res = (ll)->next; // 1st node
	while (res != NULL) {
		if (compare == NULL)
			eq = (res->id == id);
		else 
			eq = compare(res->content,id);
		if (eq)
			return (void *)(res->content); // it's me!
		else
			res = res->next; // not yet...
	}
	return NULL;
} // end of void *ct_linkedlist_get(void *ll, int id)

// Function void *ct_linkedlist_getPos(void *ll,int pos)
void *ct_linkedlist_getPos(LList *ll, int pos)
{
	nd *res = (ll)->next; // 1st node

	while (res != NULL) {
		pos--;	
		if (pos == -1)
			return (void *)(res->content); // it's me!
		else
			res = res->next; // not yet...
	}
	return NULL;
} // end of void *ct_linkedlist_getPos(void *ll, int pos)

// Function: int ct_linkedlist_len(LList *ll)
int ct_linkedlist_len(LList *ll) {
   if (ll == NULL)
   	return 0;
   	
   return (ll)->nele;
}   
