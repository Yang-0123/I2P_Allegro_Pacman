// [shared.c]
// you should define the shared variable declared in the header here.

#include "shared.h"
#include "utility.h"
#include "game.h"
// #include "scene_menu.h"

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_SAMPLE* themeMusic = NULL;
ALLEGRO_SAMPLE* PACMAN_MOVESOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND = NULL;
ALLEGRO_FONT* menuFont = NULL;
//my
ALLEGRO_FONT* font_pirulen_50;
ALLEGRO_SAMPLE* bgmMusic = NULL;
ALLEGRO_SAMPLE* finishMusic = NULL;
int fontSize = 30;
float music_volume = 0.5;
float effect_volume = 0.5;
bool gameDone = false;
int keyup = ALLEGRO_KEY_W;
int keyleft = ALLEGRO_KEY_A;
int keydown = ALLEGRO_KEY_S;
int keyright = ALLEGRO_KEY_D;
bool pacman_pass_wall = false;

/*
	A way to accelerate is load assets once.
	And delete them at the end.
	This method does provide you better management of memory.
	
*/
void shared_init(void) {
	
	menuFont = load_font("Assets/Minecraft.ttf", fontSize);
	font_pirulen_32 = load_font("Assets/OpenSans-Regular.ttf", 32);
	themeMusic = load_audio("Assets/Music/original_theme.ogg");
	PACMAN_MOVESOUND = load_audio("Assets/Music/pacman-chomp.ogg");
	PACMAN_DEATH_SOUND = load_audio("Assets/Music/pacman_death.ogg");
	bgmMusic = load_audio("Assets/Music/pacman_bgm1.ogg");
	font_pirulen_50 = load_font("Assets/Minecraft.ttf", 50);
	finishMusic = load_audio("Assets/Music/pacman_intermission.ogg");
	font_pirulen_24 = load_font("Assets/OpenSans-Regular.ttf", 24);
}

void shared_destroy(void) {

	al_destroy_font(menuFont);
	al_destroy_font(font_pirulen_32);
	al_destroy_sample(themeMusic);
	al_destroy_sample(PACMAN_MOVESOUND);
	al_destroy_sample(PACMAN_DEATH_SOUND);
	al_destroy_font(font_pirulen_50);
	al_destroy_sample(finishMusic);
	al_destroy_font(font_pirulen_24);
}