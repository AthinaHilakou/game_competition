#include "raylib.h"
#include <assert.h>
#include "state.h"
#include "interface.h"
#include <stdio.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define CHAR_ON_SCREEN_X 160
#define CHAR_ON_SCREEN_Y ((SCREEN_HEIGHT)-(SCREEN_HEIGHT)/5) // κοντά στη χρυσή τομή για να τέρπεται ο παίκτης 


// Assets
Texture giorgio_img;
Texture pixelbot_img;
Sound waldstein_sonata;
Sound game_over_snd;




void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "giorgio's bizare adventure");
	SetTargetFPS(60);
    InitAudioDevice();

	// Φόρτωση εικόνων και ήχων
    giorgio_img = LoadTextureFromImage(LoadImage("assets/giorgio_ridin.png"));
    game_over_snd = LoadSound("assets/game_over.mp3");
}




void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}




// Draw game (one frame)
void interface_draw_frame(State state) {
	//Άντλησε πληροφορίες για την κατάσταση του παιχνιδιού και ξεκίνα να ζωγραφίζεις το frame 
    BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(RAYWHITE);

    //Σχεδίασε το επίπεδο όπου πατάνε τα αντικείμενα
 
    StateInfo info = state_info(state);
    int startPosY = CHAR_ON_SCREEN_Y + (info->character->rect.height);
    DrawLine(0,startPosY, SCREEN_WIDTH,startPosY, BLACK);  

	// Σχεδιάζουμε τον χαρακτήρα
	DrawTexture(giorgio_img, CHAR_ON_SCREEN_X,CHAR_ON_SCREEN_Y + info->character->rect.y, WHITE);
	
    //Σχεδίασε τα αντικείμενα που είναι ορατά στην οθόνη βάσει της θέσης του χαρακτήρα 
    float x_from = info->character->rect.x - CHAR_ON_SCREEN_X*2;
    float x_to = info->character->rect.x + (SCREEN_WIDTH - CHAR_ON_SCREEN_X)*2;
    
    List list = state_objects(state,x_from,x_to); 
    
    for(ListNode node = list_first(list); node!=LIST_EOF; node = list_next(list,node)){
        Object obj = list_node_value(list, node);
        float x_pos = CHAR_ON_SCREEN_X - (info->character->rect.x - obj->rect.x);
         
        if(obj->type == ENEMY){
            DrawRectangle(x_pos,(CHAR_ON_SCREEN_Y - (obj->rect.height)/2),obj->rect.width,obj->rect.height,RED);
        }
        else if(obj->type == OBSTACLE){ 
            DrawRectangle(x_pos,(CHAR_ON_SCREEN_Y - (obj->rect.height)/2),obj->rect.width,obj->rect.height,GREEN);
        }
        else if(obj->type == PORTAL){
            DrawRectangle(x_pos,(CHAR_ON_SCREEN_Y + 2*(obj->rect.height)),obj->rect.width,obj->rect.height,BLACK);

       }
    }
    list_destroy(list);


	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawFPS(SCREEN_WIDTH - 80, 0);
    
    //Αν το η waldstein_sonata έχει τελειώσει ξαναπαίξε την 
    // if(!IsSoundPlaying(waldstein_sonata)){
    //     PlaySound(waldstein_sonata);
    // }

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!info->playing) {
        //StopSound(waldstein_sonata);
        PlaySound(game_over_snd);
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
    else if (info->playing){
        DrawText("PORTAL",20,20, 30, GRAY);
        DrawText(TextFormat("%04i",info->current_portal),MeasureText("PORTAL", 20)*(5/2), 20, 30, GRAY);
        DrawText( "WINS",20,50,30, GRAY);
        DrawText(TextFormat("%04i",info->wins),MeasureText("PORTAL", 20)*(5/2) , 50, 30, GRAY);
        
    }
    else if(info->paused){
        DrawText(
			"PRESS P TO CONTINUE",
			 GetScreenWidth() / 2 - MeasureText("PRESS P TO CONTINUE", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);


    }

	EndDrawing();
}



