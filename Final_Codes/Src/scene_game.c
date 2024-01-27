#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"


// TODO-HACKATHON 2-0: Create one ghost
// Just modify the GHOST_NUM to 1
#define GHOST_NUM 4
// TODO-GC-ghost: create a least FOUR ghost!
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
int game_main_Score = 0;
bool game_over = false;
extern bool pacman_pass_wall;
/* Internal variables*/
static ALLEGRO_TIMER* power_up_timer;
static const int power_up_duration = 10;
static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;
bool debug_mode = false;
bool cheat_mode = false;
ALLEGRO_SAMPLE_ID gameBGM;
ALLEGRO_SAMPLE_ID finishBGM;
//my
bool win;
bool ghost_freeze = false;
bool finishBGM_playing = false;
ALLEGRO_TIMER* win_timer;
FILE *fptr = NULL;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(int btn, int x, int y, int dz);
static void render_init_screen(void);
static void draw_hitboxes(void);
typedef struct{
	RecArea body;
	bool hovered;
}popupButton;
typedef struct {
	RecArea body;
	char* enter;
	bool clicked;
	popupButton finish_enter;
	bool hovered;
	int idx;
	bool finish;
}Enter_field;
typedef struct {
	char* name;
	int second;
}Tuple;

typedef struct{
	bool open;
	popupButton button;
	popupButton close;
	RecArea body;
	Enter_field enter_field;
	Tuple* record[6];
	int record_nums;
}High_score_table;
High_score_table table;
static void init_high_score_table(){
	table.open = false;
	//
	table.body.x = 150;
	table.body.y = 150;
	table.body.w = 500;
	table.body.h = 500;
	//button
	table.button.body.w = 250;
	table.button.body.h = 45;
	table.button.body.x = (SCREEN_W-table.button.body.w)/2;
	table.button.body.y = SCREEN_H-450;
	table.button.hovered = 0;
	//enter_field
	table.enter_field.body.w = 300;
	table.enter_field.body.h = 36;
	table.enter_field.body.x = (SCREEN_W-table.enter_field.body.w-80)/2;
	table.enter_field.body.y = SCREEN_H-560;
	table.enter_field.hovered = 0;
	table.enter_field.enter = (char*)malloc(sizeof(char)*21);
	table.enter_field.enter[0] = '\0';
	table.enter_field.idx = 0;
	table.enter_field.finish = 0;
	//enter
	table.enter_field.finish_enter.body.x = table.enter_field.body.x+table.enter_field.body.w;
	table.enter_field.finish_enter.body.y = table.enter_field.body.y;
	table.enter_field.finish_enter.body.w = 80;
	table.enter_field.finish_enter.body.h = table.enter_field.body.h;
	table.enter_field.finish_enter.hovered = 0;
	//close
	table.close.body.x = table.body.x + table.body.w - 50;
	table.close.body.y = table.body.y+20;
	table.close.body.w = 30;
	table.close.body.h = 30;
	table.close.hovered = 0;
	// scanf file
	fptr = fopen("Assets/highest_record.txt","r");
	if(fptr == NULL){
		game_abort("Can't find the highest_record.txt");
	}
	game_log("%s","success open the file");
	fscanf(fptr,"%d", &table.record_nums);
	game_log("tuple number: %d",table.record_nums);
	//table.record = (Tuple*)malloc(sizeof(Tuple)*(table.record_nums+1));
	for(int i = 0 ; i < table.record_nums ; i++){
		table.record[i] = (Tuple*)malloc(sizeof(Tuple));
		table.record[i]->name = (char*)malloc(sizeof(char)*21);
		fscanf(fptr, "%s", table.record[i]->name);
		fscanf(fptr, "%d", &table.record[i]->second);
		game_log("%s %d",table.record[i]->name, table.record[i]->second);
	}
	fclose(fptr);
	fptr = fopen("Assets/highest_record.txt","w");
}
static void init(void) {
	game_over = false;
	game_main_Score = 0;
	ghost_freeze = 0;
	win = 0;
	// create map
	basic_map = create_map(NULL);
	// TODO-GC-read_txt: Create map from .txt file so that you can design your own map!!
	basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}	
	// create pacman
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacamn\n");
	}
	
	// allocate ghost memory
	// TODO-HACKATHON 2-1: Allocate dynamic memory for ghosts array.
	
	ghosts = (Ghost**)malloc(sizeof(Ghost*) * GHOST_NUM);
	
	if(!ghosts){
		game_log("We haven't create any ghosts!\n");
	}
	else {
		// TODO-HACKATHON 2-2: create a ghost.
		// Try to look the definition of ghost_create in ghost.h and figure out what should be placed here.
		for (int i = 0; i < GHOST_NUM; i++) {
			
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);  
			if (!ghosts[i])
				game_abort("error creating ghost\n");
			
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick per second
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	//my
	stop_bgm(gameBGM);
	gameBGM = play_bgm(bgmMusic, music_volume);
	init_high_score_table();
	win_timer = al_create_timer(1.0);
	al_set_timer_count(win_timer, 0);
	al_start_timer(win_timer);
	return ;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	// TODO-HACKATHON 1-3: check which item you are going to eat and use `pacman_eatItem` to deal with it.
	//game_log("location is (%d, %d)",Grid_y,Grid_x);
	switch (basic_map->map[Grid_y][Grid_x])
	{
		//game_log("now char is %c , location is (%d, %d)",basic_map->map[Grid_x][Grid_y],Grid_x,Grid_y);
		case '.':
			pacman_eatItem(pman, '.');
			game_main_Score ++;
			if(game_main_Score == 2/*basic_map->beansCount*/){
				//game_over = 1;
				win = 1;
				stop_bgm(gameBGM);
				//stop_bgm(PACMAN_MOVESOUND_ID);
				ghost_freeze = 1;
			}
			break;
		case 'P':
			// TODO-GC-PB: ease power bean
			pacman_eatItem(pman, 'P');
			game_log("powerUP mode");
			// stop and reset power_up_timer value
			// start the timer
			for(int i = 0 ; i < GHOST_NUM ; i++){
				ghost_toggle_FLEE(ghosts[i],true);
			}
			al_stop_timer(power_up_timer);
			al_set_timer_count(power_up_timer,0);
			al_start_timer(power_up_timer);
			break;
		default:
			break;
	}
	
	// TODO-HACKATHON 1-4: erase the item you eat from map
	// Be careful, don't erase the wall block.
	/*
		basic_map->map...;
	*/
	//if(basic_map->map[Grid_x][Grid_y] == '.') 
	if(basic_map->map[Grid_y][Grid_x] == '.') basic_map->map[Grid_y][Grid_x] = ' ';
	if(basic_map->map[Grid_y][Grid_x] == 'P') basic_map->map[Grid_y][Grid_x] = ' '; 
}
static void status_update(void) {
	// TODO-PB: check powerUp duration
	
	if (pman->powerUp)
	{
		// Check the value of power_up_timer
		// If runs out of time reset all relevant variables and ghost's status
		// hint: ghost_toggle_FLEE
		if(al_get_timer_count(power_up_timer) >= power_up_duration){
			pman->powerUp = false;
			for(int i = 0 ; i < GHOST_NUM ; i++){
				ghost_toggle_FLEE(ghosts[i], false);
			}
		}
	}
	


	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN){
			continue;
		}
		else if (ghosts[i]->status == FREEDOM)
		{
			// TODO-GC-game_over: use `getDrawArea(..., GAME_TICK_CD)` and `RecAreaOverlap(..., GAME_TICK_CD)` functions to detect if pacman and ghosts collide with each other.
			// And perform corresponding operations.
			// [NOTE] You should have some if-else branch here if you want to implement power bean mode.
			// Uncomment Following Code
			RecArea ra = getDrawArea(&ghosts[i]->objData, GAME_TICK_CD);
			RecArea rb = getDrawArea(&pman->objData, GAME_TICK_CD);
			if(!cheat_mode && RecAreaOverlap(&ra, &rb) && !win)
			{
					game_log("collide with ghost\n");
					stop_bgm(gameBGM);
					al_rest(1.0);
					pacman_die();
					game_over = true;
					break; // animation shouldn't be trigger twice.
			}
		}
		else if (ghosts[i]->status == FLEE)
		{
			// TODO-GC-PB: if ghost is collided by pacman, it should go back to the cage immediately and come out after a period.
			RecArea ra = getDrawArea(&ghosts[i]->objData, GAME_TICK_CD);
			RecArea rb = getDrawArea(&pman->objData, GAME_TICK_CD);
			if(RecAreaOverlap(&ra, &rb))
			{
				ghost_collided(ghosts[i]);
			}
			
		}
	}
}

