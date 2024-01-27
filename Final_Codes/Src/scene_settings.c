#include <allegro5/allegro_primitives.h>
#include "utility.h"
// TODO-HACKATHON 3-9: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
#include "scene_settings.h"
extern bool cheat_mode;
// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO-IF: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.
typedef struct {
    RecArea body;
    bool hovered;
	bool clicked;
} KeyButton;
KeyButton keybuttons[4];

typedef struct {
    const char* filename;
    const char* display_name;
	RecArea body;
	bool hovered;
} MusicOption;

static ALLEGRO_SAMPLE_ID BGM;
float tmpvolume;

MusicOption musicOptions[] = {
    {"Assets/Music/pacman_bgm1.ogg", "Siren(default)", 0,0},
	{"Assets/Music/pacman_bgm2.ogg", "remix version", 0,0}
};

typedef struct {
	int optionHeight;
	RecArea body;
	bool hovered;
	bool open;
} DropDown;
DropDown dropdown;

typedef struct {
	float max;
	float min;
	float* value;
	RecArea body;
	RecArea valuebody;
	bool grabbed;
	bool hovered;
	bool slider_hovered;
}Slider;
Slider slider;

typedef struct {
	const char* name;
	bool hovered;
    bool checked;
	func_ptr methodcheck;
	func_ptr methoduncheck;
    RecArea body;
} Checkbox;
static void mutevolume(){
	stop_bgm(BGM);
	tmpvolume = music_volume;
	music_volume = 0;
}
static void unmutevolume(){
	stop_bgm(BGM);
	slider.valuebody.x = slider.body.x + (slider.body.w/100)*(music_volume)*10;
	music_volume = tmpvolume;
	BGM = play_bgm(bgmMusic, music_volume);
}
/*
static void cheatmode(){
	
}
*/
static void setcheatmode(){
	cheat_mode = 1;
}
static void unsetcheatmode(){
	cheat_mode = 0;
}
// cheat mode function link checkbox 
Checkbox checkboxs[] = {{"Mute",0,0, &mutevolume, &unmutevolume, 0},{"Cheat Mode",0,0,&setcheatmode,&unsetcheatmode,0}};
static void draw_dropdown(DropDown dropdown){
	float text_height = al_get_font_line_height(font_pirulen_32);
	al_draw_rectangle(dropdown.body.x, dropdown.body.y, dropdown.body.x + dropdown.body.w
		, dropdown.body.y + dropdown.body.h, al_map_rgb(128, 128, 128), 2);
	al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), dropdown.body.x + 10, dropdown.body.y + (dropdown.body.h - text_height) / 2 , 0, "Select BGM");
	if(!dropdown.open){
        float triangle_x = dropdown.body.x + dropdown.body.w - 20;
        float triangle_y = dropdown.body.y + (dropdown.body.h - 10) / 2;
        
        al_draw_filled_triangle(triangle_x, triangle_y,
                                triangle_x + 10, triangle_y,
                                triangle_x + 5, triangle_y + 10,
                                al_map_rgb(255, 255, 255));
	}
	if (dropdown.open) {
        for (int i = 0; i < sizeof(musicOptions) / sizeof(MusicOption); ++i) {
			
			al_draw_filled_rectangle(dropdown.body.x+5,
                                     dropdown.body.y + dropdown.body.h + i * dropdown.optionHeight,
                                     dropdown.body.x + dropdown.body.w - 5,
                                     dropdown.body.y + dropdown.body.h  +32 + i * dropdown.optionHeight,
                                     al_map_rgb(255, 255, 255));
			
            al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0),
                         dropdown.body.x + 10,
                         dropdown.body.y + dropdown.body.h + i * dropdown.optionHeight+(dropdown.optionHeight-text_height)/2,
                         0, musicOptions[i].display_name);
			
        }
    }
}
static void draw_slider(Slider slider){
	if(slider.valuebody.x == slider.body.x) checkboxs[0].checked = 1;
	al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255),slider.body.x,slider.body.y-50,0,"BGM Volume");
	al_draw_filled_rectangle(slider.body.x, slider.body.y,
                             slider.body.x + slider.body.w, slider.body.y + slider.body.h-5,
                             al_map_rgb(255, 255, 255));
	al_draw_filled_rectangle(slider.valuebody.x, slider.valuebody.y,
                             slider.valuebody.x+slider.valuebody.w, slider.body.y + slider.valuebody.h,
                             al_map_rgb(0, 0, 255));
}
static void draw_checkbox(Checkbox checkbox){
	al_draw_filled_rectangle(checkbox.body.x, checkbox.body.y,
                             checkbox.body.x + checkbox.body.w, checkbox.body.y + checkbox.body.h,
                             al_map_rgb(128, 128, 128));
	
	al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), checkbox.body.x+checkbox.body.w+20, checkbox.body.y+(checkbox.body.h - 32)/2, 0, checkbox.name);
    // Draw check mark if the checkbox is checked
    if (checkbox.checked) {
        al_draw_line(checkbox.body.x, checkbox.body.y,
                     checkbox.body.x + checkbox.body.w, checkbox.body.y + checkbox.body.h,
                     al_map_rgb(255, 255, 255), 2.0);
        al_draw_line(checkbox.body.x + checkbox.body.w, checkbox.body.y,
                     checkbox.body.x, checkbox.body.y + checkbox.body.h,
                     al_map_rgb(255, 255, 255), 2.0);
    }
}
static void draw_keybutton(){
	for (int i = 0; i < 4; i++) {
        al_draw_filled_rectangle(keybuttons[i].body.x, keybuttons[i].body.y,
                                 keybuttons[i].body.x + keybuttons[i].body.w,
                                 keybuttons[i].body.y + keybuttons[i].body.h,
                                 (keybuttons[i].hovered || keybuttons[i].clicked)? al_map_rgb(128, 128, 128) : al_map_rgb(0, 0, 255));
		int keycode;
		if(i == 0) keycode = keyup;
		else if(i == 1) keycode = keyleft;
		else if(i == 2)keycode = keydown;
		else keycode = keyright;
		const char* key_name = al_keycode_to_name(keycode);

        float text_width = al_get_text_width(font_pirulen_32, key_name);
        float text_height = al_get_font_line_height(font_pirulen_32);

        float text_x = keybuttons[i].body.x + (keybuttons[i].body.w - text_width) / 2;
        float text_y = keybuttons[i].body.y + (keybuttons[i].body.h - text_height) / 2;

		int arrow_size = 30;
        float arrow_x = keybuttons[i].body.x + (keybuttons[i].body.w - arrow_size) / 2; // Center horizontally
        float arrow_y = keybuttons[i].body.y - arrow_size - arrow_size / 4; // Above the button, slightly higher

        switch (i) {
            case 0:
                al_draw_filled_triangle(arrow_x, arrow_y + arrow_size,
                                        arrow_x + arrow_size, arrow_y + arrow_size,
                                        arrow_x + arrow_size/2, arrow_y + arrow_size / 2, al_map_rgb(255, 255, 255));
                break;
            case 1:
                al_draw_filled_triangle(arrow_x + arrow_size / 2+6, arrow_y,
                                        arrow_x + arrow_size / 2+6, arrow_y + arrow_size,
                                        arrow_x+6, arrow_y + arrow_size / 2, al_map_rgb(255, 255, 255));
                break;
            case 2:
                al_draw_filled_triangle(arrow_x, arrow_y + arrow_size / 2,
                                        arrow_x + arrow_size, arrow_y + arrow_size / 2,
                                        arrow_x + arrow_size / 2, arrow_y + arrow_size, al_map_rgb(255, 255, 255));
                break;
            case 3:
                al_draw_filled_triangle(arrow_x+6, arrow_y,
                                        arrow_x+6, arrow_y + arrow_size,
                                        arrow_x+6 + arrow_size / 2, arrow_y + arrow_size / 2, al_map_rgb(255, 255, 255));
                break;
            default:
                break;
        }
		al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), text_x, text_y, 0, key_name);
    }
}
static void draw(void ){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"<ENTER> Back to menu"
	);
	draw_dropdown(dropdown);
	draw_slider(slider);
	for(int i = 0 ; i < sizeof(checkboxs)/sizeof(Checkbox) ; i++){
		draw_checkbox(checkboxs[i]);
	}
	draw_keybutton();
}
static void on_key_down(int keycode) {
	for(int i = 0 ; i < 4 ; i++){
		if(keybuttons[i].clicked){
			switch(i){
				case 0:
					keyup = keycode;
					break;
				case 1:
					keyleft = keycode;
					break;
				case 2:
					keydown = keycode;
					break;
				case 3:
					keyright = keycode;
				default:
					break;
			}
			keybuttons[i].clicked = 0;
		}
	}
	//stop_bgm(BGM);
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			game_change_scene(scene_menu_create());
			break;
		default:
			break;
	}
}
static void init_dropdown(DropDown* dropdown){
	dropdown->body.y = 50;
	dropdown->body.h = 32;
	dropdown->body.w = 220;
	dropdown->body.x = (SCREEN_W - dropdown->body.w)/2;
	dropdown->optionHeight = 32;
	for (int i = 0; i < sizeof(musicOptions) / sizeof(MusicOption); ++i) {
		musicOptions[i].body.x = dropdown->body.x + 5;
		musicOptions[i].body.y = dropdown->body.y + dropdown->body.h + i * dropdown->optionHeight ;
		musicOptions[i].body.h = 32;
		musicOptions[i].body.w = dropdown->body.w - 10;
	}
	
}
static void init_slider(Slider* slider){
	slider->grabbed = 0;
	slider->max = 100;
	slider->min = 0;
	slider->value = &music_volume;
	slider->body.x = 200;
	slider->body.y = 400;
	slider->body.w = 400;
	slider->body.h = 10;
	slider->valuebody.y = slider->body.y-7.5;
	slider->valuebody.h = 20;
	slider->valuebody.w = 20;
	slider->valuebody.x = slider->body.x + (slider->body.w/100)*(*slider->value)*10;
}
static void init_checkbox(Checkbox* checkbox, bool initialState, int idx) {
    checkbox->checked = initialState;
    checkbox->body.x = 200;
    checkbox->body.w = 40;
    checkbox->body.h = 40;
	checkbox->body.y = 500+(20+checkbox->body.h)*idx;
}
static void init_keybutton(void ){
	int buttonSize = 50;
    for (int i = 0; i < 4; i++) {
        keybuttons[i].body.x = 200 + i * (buttonSize + 50);
        keybuttons[i].body.y = 250;
        keybuttons[i].body.w = buttonSize;
        keybuttons[i].body.h = buttonSize;
        keybuttons[i].hovered = false;
		keybuttons[i].clicked = 0;
    }
}
static void init(){
	init_dropdown(&dropdown);
	BGM = play_bgm(bgmMusic, music_volume);
	init_slider(&slider);
	for(int i = 0 ; i < sizeof(checkboxs)/sizeof(Checkbox) ; i++){
		init_checkbox(&checkboxs[i], 0, i);
	}
	game_log("musicvolume %f",music_volume);
	if(music_volume == 0) checkboxs[0].checked = 1;
	if(cheat_mode) checkboxs[1].checked = 1;
	init_keybutton();

}
//int sampleRecord[5] = {-1,-1,-1,-1,-1};
static void destroy(){
	stop_bgm(BGM);
}
static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	dropdown.hovered = pnt_in_rect(mouse_x, mouse_y, dropdown.body);
	for (int i = 0; i < sizeof(musicOptions) / sizeof(MusicOption); ++i){
		if(dropdown.open){
			musicOptions[i].hovered = pnt_in_rect(mouse_x, mouse_y, musicOptions[i].body);
		}else{
			musicOptions[i].hovered = 0;
		}
	}
	slider.hovered = pnt_in_rect(mouse_x, mouse_y, slider.valuebody);
	slider.slider_hovered = pnt_in_rect(mouse_x, mouse_y, slider.body);
	if(slider.grabbed && slider.slider_hovered){
		//game_log("sliding");
		slider.valuebody.x = mouse_x;
		*slider.value = (slider.valuebody.x-slider.body.x)/10/(slider.body.w/100);
		if(*slider.value != 0) checkboxs[0].checked = 0;
		stop_bgm(BGM);
		BGM = play_bgm(bgmMusic, music_volume);
	}else if(slider.grabbed){
		slider.grabbed = false;
	}
	for(int i = 0 ;i < sizeof(checkboxs)/sizeof(Checkbox) ; i++){
		checkboxs[i].hovered = pnt_in_rect(mouse_x, mouse_y, checkboxs[i].body);
	}
	for(int i = 0 ; i < 4 ; i++){
		keybuttons[i].hovered = pnt_in_rect(mouse_x, mouse_y, keybuttons[i].body);
	}
}
static void on_mouse_down() {
	if (dropdown.hovered) dropdown.open ? (dropdown.open = 0) : (dropdown.open = 1);
	if(slider.hovered){
		slider.grabbed = true;
	}else{
		slider.grabbed = false;
	}
	for (int i = 0; i < sizeof(musicOptions) / sizeof(MusicOption); ++i){
		if(musicOptions[i].hovered){
			stop_bgm(BGM);
			//sampleRecord[i] = i;
			bgmMusic = al_load_sample(musicOptions[i].filename);
			BGM = play_bgm(bgmMusic, music_volume);
			//musicOptions[i].hovered = false;
		}
	}
	for(int i = 0 ; i < sizeof(checkboxs)/sizeof(Checkbox) ; i++){
		if(checkboxs[i].hovered){
			game_log("%d before",checkboxs[i].checked);
			checkboxs[i].checked = checkboxs[i].checked ? 0 : 1;
			game_log("%d after",checkboxs[i].checked);
			if(checkboxs[i].checked) checkboxs[i].methodcheck();
			else{
				if(i == 0) music_volume = 0.5;
				checkboxs[i].methoduncheck();
				//draw();
			}
		}
	}
	for(int i = 0 ; i < 4 ; i++){
		if(keybuttons[i].hovered){
			keybuttons[i].clicked = 1;
		}
	}
}

