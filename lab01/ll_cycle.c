#include "ll_cycle.h"
#include <stddef.h>

int ll_has_cycle(node *head) {
	if (head == NULL || head->next == NULL) {
		return 0; 
	}

	struct node *tortoise = head;
    struct node *hare = head;

  	while (hare->next != NULL && hare->next->next != NULL) {
		tortoise = tortoise->next; 
		hare = hare->next->next;
		
		if (tortoise == hare) {
			return 1; // Cycle detected
		}
	}
	
	// If we reach here, no cycle was detected
	return 0;
}
