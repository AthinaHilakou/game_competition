#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include "interface.h"
#include "state.h"
#include <assert.h>


State state;
KeyState keys;
 //δημιουργεί την αρχική κατάσταση των keys  
KeyState keys_create(){
    KeyState keys = malloc(sizeof(*keys));
    keys->left = false;
    keys->right = false;
    keys->up = false;
    keys->p = false; 
    keys->n = false;          
    keys->enter = false; 
    return keys;
}
//ελέγχει ποια πλήκτρα έιναι πατημένα και ενημερώνει την KeyState
void keys_update(KeyState keys){
    keys->left = IsKeyDown(KEY_LEFT);
    keys->right = IsKeyDown(KEY_RIGHT);
    keys->up = IsKeyDown(KEY_UP);
    keys->p = IsKeyPressed(KEY_P);
    keys->n = IsKeyPressed(KEY_N);           
    keys->enter = IsKeyPressed(KEY_ENTER);

}

void update_and_draw() {
	keys_update(keys);
    state_update(state, keys);
    StateInfo info = state_info(state);
    if(keys->enter && !(info->playing)){
		state_destroy(state);
		state = state_create();
	}
	interface_draw_frame(state);
}

int main() {
	state = state_create();
    keys = keys_create();
	interface_init();

	// Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	start_main_loop(update_and_draw);

	interface_close();
    free(keys);
    state_destroy(state);

	return 0;
}
