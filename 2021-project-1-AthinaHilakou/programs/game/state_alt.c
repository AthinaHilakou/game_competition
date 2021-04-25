
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "ADTSet.h"
#include "ADTMap.h"
#include "state.h"
#include "set_utils.h"

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	Map entrance_to_exit;		// περιέχει μια αντιστοίχιση πυλών είσοδο προς έξοδο 
	Map exit_to_entrance;		// περιέχει μια αντιστοίχιση πυλών έξοδο προς είσοδο 
	struct state_info info;
};







/*----------------------------------------------------------------------------------------------------------------*/
int compare_objects (Pointer a, Pointer b){
	Object oba = a;
	Object obb = b; 	
if (oba->rect.x != obb->rect.x){
	return oba->rect.x - obb->rect.x;
}
	return (int)(a - b); 
}
//Δεμσεύει μνήμη για ένα float στο σωρό
float* create_float(float value) {
	float* pointer = malloc(sizeof(float));		// δέσμευση μνήμης
	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}
//Σύγκριση floating point αριθμών 
//Αξιοποιείται στην αρχικοποίηση των portal pairs μέσω της state_crearte
int compare_floats(Pointer a, Pointer b) {
    float* ia = a;
    float* ib = b;
    return *ia - *ib;       // αρνητικός, μηδέν ή θετικός, ανάλογα με τη διάταξη των a,b
}




Object create_object(Object obj) {
	Object temp_obj = malloc(sizeof(*temp_obj));
	temp_obj->rect.x = obj->rect.x;
	temp_obj->rect.y = obj->rect.y;
	temp_obj->rect.width = obj->rect.width;
	temp_obj->rect.height = obj->rect.height;
	temp_obj->type = obj->type;
	temp_obj->forward = obj->forward;
	temp_obj->jumping = obj->jumping;
	return temp_obj;
}


//Δεμσεύει μνήμη για ένα int στο σωρό
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


