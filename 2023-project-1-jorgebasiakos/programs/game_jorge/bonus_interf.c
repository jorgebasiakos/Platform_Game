#include "raylib.h"

#include "bonus_state.h"
#include "bonus_interf.h"
#include <stdio.h>
#include <stdlib.h>

// Assets
Texture bird_img;
Sound game_over_snd;
Texture2D background;

float scrollingBack = 0.0f;

void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game_jorge");
	SetTargetFPS(60);
    InitAudioDevice();

	// Φόρτωση εικόνων και ήχων
	background = LoadTexture("assets/bg.png");
	bird_img = LoadTextureFromImage(LoadImage("assets/bird.png"));
	game_over_snd = LoadSound("assets/go.mp3");
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	scrollingBack -= 0.9f;
	BeginDrawing();
	ClearBackground(RAYWHITE);
	DrawTextureEx(background, (Vector2){ scrollingBack, 20 }, 0.0f, 2.0f, WHITE);
	DrawTextureEx(background, (Vector2){ background.width*2 + scrollingBack, 20 }, 0.0f, 2.0f, WHITE);

	//sxediazw thn mpala
	StateInfo info = state_info(state);

	double offset = info->ball->rect.x - 300;
	
    //DrawCircle(info->ball->rect.x - offset, info->ball->rect.y, 23, ORANGE);
	DrawTexture(bird_img, info->ball->rect.x - offset, info->ball->rect.y, WHITE);

	List objs = state_objects(state, info->ball->rect.x - 300, info->ball->rect.x + SCREEN_WIDTH - 300);

	
	for(ListNode node = list_first(objs); node != LIST_EOF; node = list_next(objs, node)){
		Object obj = list_node_value(objs, node);

		if(obj->type == PLATFORM){

			if(obj->unstable == true){
			
				//DrawTexture(bird_img, obj->rect.x - offset, obj->rect.y, WHITE);
				DrawRectangle(obj->rect.x - offset, obj->rect.y, obj->rect.width, 15, RED);
			
			}else{
				//DrawTexture(bird_img, obj->rect.x - offset, obj->rect.y, BLACK);
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
	if (info->playing) {
		PlaySound(game_over_snd);
	}else{
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
	
	EndDrawing();
}
