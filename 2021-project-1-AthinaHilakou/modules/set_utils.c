
#include "common_types.h"
#include "ADTSet.h"
#include <stdio.h>
#include <stdlib.h>
Pointer set_find_eq_or_greater(Set set, Pointer value){
    SetNode node;
    Pointer  val; 
    //δεν θέλουμε η value που έχει περάσει ο χρήστης σαν όρισμα να γίνει deallocate
    // θέτουμε το destroy func σε NULL για την αφάιρεση κόμβων μέσω της συνάρτησης
    // και μετά επαναφέρουμε την αρχική destroy func που είχε περάσει ο χρήστης  
     DestroyFunc Destroy = set_set_destroy_value(set, NULL); 
    if ((node = set_find_node(set,value)) != NULL){
        set_set_destroy_value(set, Destroy); 
        return set_node_value(set,node);
    }
    set_insert(set,value);
    if((node = set_next(set,set_find_node(set,value)))== NULL){
        set_remove(set,value);
        set_set_destroy_value(set, Destroy); 
        return NULL;
   }
    val = set_node_value(set,node);
    set_remove(set,value);
    set_set_destroy_value(set, Destroy); 
    return val;
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    SetNode node;
    Pointer  val; 
    //δεν θέλουμε η value που έχει περάσει ο χρήστης σαν όρισμα να γίνει deallocate
    // θέτουμε το destroy func σε NULL για την αφάιρεση κόμβων μέσω της συνάρτησης
    // και μετά επαναφέρουμε την αρχική destroy func που είχε περάσει ο χρήστης  
    DestroyFunc Destroy = set_set_destroy_value(set, NULL); 
    if ((node = set_find_node(set,value)) != NULL){
        set_set_destroy_value(set, Destroy); 
        return set_node_value(set,node);
    }
    set_insert(set,value);
   if((node = set_previous(set,set_find_node(set,value)))== NULL){
        set_remove(set,value);
        set_set_destroy_value(set, Destroy); 
        return NULL;
   }
    val = set_node_value(set,node);
    set_remove(set,value);
    set_set_destroy_value(set, Destroy); 
    return val;
}