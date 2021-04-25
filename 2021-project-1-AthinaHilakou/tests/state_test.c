//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTList.h"
#include "state.h"

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->current_portal == 0);
	TEST_ASSERT(info->wins == 0);
	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);

	List my_list=state_objects(state, 0, PORTAL_NUM*SPACING); //αν η state_create δουλεύει σωστά
	TEST_ASSERT( list_size(my_list) == PORTAL_NUM-1);				//σε ενα διάστημα της μορφής (0, i*SPACING)
	list_destroy(my_list);
	my_list= state_objects(state, 0, 4*PORTAL_NUM*SPACING);	//θα υπάρχουν  i-1 το πλήθος objects
	TEST_ASSERT(list_size(my_list)== 4*PORTAL_NUM-1);							//αν η state_objects δουλεύει σωστά
	list_destroy(my_list);													//τα objects αυτά θα πρέπει να έχουν εισαχθεί στη λίστα   
	my_list = state_objects(state, 5*PORTAL_NUM*SPACING, 6*PORTAL_NUM*SPACING);
	TEST_ASSERT(list_size(my_list)==0);	


	 int i=0; 														
	 for( ListNode lnode=list_first(my_list); lnode!=LIST_EOF;lnode=list_next(my_list, lnode)){  
	 	if(!(i % 4)){ //επειδη τα objects έχουν εισαχθεί αντίστροφα από ότι στο vector το 0, 4, 8 κλπ είναι portal
		Object portal=list_node_value(my_list, lnode);			
		TEST_ASSERT(portal->type==PORTAL);
	 	}															  
	 i++;																  
	 }
	 list_destroy(my_list);
state_destroy(state);

}






void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, ο χαρακτήρας μετακινείται 7 pixels μπροστά
	Rectangle old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->character->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 7 && new_rect.y == old_rect.y );

	// Με πατημένο το δεξί βέλος, ο χαρακτήρας μετακινείται 12 pixes μπροστά
	keys.right = true;
	old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 12 && new_rect.y == old_rect.y );


	// Με πατημένο το αριστερό βέλος, ο χαρακτήρας αλλάζει κατεύθυνση 
	//Ενημέρωση δύο φορές της κατάστασης, όσο το αριστερό βέλος είναι πατημένο = κίνηση 12 pixel πίσω  
	keys.right = false;
	keys.left = true;
	old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;

	TEST_ASSERT( new_rect.x == old_rect.x - 12 && new_rect.y == old_rect.y );

	//Με πατημένο το πλήκτρο p το παιχνίδι σταματά να ενημερώνεται 
	keys.p = true;
	old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	keys.p = false;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;

	TEST_ASSERT( new_rect.x == old_rect.x  && new_rect.y == old_rect.y );
	keys.p = true; // ξεπαγώνουμε το παιχωίδι και συνεχίζει να ενημερώνεται 
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT( new_rect.x == old_rect.x - 12 && new_rect.y == old_rect.y );

	keys.up = true;
	keys.p = false;
	old_rect = new_rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT( (new_rect.x == old_rect.x - 12) && (new_rect.y == old_rect.y - 15));
	old_rect = new_rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT( (new_rect.x == old_rect.x - 12) && (new_rect.y == old_rect.y - 15));




}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};