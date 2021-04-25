
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "ADTSet.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	List portal_pairs;		// περιέχει PortalPair (ζευγάρια πυλών, είσοδος/έξοδος)

	struct state_info info;
};

// Ζευγάρια πυλών

typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;




/*----------------------------------------------------------------------------------------------------------------*/
//Δεμσεύει μνήμη για ένα int στο σωρό
//Αξιοποιείται στην αρχικοποίηση των portal pairs μέσω της state_crearte
int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης
	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}
//Σύγκριση ακεραίων
//Αξιοποιείται στην αρχικοποίηση των portal pairs μέσω της state_crearte
int compare_ints(Pointer a, Pointer b) {
    int* ia = a;
    int* ib = b;
    return *ia - *ib;       // αρνητικός, μηδέν ή θετικός, ανάλογα με τη διάταξη των a,b
}

int compare_pair_exits(Pointer a, Pointer b){ //συγκρίνει δύο pairs βάσει τον εξόδων τους 
	PortalPair pair_a = a;
	PortalPair pair_b = b;
	float exit_a = pair_a->exit->rect.x;
	float exit_b = pair_b->exit->rect.x;
	return (int)(exit_a-exit_b);

}
int compare_pair_entrances(Pointer a, Pointer b){ //συγκρίνει δύο pairs βάσει τον εισόδων τους
	PortalPair pair_a = a;
	PortalPair pair_b = b;
	float entrance_a = pair_a->entrance->rect.x;
	float entrance_b = pair_b->entrance->rect.x;
	return (int)(entrance_a - entrance_b);

}


void nearest_portal(StateInfo info){
	float x  = info->character->rect.x;
	//οι συντεταγμένες του χαρακτήρα δια τις συντεταγμένες της πρώτης πύλης 
	//που αποτελούν ταυτόχρονα την απόσταση μεταξύ δύο οποιονδήποτε πυλών 
	if( (x >= 0) && ((x/(4*SPACING)) < PORTAL_NUM))
		info->current_portal = (x/(4*SPACING));
	else if (x < 0)
		info->current_portal = 0;
	else if(x > PORTAL_NUM)
		info->current_portal = PORTAL_NUM;
}



//module της state_ubdate για καλυτερη αναγνωσιμοτητα του κωδικα
void character_position_update(Object character, KeyState keys){
	//ενημέρωση της οριζόντιας θέσης και φοράς κίνησης 
	if(character->forward){ 
		if(keys->right)
			character->rect.x += 12;
		else if(keys->left)
			character->forward= false;
		else  
			character->rect.x += 7;
	}
	else {
		if(keys->left)
			character->rect.x -= 12;
		else if(keys->right)
			character->forward= true;
		else 
			character->rect.x -= 7;
	}
	//ενημέρωση της κατακόρυφης θέσης και φοράς κίνησης
	static bool fall;
	if(keys->up && (character->rect.y == - character->rect.height)){ //μπαίνει σε κατάσταση άλματος
		character->jumping=true;
		character->rect.y-=15;
		fall = false;	
	}
	else if(character->jumping && (character->rect.y>-220) && fall==false) {//στην κατάσταση άλματος μετακινείται 15 pixels προς τα πάνω σε κάθε frame
	 	character->rect.y-=15;								//μέχρι να φτάσει 220 pixels από το έδαφος
		if(character->rect.y<=-220){
			fall = true;	
		}							
	}
	else if(character->jumping && (character->rect.y< - character->rect.height) && fall==true){
		character->rect.y+=15;
		if (character->rect.y == - character->rect.height)
			character->jumping=false;
	} 	
}

void enemy_position_update(Vector objects){
	for(int i=0; i<vector_size(objects); i++){
		if(i % 4 == 3)
			continue;
		Object enemy = vector_get_at(objects, i);
		if(enemy->type == ENEMY){
			if(enemy->forward)
				enemy->rect.x += 5;
			else
				enemy->rect.x -= 5;
		}
	}
}