// The only function that is shared across files.
Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.draw = &draw;
	scene.on_key_down = &on_key_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	//my
	scene.initialize = &init;
	dropdown.open = 0;
	dropdown.hovered = 0;
	scene.on_mouse_down = &on_mouse_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.destroy = &destroy;
	//scene.on_mouse_up;
	//scene.on_mouse_scroll;
	game_log("Settings scene created");
	return scene;
}

// TODO-Graphical_Widget:
// Just suggestions, you can create your own usage.
	// Selecting BGM:
	// 1. Declare global variables for storing the BGM. (see `shared.h`, `shared.c`)
	// 2. Load the BGM in `shared.c`.
	// 3. Create dropdown menu for selecting BGM in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and play the BGM if the corresponding option is selected.

	// Adjusting Volume:
	// 1. Declare global variables for storing the volume. (see `shared.h`, `shared.c`)
	// 2. Create a slider for adjusting volume in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. You may use checkbox to switch between mute and unmute.
	// 3. Adjust the volume and play when the button is pressed.

	// Selecting Map:
	// 1. Preload the map to `shared.c`.
	// 2. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. For player experience, you may also create another scene between menu scene and game scene for selecting map.
	// 3. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and draw the map if the corresponding option is selected.
		// 4.1. Suggestions: You may use `al_draw_bitmap` to draw the map for previewing. 
							// But the map is too large to be drawn in original size. 
							// You might want to modify the function to allow scaling.