int compare_entrances_or_exits(Pointer a, Pointer b){ //συγκρίνει δύο εισόδους ή εξόδους portal pair
	Object ob_a = a;						// που είναι καταχωρημένα στο map entrance_to_exit ή exit_to_entrance
	Object ob_b = b;						//ένα object potral μπορεί να θεωρηθεί είτε entrance 
	float x_coordinate_a = ob_a->rect.x;	// είτε exit ανάλογα με τους σκοπούς της αναζήτησης 
	float x_coordinate_b = ob_b->rect.x;	// αυτό καθίσταται δυνατό καθώς τα keys και τα values του map  
	return (int)(x_coordinate_a - x_coordinate_b);//ανήκουν στο ίδιο σύνολο (set-objects)

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

void enemy_position_update(Set objects){
	SetNode node =  set_first(objects);
	while (node != SET_EOF){
		Object enemy = set_node_value(objects, node);
		SetNode temp_node;
		Object temp_enemy;
		if(enemy->type != ENEMY){
			node = set_next(objects, node);
			continue;
		}


		if(enemy->forward){
			node = set_next(objects, node);
			if (node != NULL){	
				temp_enemy = set_node_value(objects, node);
				//αν χαλάει η σχέση διάταξης του set objects
				//αφαίρεσε το τρέχον αντικείμενο από το set και ξαναβάλε το, με τις ενημερωμένες συντεταγμένες
				if(enemy->rect.x + 5 > temp_enemy->rect.x){ 
					temp_enemy = create_object(enemy);
					set_remove(objects, enemy);
					temp_enemy->rect.x += 5;
					set_insert(objects, temp_enemy);
				}
				else{
					enemy->rect.x += 5;
				}
			}
			else{
				enemy->rect.x += 5;
			}
		}
		else{
			temp_node = set_previous(objects, node);
			if (temp_node != NULL){	
				temp_enemy = set_node_value(objects, node);
				if(enemy->rect.x - 5 < temp_enemy->rect.x){
					temp_enemy = create_object(enemy);
					node = set_next(objects, node);
					set_remove(objects, enemy);
					temp_enemy->rect.x -= 5;
				set_insert(objects, temp_enemy);
				}
				else{ 
					enemy->rect.x -= 5;
					node = set_next(objects, node);
				}
			}
			else{
				enemy->rect.x -= 5;
				node = set_next(objects, node);
			}
		}
	}
}

void character_collision_control(State state){
	Object temp_obj = malloc(sizeof(*temp_obj));
	temp_obj->rect.x = state->info.character->rect.x;
	Object object;
	Object character = state->info.character;
	if(character->forward){ 			//Αν ο Χαρακτήρας συγκρουστεί με μια Πύλη A ενώ κινείται προς τα δεξιά 
		if ((object = set_find_eq_or_greater(state->objects, temp_obj))!= NULL){
			free(temp_obj);		
			if (object != NULL){
				if(CheckCollisionRecs(state->info.character->rect,object->rect) && object->type != PORTAL )
					state->info.playing = false; 
				else if (CheckCollisionRecs(state->info.character->rect,object->rect)){
					if (object->rect.x == 4*PORTAL_NUM*SPACING){ // σύγκρουση με την τελευταία πύλη 
						character->rect.x = 0;
						state->info.wins += 1;
					}
					else{
						float* entrance = &(object->rect.x);
						float* exit = map_find(state->entrance_to_exit,entrance);
						//Αν ο Χαρακτήρας συγκρουστεί με μια Πύλη A ενώ κινείται προς τα δεξιά 
						state->info.character->rect.x = ((*exit) + (object->rect.width));//μετακινείται στην αντίστοιχη πύλη B στην οποία οδηγεί η Α.
					}
				}
			}
		}
	}
	else {
		if ((object = set_find_eq_or_smaller(state->objects, temp_obj))!= NULL){
			free(temp_obj);		
			if (object != NULL){
				if(CheckCollisionRecs(state->info.character->rect,object->rect) && object->type != PORTAL )
					state->info.playing = false; 
				else if (CheckCollisionRecs(state->info.character->rect,object->rect)){
					if (object->rect.x == 4*PORTAL_NUM*SPACING){ // σύγκρουση με την τελευταία πύλη 
						character->rect.x = 0;
						state->info.wins += 1;
					}
					else{
						float* exit = &(object->rect.x);
						float* entrance = map_find(state->exit_to_entrance,exit); //Αν κινείται ανάποδα, τότε η μετακίνηση είναι αντίστροφη
						character->rect.x = ((*entrance)-(object->rect.width)); //δηλαδή μετακινείται στην πύλη B η οποία οδηγεί στην A.
					}
				}
			}
		}					
	}
}


void enemy_collision_control(State state){
	SetNode node = set_first(state->objects);
	while(node!= SET_EOF){ // για κάθε εχθρό 
		 Object obj1 = set_node_value(state->objects, node);
		 Object temp_obj = malloc(sizeof(*temp_obj));
		 Object obj2;
		if (obj1->type != ENEMY){ 
		node=set_next(state->objects, node);
		continue;
		}
		if(obj1->forward){ 			//Αν το obj1 συγκρουστεί με μια Πύλη A ενώ κινείται προς τα δεξιά 
			if ((obj2 = set_find_eq_or_greater(state->objects, temp_obj))!= NULL){	
				free(temp_obj);
				if (obj2 != NULL){
					if(CheckCollisionRecs(obj1->rect,obj2->rect) && obj2->type == OBSTACLE ){
						obj1->forward = !(obj1->forward);
						node=set_next(state->objects, node);
					}	 
					else if (CheckCollisionRecs(obj1->rect,obj2->rect) && obj2->type == PORTAL){
							float* entrance = &(obj2->rect.x);
							float* exit = map_find(state->entrance_to_exit,entrance);//Αν ο εχθρός συγκρουστεί με μια Πύλη A ενώ κινείται προς τα δεξιά 
							node=set_next(state->objects, node);
							Object new_obj1 = create_object(obj1);//μετακινείται στην αντίστοιχη πύλη B στην οποία οδηγεί η Α.
							set_remove(state->objects, obj1); //η διάταξη του set θα επηρεαστεί
							new_obj1->rect.x = ((*exit)+(obj2->rect.width)/2); //γι'αυτό αφαιρούμε και ξαναβάζουμε στο set το obj1 
							set_insert(state->objects, new_obj1);
					}
				}
				else 
					node=set_next(state->objects, node);
			}
			else 
				node=set_next(state->objects, node);
		}
		else {
			if ((obj2 = set_find_eq_or_smaller(state->objects, temp_obj))!= NULL){
				free(temp_obj);		
				if (obj2 != NULL){
					if(CheckCollisionRecs(obj1->rect,obj2->rect) && obj2->type == OBSTACLE ){
						obj1->forward = !(obj1->forward);
						node=set_next(state->objects, node);
					}
					else if (CheckCollisionRecs(obj1->rect,obj2->rect) &&  obj2->type == PORTAL){
							float* exit = &(obj2->rect.x);
							float* entrance = map_find(state->exit_to_entrance, exit); //Αν κινείται ανάποδα, τότε η μετακίνηση είναι αντίστροφη
							node=set_next(state->objects, node);
							Object new_obj1 = create_object(obj1);//δηλαδή μετακινείται στην πύλη B η οποία οδηγεί στην A.
							set_remove(state->objects, obj1);//η διάταξη του set θα επηρεαστεί
							new_obj1->rect.x = ((*entrance) + (obj2->rect.width)/2);//γι'αυτό αφαιρούμε και ξαναβάζουμε στο set το obj1  
							set_insert(state->objects, new_obj1);
					}
				}
				else 
					node=set_next(state->objects, node);
			}
			else
				node=set_next(state->objects, node);
		}
	}
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

	state->objects = set_create(compare_objects, free);		// Δημιουργία του set 
	for (int i = 0; i < 4*PORTAL_NUM; i++) {
		// Δημιουργία του Object και προσθήκη στο vector
		Object obj = malloc(sizeof(*obj));
		
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
		set_insert(state->objects, obj);
	}

    // αρχικοποίηση των map entrance_to_exit και exit_to_entrance που περιέχουν μια 1-1 αντιστοίχιση 
	//εισόδων-εξόδων και εξόδων-εισόδων αντίστοιχα 
    
	state->entrance_to_exit = map_create(compare_floats,free, free);
	state->exit_to_entrance = map_create(compare_floats,free, free);
	Set exits = set_create(compare_ints, free); //αντιστοίχιση με τη βοήθεια ενός set 
												// που ελέγχει αν μια πύλη έχει ήδη αντιστοιχιστεί σ είσοδοο
	for(int i=3, exit_num; i<set_size(state->objects); i+=4){// στην αρχική κατάσταση του παιχνιδιού είναι δεδομένο
															// ότι τα objets με i%4=3 είναι portals
		do {											//τυχαίος αριθμός από -1 εώς 4*PORTAL_NUM-1
			exit_num=(((rand()%(PORTAL_NUM+1))*4)-1); 	//που αντιστοιχεί σε αριθμό πύλης για την συγκεκριμένη
										  				 //υλοποίηση του vector state->objets
		} while(exit_num==-1); //-1 = η μόνη invalid τιμή που μπορεί να επιστρέψει η παραπάνω φόρμουλα 
		while (set_find(exits, &exit_num)!= NULL){// όσο ο αριθμός της πύλης εξόδου υπάρχει στο σύνολο 
			exit_num+=4;
			if(exit_num > 4*PORTAL_NUM)
				exit_num-= 4*PORTAL_NUM;
		}
		set_insert(exits,create_int(exit_num));
		map_insert(state->entrance_to_exit,create_float((i+1) * SPACING),create_float((exit_num+1) * SPACING));
		map_insert(state->exit_to_entrance,create_float((exit_num+1) * SPACING),create_float((i+1) * SPACING));
	}
	set_destroy(exits);

 

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
    Set objects= state->objects;
	Object first_obj = malloc(sizeof(*first_obj));
	first_obj->rect.x = x_from;
	Object last_obj = malloc(sizeof(*last_obj));
	last_obj->rect.x = x_to;

	Object first = set_find_eq_or_greater(objects, first_obj);
	free(first_obj);
	Object lastl = set_find_eq_or_smaller(objects, last_obj);
	free(last_obj);
	if (first == NULL || lastl == NULL) return oblist;
	SetNode last = set_find_node(objects, lastl);
	SetNode node = set_find_node(objects, first);
    for(;node != last; node = set_next(objects, node)){
		Object obj = set_node_value(objects, node);
		list_insert_next(oblist,LIST_BOF,obj); //οι συντεταγμένες αποθηκεύονται με φθίνουσα σειρά 
	
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
	set_destroy(state->objects);
	//map destroy entrance_to_exit
	map_destroy(state->entrance_to_exit);
	//map destroy exit_to_entrance
	map_destroy(state->exit_to_entrance);
	// destroy struct state_info 
	free(state->info.character);
	//finally destroy state  
	free(state);
}