void character_collision_control(State state){
	Object character = state->info.character;
	Object lastportal = vector_get_at(state->objects, 4*PORTAL_NUM-1); 
	for(int i = 0; i<vector_size(state->objects); i++){
		Object object = vector_get_at(state->objects, i);
		if(object->type == OBSTACLE || object->type == ENEMY){ 
			if(CheckCollisionRecs(character->rect,object->rect)){
				state->info.playing = false; 
			}
		}
		//σύγκρουση με πύλη που δεν είναι η τελευταία
		else if(object->type == PORTAL && object != lastportal){  
			if(CheckCollisionRecs(character->rect,object->rect)){
				PortalPair temp_pair1 = malloc(sizeof(*temp_pair1));
				if(character->forward){ 			//Αν ο Χαρακτήρας συγκρουστεί με μια Πύλη A ενώ κινείται προς τα δεξιά 
					temp_pair1->entrance = object;	//μετακινείται στην αντίστοιχη πύλη B στην οποία οδηγεί η Α.
					PortalPair temp_pair2  = list_find(state->portal_pairs,temp_pair1, compare_pair_entrances);
					character->rect.x = (temp_pair2->exit->rect.x + temp_pair2->exit->rect.width);
				}
				else {
					temp_pair1->exit = object; //Αν κινείται ανάποδα, τότε η μετακίνηση είναι αντίστροφη
					PortalPair temp_pair3 = list_find(state->portal_pairs,temp_pair1, compare_pair_exits);  
					character->rect.x = (temp_pair3->entrance->rect.x - temp_pair3->entrance->rect.width); //δηλαδή μετακινείται στην πύλη B η οποία οδηγεί στην A.				
				}
				free(temp_pair1);
			}
		}
		else{ // σύγκρουση με την τελευταία πύλη 
			if(CheckCollisionRecs(character->rect,object->rect)){
				character->rect.x = 0;
				state->info.wins += 1;
			}
		}
	}
}

void enemy_collision_control(State state){
	for(int i = 0; i<vector_size(state->objects); i++){ // για κάθε εχθρό 
		Object obj1 =vector_get_at(state->objects, i);
		if (obj1->type != ENEMY) continue; 
		for(int j = 1; j<vector_size(state->objects); j++){ //έλεγξε αν συγκρούεται με κάποιο obstacle ή portal
			int k= i+j;  //διατρέχοντας τα όλα τα υπόλοιπα στοιχεία του vector 
			if (k>=vector_size(state->objects))
					k-=vector_size(state->objects);
			Object obj2 = vector_get_at(state->objects, k);
			//αν χτυπήσει σε obstacle αλλάζει κατεύθυνση 
			if(CheckCollisionRecs(obj1->rect,obj2->rect) && obj2->type== OBSTACLE) {
					obj1->forward = !(obj1->forward);	
			}
			else if(CheckCollisionRecs(obj1->rect,obj2->rect)&& obj2->type== PORTAL){ //σύγκρουση με πύλη
				PortalPair temp_pair1 = malloc(sizeof(*temp_pair1));
				if(obj1->forward){ 			//Αν ο εχθρός συγκρουστεί με μια Πύλη A ενώ κινείται προς τα δεξιά 
					temp_pair1->entrance = obj2;	//μετακινείται στην αντίστοιχη πύλη B στην οποία οδηγεί η Α.
					PortalPair temp_pair2  = list_find(state->portal_pairs,temp_pair1, compare_pair_entrances);
					obj1->rect.x = (temp_pair2->exit->rect.x + 2*(temp_pair2->exit->rect.width));
				}
				else {
					temp_pair1->exit = obj2; //Αν κινείται ανάποδα, τότε η μετακίνηση είναι αντίστροφη
					PortalPair temp_pair3 = list_find(state->portal_pairs,temp_pair1, compare_pair_exits); 
					obj1->rect.x = (temp_pair3->entrance->rect.x - 2*(temp_pair3->entrance->rect.width)); //δηλαδή μετακινείται στην πύλη B η οποία οδηγεί στην A.
				}
				free(temp_pair1);
			}
		}
	}
}



PortalPair portal_pair_create (Object entrance, Object exit){ //δημιουργεί(στο σωρό) και επιστρέφει ένα portal_pair 
	PortalPair pair= malloc(sizeof(*pair));					 
	pair->entrance= entrance;
	pair->exit= exit; 
	return pair;
}
/*----------------------------------------------------------------------------------------------------------------*/






// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.current_portal = 0;			// Δεν έχουμε περάσει καμία πύλη
	state->info.wins = 0;					// Δεν έχουμε νίκες ακόμα
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.

	// Πληροφορίες για το χαρακτήρα.
	Object character = state->info.character = malloc(sizeof(*character));
	assert(character != NULL);
	character->type = CHARACTER;
	character->forward = true;
	character->jumping = false;

    // Ο χαρακτήρας (όπως και όλα τα αντικείμενα) έχουν συντεταγμένες x,y σε ένα
    // καρτεσιανό επίπεδο.
	// - Στο άξονα x το 0 είναι η αρχή στης πίστας και οι συντεταγμένες
	//   μεγαλώνουν προς τα δεξιά.
	// - Στον άξονα y το 0 είναι το "δάπεδο" της πίστας, και οι
	//   συντεταγμένες μεγαλώνουν προς τα _κάτω_.
	// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
	// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
	// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
	// στο include/raylib.h).
	// 
	// Προσοχή: τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle, και
	// τα y μεγαλώνουν προς τα κάτω, οπότε πχ ο χαρακτήρας που έχει height=38,
	// αν θέλουμε να "κάθεται" πάνω στο δάπεδο, θα πρέπει να έχει y=-38.

	character->rect.width = 70;
	character->rect.height = 38;
	character->rect.x = 0;
	character->rect.y = - character->rect.height;

	// Δημιουργία των objects (πύλες / εμπόδια / εχθροί) και προσθήκη στο vector
	// state->objects. Η πίστα περιέχει συνολικά 4*PORTAL_NUM αντικείμενα, από
	// τα οποία τα PORTAL_NUM είναι πύλες, και τα υπόλοια εμπόδια και εχθροί.

	state->objects = vector_create(0, free);		// Δημιουργία του vector

	for (int i = 0; i < 4*PORTAL_NUM; i++) {
		// Δημιουργία του Object και προσθήκη στο vector
		Object obj = malloc(sizeof(*obj));
		vector_insert_last(state->objects, obj);

		// Κάθε 4 αντικείμενα υπάρχει μια πύλη. Τα υπόλοιπα αντικείμενα
		// επιλέγονται τυχαία.

		if(i % 4 == 3) {							// Το 4ο, 8ο, 12ο κλπ αντικείμενο
			obj->type = PORTAL;						// είναι πύλη.
			obj->rect.width = 100;
			obj->rect.height = 5;

		} else if(rand() % 2 == 0) {				// Για τα υπόλοιπα, με πιθανότητα 50%
			obj->type = OBSTACLE;					// επιλέγουμε εμπόδιο.
			obj->rect.width = 10;
			obj->rect.height = 80;

		} else {
			obj->type = ENEMY;						// Και τα υπόλοιπα είναι εχθροί.
			obj->rect.width = 30;
			obj->rect.height = 30;
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
		}

		// Τα αντικείμενα είναι ομοιόμορφα τοποθετημένα σε απόσταση SPACING
		// μεταξύ τους, και "κάθονται" πάνω στο δάπεδο.

		obj->rect.x = (i+1) * SPACING;
		obj->rect.y = - obj->rect.height;
	}

	// TODO: αρχικοποίηση της λίστας obj->portal_pairs
	List pairs = state->portal_pairs = list_create(free);
	for(int i=3, exit_num; i<vector_size(state->objects); i+=4){
		do {											//τυχαίος αριθμός από -1 εώς 4*PORTAL_NUM-1
			exit_num=(((rand()%(PORTAL_NUM+1))*4)-1); 	//που αντιστοιχεί σε αριθμό πύλης για την συγκεκριμένη
										  				 //υλοποίηση του vector state->objets
		} while(exit_num==-1); //-1 = η μόνη invalid τιμή που μπορεί να επιστρέψει η παραπάνω φόρμουλα
		Object exit = vector_get_at(state->objects,exit_num);
		PortalPair pair= portal_pair_create(vector_get_at(state->objects,i), exit);
		while (list_find(pairs,pair, compare_pair_exits)!= NULL){// όσο ο αριθμός της πύλης εξόδου υπάρχει στο σύνολο 
			exit_num+=4;
			if(exit_num > 4*PORTAL_NUM)
				exit_num-= 4*PORTAL_NUM;
			pair->exit= vector_get_at(state->objects,exit_num);
		}
		list_insert_next(state->portal_pairs, LIST_BOF, pair);
	}
	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state


StateInfo state_info(State state){
	assert(state!=NULL);
    StateInfo state_inf = &(state->info);
	return state_inf;
}


// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.


List state_objects(State state, float x_from, float x_to) {
	List oblist = list_create(NULL); //λίστα αντικειμένων που πρόκειται να επιστραφούν 
    Vector objects= state->objects;
	
     for(VectorNode node = vector_first(objects); 
     node!=VECTOR_EOF; node = vector_next(objects, node)){
        Object current_object = vector_node_value(objects,node);
        float x_coordinate = current_object->rect.x;
        if((x_coordinate > x_from) &&  (x_coordinate < x_to )){

    	   list_insert_next(oblist,LIST_BOF, current_object); //οι συντεταγμένες αποθηκεύονται με φθίνουσα σειρά 
       }
     }  
	return oblist;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	if(state->info.playing){
		if (!(state->info.paused)){
			if (keys->p)
				state->info.paused = true;
			else {
				character_collision_control(state);
				enemy_collision_control(state);
				character_position_update(state->info.character,keys);
				enemy_position_update(state->objects);
				nearest_portal(&(state->info));
			}
		}
		else if (keys->p){
				state->info.paused = false;
				character_collision_control(state);
				enemy_collision_control(state);
				character_position_update(state->info.character,keys);
				enemy_position_update(state->objects);
				nearest_portal(&(state->info));
		}
		else if (keys->n) {
			character_collision_control(state);
			enemy_collision_control(state);
			character_position_update(state->info.character,keys);
			enemy_position_update(state->objects);
			nearest_portal(&(state->info));
		}
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	//vector destroy objects 
	vector_destroy(state->objects);
	//list destroy pairs 
	list_destroy(state->portal_pairs);
	// destroy struct state_info 
	free(state->info.character);
	//finally destroy state  
	free(state);
}