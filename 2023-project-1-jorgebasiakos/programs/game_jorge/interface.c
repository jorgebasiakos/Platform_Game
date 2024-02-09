#include "raylib.h"

#include "state.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>

// Assets
Texture bird_img;
Sound game_over_snd;


void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game_jorge");
	SetTargetFPS(60);
    InitAudioDevice();

	// Φόρτωση εικόνων και ήχων
	bird_img = LoadTextureFromImage(LoadImage("assets/bird.png"));
	game_over_snd = LoadSound("programs/game_jorge/assets/game_over.mp3");
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(RAYWHITE);

	//sxediazw thn mpala

	StateInfo info = state_info(state);

	double offset = info->ball->rect.x - 300;
	
    DrawCircle(info->ball->rect.x - offset, info->ball->rect.y, 23, ORANGE);

	List objs = state_objects(state, info->ball->rect.x - 300, info->ball->rect.x + SCREEN_WIDTH - 300);


	for(ListNode node = list_first(objs); node != LIST_EOF; node = list_next(objs, node)){
		Object obj = list_node_value(objs, node);

		if(obj->type == PLATFORM){

			if(obj->unstable == true){
			
				DrawRectangle(obj->rect.x - offset, obj->rect.y, obj->rect.width, 15, RED);
			
			}else{

				DrawRectangle(obj->rect.x - offset, obj->rect.y, obj->rect.width , 15, BLUE);
			
			}
		}
		if(obj->type == STAR){

			DrawCircle(obj->rect.x - offset, obj->rect.y, 10, GREEN);
		
		}
	}
	


	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", info->score ), 20, 20, 40, GRAY);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!info->playing) {
		PlaySound(game_over_snd);
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
		
	EndDrawing();
}