static void update(void) {

	if (game_over) {
		// TODO-GC-game_over: start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
		// hint: refer al_get_timer_started(...), al_get_timer_count(...), al_stop_timer(...), al_rest(...)
		
			// start the timer if it hasn't been started.
			// check timer counter.
			// stop the timer if counter reach desired time.
			if(!al_get_timer_count(pman->death_anim_counter)){
				al_start_timer(pman->death_anim_counter);

			}else if(al_get_timer_count(pman->death_anim_counter) > 12){
				al_stop_timer(pman->death_anim_counter);
				game_change_scene(scene_menu_create());
			}
		
		return;
	}
	if(win && !finishBGM_playing){
		finishBGM_playing = true;
		stop_bgm(gameBGM);
		finishBGM = play_bgm(finishMusic, music_volume);
		al_stop_timer(win_timer);
	}

	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++){
		if(ghost_freeze) break;
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
	}
}
static void draw_not_open_table(){
	if(!table.button.hovered)
	al_draw_filled_rounded_rectangle(table.button.body.x, table.button.body.y, 
					table.button.body.x + table.button.body.w, table.button.body.y + table.button.body.h,
					10,10,
					al_map_rgb(0x13,0x38,0xBE));
	else
		al_draw_filled_rounded_rectangle(table.button.body.x, table.button.body.y, 
					table.button.body.x + table.button.body.w, table.button.body.y + table.button.body.h,
					10,10,
					al_map_rgb(0x63,0xC5,0xDA));
	float text_width = al_get_text_width(font_pirulen_32,"Highest Record");
	float text_height = al_get_font_line_height(font_pirulen_32);
	al_draw_text(font_pirulen_32, al_map_rgb(255,255,255), table.button.body.x +(table.button.body.w - text_width)/2, table.button.body.y +(table.button.body.h-text_height)/2, 
				0,"Highest Record");
}
static void drwa_open_table(){
	//window
	al_draw_filled_rectangle(table.body.x, table.body.y, 
							table.body.x+table.body.w, table.body.y+table.body.h, 
							al_map_rgb(0xC1,0xBE,0xBF));
	//title
	al_draw_text(menuFont, al_map_rgb(0xFF, 0xD7, 0x00), table.body.x+20, table.body.y+20, 0,"HALL OF FAME");
	//enter field
	al_draw_filled_rectangle(table.enter_field.body.x, table.enter_field.body.y, 
							table.enter_field.body.x+table.enter_field.body.w, table.enter_field.body.y+table.enter_field.body.h,
							al_map_rgb(255,255,255));
	//enter button 
	al_draw_rectangle(table.enter_field.finish_enter.body.x, table.enter_field.finish_enter.body.y, 
							table.enter_field.finish_enter.body.x+table.enter_field.finish_enter.body.w, 
							table.enter_field.finish_enter.body.y+table.enter_field.finish_enter.body.h,
							al_map_rgb(0,0,0), 0);
	float text_x = al_get_text_width(font_pirulen_32, "Enter");
	float text_y = al_get_font_line_height(font_pirulen_32);
	float x = table.enter_field.finish_enter.body.x+(table.enter_field.finish_enter.body.w-text_x)/2;
	float y = table.enter_field.finish_enter.body.y+(table.enter_field.finish_enter.body.h - text_y)/2;
	al_draw_text(font_pirulen_32, al_map_rgb(0,0,0), x,y,0,"Enter");

	//close button
	al_draw_filled_rounded_rectangle(
        table.close.body.x, table.close.body.y,
		table.close.body.x+table.close.body.w, table.close.body.y + table.close.body.h,
        10, 10, al_map_rgb(255, 0, 0)
    );

    al_draw_line(
        table.close.body.x+10, table.close.body.y +10,
        table.close.body.x+table.close.body.w-10, table.close.body.y+table.close.body.h-10,
        al_map_rgb(255, 255, 255), 3
    );
    al_draw_line(
        table.close.body.x+10, table.close.body.y+table.close.body.h-10,
        table.close.body.x+table.close.body.w-10, table.close.body.y+10,
        al_map_rgb(255, 255, 255), 3
    );

	//enter_field
	int words_count = strlen(table.enter_field.enter);
	char tmp[21];
	sprintf(tmp,"%s",table.enter_field.enter);
	const char *chr = tmp;
	float offset = al_get_text_width(font_pirulen_32,tmp);
	if(table.enter_field.clicked){
		if(((al_get_timer_count(game_tick_timer) >> 6) & 1) == 0){
			al_draw_line(5 + table.enter_field.body.x+offset, table.enter_field.body.y, 
						5 + table.enter_field.body.x+offset, table.enter_field.body.y+table.enter_field.body.h, 
						al_map_rgb(0,0,0),ALLEGRO_ALIGN_LEFT);
		}
	}
	for(int i = 0 ; i < words_count ; i++){
		/*
		char tmp = table.enter_field.enter[i]; 
		const char *chr = &tmp;
		*/
		//char tmp[21];
		//sprintf(tmp,"%s",table.enter_field.enter);
		//const char *chr = tmp;
		//al_get_text_width(font_pirulen_32, chr);
		al_draw_text(font_pirulen_32, al_map_rgb(0,0,0), 5+table.enter_field.body.x, 
					table.enter_field.body.y,0,chr);
	}

	//tuple
	al_draw_filled_rectangle(table.enter_field.body.x, table.enter_field.body.y + 60 + table.enter_field.idx * 40,
							table.enter_field.body.x+table.enter_field.body.w+table.enter_field.finish_enter.body.w, 
							table.enter_field.body.y + 60 + (table.enter_field.idx+1) * 40, al_map_rgb(0x63,0xC5, 0xDA));

	for (int i = 0; i < 5 && i < table.record_nums; i++) {
    char record_str[100];
    sprintf(record_str, "%d. %s - %d seconds", i + 1, table.record[i]->name, table.record[i]->second);
    al_draw_text(
        font_pirulen_32,
        al_map_rgb(0, 0, 0),
        table.enter_field.body.x,
        table.enter_field.body.y + 60 + i * 40,
        0,
        record_str
    );
	if(table.enter_field.idx >= 5){
		sprintf(record_str, "%d+. %s - %d seconds",5, table.record[5]->name, table.record[5]->second);
    
		al_draw_text(
			font_pirulen_32,
			al_map_rgb(0, 0, 0),
			table.enter_field.body.x,
			table.enter_field.body.y + 60 + 5 * 40,
			0,
			record_str
		);
	}

}
	

}
static void winner_draw(){
    al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba(54, 54, 54, 230));

    draw_map(basic_map);
    pacman_draw(pman);
    for (int i = 0; i < GHOST_NUM; i++) {
        ghost_draw(ghosts[i]);
    }

    al_draw_text(
        font_pirulen_50,
        al_map_rgb(255, 255, 255),
        SCREEN_W/2, SCREEN_H/2-150,
        ALLEGRO_ALIGN_CENTER,
        "You Win"
    );
	if(!table.open){
		draw_not_open_table();
	}else{
		drwa_open_table();
	}
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"<ENTER> Back to menu"
	);

    return;
    
}
static void draw(void) {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	if(win){
		winner_draw();
		return;
	}
	// TODO-GC-scoring: Draw scoreboard, something your may need is sprinf();
		char str[100];
		char str2[100];
		sprintf(str,"%d",game_main_Score);
		strcpy(str2,"Scores: ");
		strcat(str2,str);
		al_draw_text(menuFont, al_map_rgb(0x28,0x32,0xC2), 0,0,0,str2);
	

	draw_map(basic_map);

	pacman_draw(pman);
	if (game_over)
		return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);
	
	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}

}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea((object *)pman, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea((object *)ghosts[i], GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
	// TODO-GC-memory: free map array, Pacman and ghosts
	delete_map(basic_map);
	pacman_destroy(pman);
	stop_bgm(gameBGM);
	for(int i = 0 ; i < GHOST_NUM ; i++) ghost_destory(ghosts[i]);
	stop_bgm(finishBGM);
	al_stop_timer(win_timer);
	al_destroy_timer(win_timer);
	free(table.enter_field.enter);
	fclose(fptr);
	for(int i = 0 ; i < table.record_nums ; i++){
		free(table.record[i]->name);
		free(table.record[i]);
	}

}
static void on_mouse_move(){
	if(!win)return;
	if(!table.open){
		table.button.hovered = pnt_in_rect(mouse_x, mouse_y, table.button.body);
	}else{
		table.close.hovered = pnt_in_rect(mouse_x, mouse_y, table.close.body);
		table.enter_field.hovered = pnt_in_rect(mouse_x, mouse_y, table.enter_field.body);
		table.enter_field.finish_enter.hovered = pnt_in_rect(mouse_x, mouse_y, table.enter_field.finish_enter.body);
	}
}
static void on_key_down(int key_code) {
	if(!win){
		if( key_state[ALLEGRO_KEY_LCTRL]|| key_state[ALLEGRO_KEY_RCTRL] ){
			if(key_code == ALLEGRO_KEY_S){
				ghost_freeze = !ghost_freeze;
				game_log(ghost_freeze? "freeze_mode on" : "freeze_mode off");
			}else if(key_code == ALLEGRO_KEY_L){
				pacman_pass_wall = ! pacman_pass_wall;
				game_log(pacman_pass_wall ? "pacman_pass_wall on" : "pacman_pass_wall off");
			}
		}
		if(key_code == keyup){
			pacman_NextMove(pman, UP);
		}else if(key_code == keyleft){
			pacman_NextMove(pman, LEFT);
		}else if(key_code == keydown){
			pacman_NextMove(pman, DOWN);
		}else if(key_code == keyright){
			pacman_NextMove(pman, RIGHT);
		}else
		switch (key_code)
		{
			// TODO-HACKATHON 1-1: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
			// we provided you a function `pacman_NextMove` to set the pacman's next move direction.
			case ALLEGRO_KEY_UP:
				//pacman_NextMove(pman, UP);
				break;
			case ALLEGRO_KEY_A:
				//pacman_NextMove(pman, LEFT);
				break;
			case ALLEGRO_KEY_S:
				//pacman_NextMove(pman, DOWN);
				break;
			case ALLEGRO_KEY_D:
				//pacman_NextMove(pman, RIGHT);
				break;
			case ALLEGRO_KEY_C:
				cheat_mode = !cheat_mode;
				if (cheat_mode)
					printf("cheat mode on\n");
				else 
					printf("cheat mode off\n");
				break;
			case ALLEGRO_KEY_G:
				debug_mode = !debug_mode;
				break;
			case ALLEGRO_KEY_K:
				for(int i = 0 ; i < GHOST_NUM ; i++){
					ghosts[i]->speed = 4;
					ghosts[i]->status = GO_IN;
				}
				break;
			
		default:
			break;
		}
	}else{
		if(!table.open){
			switch (key_code) {
				case ALLEGRO_KEY_ENTER:
					game_change_scene(scene_menu_create());
					break;
				default:
					break;
			}
		}else if(table.enter_field.clicked && !table.enter_field.finish){
			int words_count = strlen(table.enter_field.enter);
			if (key_code == ALLEGRO_KEY_ENTER) {
				int i;
				//game_log("nums+1: %d",nums+1);
				table.record[table.record_nums] = (Tuple*)malloc(sizeof(Tuple));
				table.record[table.record_nums]->name = (char*)malloc(sizeof(char)*21);
				for(i = table.record_nums-1 ;i >= 0 ; i--){
					game_log("i : %d",i);
					if(al_get_timer_count(win_timer) >= table.record[i]->second) break;
				}
				table.enter_field.idx = i+1;
				if(i == table.record_nums-1){
					strcpy(table.record[table.record_nums]->name, table.enter_field.enter);
					table.record[table.record_nums]->second = al_get_timer_count(win_timer);
					table.record_nums++;
					//table.enter_field.idx = table.record_nums;
				}else{
					for(int j = table.record_nums-1; j > i ; j--){
						game_log("j:%d",j);
						char tmp[25];
						strcpy(tmp, table.record[j]->name);
						game_log("j+1 : %d",j+1);
						stpcpy(table.record[j+1]->name, tmp);
						table.record[j+1]->second = table.record[j]->second;
						game_log("%s %d",table.record[j+1]->name, table.record[j+1]->second);
					}
					game_log("i: %d",i);
					strcpy(table.record[i+1]->name, table.enter_field.enter);
					table.record[i+1]->second = al_get_timer_count(win_timer);
					table.record_nums++;
				}
				if(table.record_nums == 6) fprintf(fptr,"5");
				else fprintf(fptr, "%d", table.record_nums);
				fprintf(fptr, "\n");
				for(int i = 0 ; i < 5 && i < table.record_nums ; i++){
					fprintf(fptr, "%s", table.record[i]->name);
					game_log("fout record[%d]:%s %d",i,table.record[i]->name, table.record[i]->second);
					fprintf(fptr," ");
					fprintf(fptr, "%d", table.record[i]->second);
					fprintf(fptr, "\n");
				}
				table.enter_field.finish = 1;
        	} else if (key_code == ALLEGRO_KEY_BACKSPACE && words_count > 0) {
            	table.enter_field.enter[words_count - 1] = '\0';
        	}else{ 
				if(words_count == 0 && key_code == ALLEGRO_KEY_BACKSPACE) return;
				const char* tmp = al_keycode_to_name(key_code);
				table.enter_field.enter[words_count] = *tmp;
				table.enter_field.enter[words_count+1] = '\0';
			}
		}
	}

}

static void on_mouse_down(int btn, int x, int y, int dz) {
	if(!win) return;
	if(!table.open && table.button.hovered){
		table.open = 1;
	}else{
		if(table.close.hovered){
			table.open = 0;
		}else if(table.enter_field.hovered){
			table.enter_field.clicked = 1;
		}else if(table.enter_field.finish_enter.hovered){
			//
		}
	}

}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_flip_display();
	al_rest(2.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	scene.on_mouse_move = &on_mouse_move;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
int64_t get_power_up_timer_tick(){
	return al_get_timer_count(power_up_timer);
}
int64_t get_power_up_duration(){
	return power_up_duration;
}
