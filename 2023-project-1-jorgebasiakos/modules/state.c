
#include <stdlib.h>
#include <stdio.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"
#include "ADTSet.h"

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.
//static int q = 0;

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};

static float speed_factor = 1.0;     //πολλαπλασιαστης ταχυτητας παιχνιδιου, μετα το generate νεων αντικειμενων

// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	return obj;
}

static void add_objects(State state, float start_x) {
	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.

	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 3*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 && (rand() % 10) == 0							// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		vector_insert_last(state->objects, platform);


		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			vector_insert_last(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = vector_create(0, free);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = vector_get_at(state->objects, 0);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 23,	// y πάνω στην πλατφόρμα
		46, 46,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	//printf("Ball -->x, y = %f,%f", state->info.ball->rect.x, state->info.ball->rect.y);
	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.


List state_objects(State state, float x_from, float x_to) {
	List result = list_create(NULL);
	for(VectorNode node = vector_first(state -> objects); node != VECTOR_EOF; node = vector_next(state -> objects, node)){
		Object obj = vector_node_value(state -> objects, node);
		if(obj->rect.x >= x_from && obj->rect.x <= x_to){
			list_insert_next(result, LIST_BOF, obj);
		}
	}
	return result;
}

void state_update(State state, KeyState keys) {	  // Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.  Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

	Object last_platform;

	if(!keys->p && state->info.paused == false && state->info.playing == true){

		if(state->info.ball->rect.y > SCREEN_HEIGHT){
			state->info.playing = false;
		}

		
		if(keys->right){
			state->info.ball->rect.x += 6;
		} else if(keys->left){
			state->info.ball->rect.x += 1;
		} else{
			state->info.ball->rect.x += 4;
		}

		if(state->info.ball->vert_mov == IDLE){
			state->info.ball->vert_speed = 17;
		}
		
		if(keys->up){
			if(state->info.ball->vert_speed == 17){
				state->info.ball->vert_mov = JUMPING;
				state->info.ball->rect.y -= state->info.ball->vert_speed;
				state->info.ball->vert_speed *= 0.85;
			} else if(state->info.ball->vert_speed > 0.5 && state->info.ball->vert_mov == JUMPING){
				state->info.ball->rect.y -= state->info.ball->vert_speed;
				state->info.ball->vert_speed *= 0.85;
			} else{
				state->info.ball->rect.y += state->info.ball->vert_speed;
				state->info.ball->vert_speed *= 1.1;
				state->info.ball->vert_mov = FALLING;
			}
		} else{
			if(keys->up){
				state->info.ball->vert_mov = IDLE;
			}
			if(state->info.ball->vert_mov == FALLING || state->info.ball->vert_mov == JUMPING){
				state->info.ball->vert_speed = 5;
				state->info.ball->rect.y += state->info.ball->vert_speed;
				if(keys->up && state->info.ball->vert_mov == FALLING){
					state->info.ball->vert_mov = IDLE;
				}
			}
		}
		
		for(VectorNode node = vector_first(state -> objects); node != VECTOR_EOF; node = vector_next(state -> objects, node)){
			Object obj = vector_node_value(state -> objects, node);
			
			if(obj->type == PLATFORM){

			last_platform = obj;

			if(CheckCollisionRecs(state->info.ball->rect, obj->rect)){
				state->info.ball->vert_mov = IDLE;
			}

			if(!CheckCollisionRecs(state->info.ball->rect, obj->rect) && obj->rect.x + 100 < state->info.ball->rect.x - 230 && !keys->up){
				state->info.ball->rect.y += 1.5*speed_factor;
			}

			

				if(obj->vert_mov == MOVING_UP){
					if(CheckCollisionRecs(state->info.ball->rect, obj->rect)){
						state->info.ball->rect.y = obj->rect.y - 23;
						if(obj->unstable){
							obj->vert_mov = FALLING;
						}
					}
					if(SCREEN_HEIGHT/4 < obj->rect.y){
						obj->rect.y -= obj->vert_speed*speed_factor; 
					}else{
						obj->vert_mov = MOVING_DOWN;
					}

					
				}
				if(obj->vert_mov == MOVING_DOWN){
					if(CheckCollisionRecs(state->info.ball->rect, obj->rect)){
						state->info.ball->rect.y = obj->rect.y - 23;
						if(obj->unstable){
							obj->vert_mov = FALLING;
						}
					}
					if(SCREEN_HEIGHT * 3/4 > obj->rect.y){
						obj->rect.y += obj->vert_speed*speed_factor;
					} else{
						obj->vert_mov = MOVING_UP;
					}
					
				}


				if(obj->vert_mov == FALLING){
					obj->rect.y += 5*speed_factor;
					if(CheckCollisionRecs(state->info.ball->rect, obj->rect)){
						state->info.ball->rect.y = obj->rect.y - 23;
					}

					if(obj->rect.y > SCREEN_HEIGHT){
				 	 	free(obj);
				    }
				}
			}

			if(obj->type == STAR){

				if(CheckCollisionRecs(state->info.ball->rect, obj->rect)){

					state->info.score +=10;
					free(obj); 

				}

			}
		}


		if(state->info.ball->rect.x > last_platform->rect.x - SCREEN_WIDTH){
			add_objects(state, last_platform->rect.x);
			speed_factor = speed_factor + speed_factor*0.1;
		}

	}else{
		if(keys->p){
			if(state->info.paused == false){
				state->info.paused = true;
			}else{
				state->info.paused = false;
			}
		}
		if(keys->enter){
			speed_factor = 1;
			state_destroy(state);
			state_create();
		}
	}

}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	free(state);
}

