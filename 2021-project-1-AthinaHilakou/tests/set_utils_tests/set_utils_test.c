//////////////////////////////////////////////////////////////////
//
// Test για το set_utils.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTSet.h"
#include "set_utils.h"


int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης
	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}

int compare_ints(Pointer a, Pointer b) {
    int* ia = a;
    int* ib = b;
    return *ia - *ib;       // αρνητικός, μηδέν ή θετικός, ανάλογα με τη διάταξη των a,b
}

void test_set_utils (){
   Set set = set_create(compare_ints, free);
   for(int i = 2; i<1000; i+=2){
	   int* value = create_int(i-1);
	   set_insert(set, create_int(i)); //πρόσθεσε στο set έναν άρτιο i 
	   int* val2  = set_find_eq_or_greater(set,value);// αναζήτησε τον περιττό i-1  
	   free(value);  
	    TEST_ASSERT( *val2 == i); 
		val2 =  set_find_eq_or_greater(set,val2);
		TEST_ASSERT (*val2 == i);
		TEST_ASSERT(set_find_node(set, value)== NULL);//εξέτασε αν το i-1 δεν υπάρχει στο set
		value = create_int(i+1);
		val2  = set_find_eq_or_greater(set,value);
		free(value);
		TEST_ASSERT(val2 == NULL);
   }
   set_destroy(set);

      set = set_create(compare_ints, free);
   for(int i = 2; i<1000; i+=2){
	   int* value = create_int(i);
	   set_insert(set, create_int(i-1)); //πρόσθεσε στο set έναν περιττό i 
	   int* val2  = set_find_eq_or_smaller(set,value);// αναζήτησε τον άρτιο i  
	   free(value);  
	    TEST_ASSERT( *val2 == i-1); 
		val2 =  set_find_eq_or_smaller(set,val2);
		TEST_ASSERT (*val2 == i-1);
		TEST_ASSERT(set_find_node(set, value)== NULL);//εξέτασε αν το i δεν υπάρχει στο set
		value = create_int(-i);
		val2 = set_find_eq_or_smaller(set,value);
		free(value);
		TEST_ASSERT(val2 == NULL);
	
   }
   set_destroy(set);
   
}

TEST_LIST = {
	
	{ "test_set_utils", test_set_utils},

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};