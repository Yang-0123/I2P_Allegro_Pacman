#include <allegro5/allegro_primitives.h>
#include "utility.h"
#include "game.h"
#include "shared.h"
#include "scene_menu.h"
#include <stdio.h>
#include <string.h>
#include <regex.h>

extern ALLEGRO_TIMER* game_tick_timer;
//interanl
static ALLEGRO_BITMAP* gameTitle = NULL;
static int gameTitleW ;
static int gameTitleH ;
//
typedef struct {
    RecArea body;
    char* content;
    bool hovered;
    bool clicked;
}Enter_field;
typedef struct {
    RecArea body;
    bool hovered;
}Click_button;

typedef struct{
    Enter_field username;
    Enter_field pwd;
    Click_button enter;
    Click_button enroll;
    bool wrong;
    bool enroll_open;
}Login;
Login login;
FILE* fptr;
typedef struct{
    RecArea body;
    Enter_field username;
    Enter_field pwd;
    Click_button enter;
}Enroll;
Enroll enroll;

bool retry = 0;
static void init_enroll(){
    enroll.body.h = 400;
    enroll.body.w = 400;
    enroll.body.x = 200;
    enroll.body.y = 200;
    float height = 36;
    float width = 250;

    enroll.username.body.h = height;
    enroll.username.body.w = width;
    enroll.username.body.x = enroll.body.x + (enroll.body.w-width) / 2;
    enroll.username.body.y = enroll.body.y + enroll.body.h / 2 - 50;
    enroll.username.clicked = false;
    enroll.username.content = (char*)malloc(sizeof(char)*21); 
    enroll.username.content[0] = '\0';

    enroll.pwd.body.w = width;
    enroll.pwd.body.h = height;
    enroll.pwd.body.x = enroll.body.x + (enroll.body.w-width) / 2 ;     
    enroll.pwd.body.y = enroll.body.y + (enroll.body.h-height) / 2 + 50; 
    enroll.pwd.clicked = false;
    enroll.pwd.content = (char*)malloc(sizeof(char)*21);
    enroll.pwd.content[0] = '\0';

    enroll.enter.body.h = height;
    enroll.enter.body.w = 50;
    enroll.enter.body.x = enroll.body.x + enroll.body.w - 100;   
    enroll.enter.body.y = enroll.body.y + enroll.body.h - 80;
}
static void init(){
    gameTitle = load_bitmap("Assets/title.png");
	gameTitleW = al_get_bitmap_width(gameTitle);
	gameTitleH = al_get_bitmap_height(gameTitle);
    float width = 400;
    float height = 36;

    login.username.body.h = height;
    login.username.body.w = width;
    login.username.body.x = (SCREEN_W-width) / 2;
    login.username.body.y = SCREEN_H / 2 - 50;
    login.username.clicked = false;
    login.username.content = (char*)malloc(sizeof(char)*21); 
    login.username.content[0] = '\0';

    login.pwd.body.w = width;
    login.pwd.body.h = height;
    login.pwd.body.x = (SCREEN_W-width) / 2 ;     
    login.pwd.body.y = SCREEN_H / 2 + 50; 
    login.pwd.clicked = false;
    login.pwd.content = (char*)malloc(sizeof(char)*21);
    login.pwd.content[0] = '\0';

    login.enter.body.h = height;
    login.enter.body.w = 50;
    login.enter.body.x = SCREEN_W / 2 - 100;   
    login.enter.body.y = SCREEN_H / 2 + 150;   

    login.enroll.body.h = height;
    login.enroll.body.w = 125;
    login.enroll.body.x = SCREEN_W / 2 + 50;     
    login.enroll.body.y = SCREEN_H / 2 + 150;  

    fptr = fopen("Assets/login_record.txt","r");
    if(!fptr){
        game_abort("Can't open the login file");
    }
    login.wrong = false;
    login.enroll_open = false;
}
static void draw_enroll(){
    al_draw_filled_rectangle(enroll.body.x, enroll.body.y, enroll.body.x+enroll.body.w, 
                            enroll.body.y + enroll.body.h, al_map_rgb(0xC1,0xBE,0xBF));
    al_draw_text(menuFont,al_map_rgb(0,0,0),enroll.body.x+30, enroll.body.y+30,0,"ENROLL");
    al_draw_text(
        font_pirulen_32, 
        al_map_rgb(0, 0, 0),
        enroll.username.body.x, enroll.username.body.y-40, 
        ALLEGRO_ALIGN_LEFT,
        "Username:"
    );
    al_draw_rectangle(
        enroll.username.body.x, enroll.username.body.y,
        enroll.username.body.x + enroll.username.body.w, enroll.username.body.y + enroll.username.body.h,
        al_map_rgb(0, 0, 0), 2
    );
    char tmp[21];
	sprintf(tmp,"%s",enroll.username.content);
	const char *chr = tmp;
	float offset = al_get_text_width(font_pirulen_32,tmp);
    al_draw_textf(font_pirulen_32,al_map_rgb(0,0,0), 5+enroll.username.body.x, 
                enroll.username.body.y-3, ALLEGRO_ALIGN_LEFT, "%s",chr);
    if(enroll.username.clicked){
        if(((al_get_timer_count(game_tick_timer)>>6) & 1) == 0){
            al_draw_line(5+enroll.username.body.x+offset, enroll.username.body.y, 
                        5+enroll.username.body.x+offset, enroll.username.body.y + enroll.username.body.h,
                        al_map_rgb(0,0,0),2);
        }
    }
    // pwd
    al_draw_text(
        font_pirulen_32,
        al_map_rgb(0, 0, 0),
        enroll.pwd.body.x, enroll.pwd.body.y -40,
        ALLEGRO_ALIGN_LEFT,
        " Password:"
    );
    al_draw_rectangle(
        enroll.pwd.body.x, enroll.pwd.body.y,
        enroll.pwd.body.x + enroll.pwd.body.w, enroll.pwd.body.y + enroll.pwd.body.h,
        al_map_rgb(0, 0, 0), 2
    );
	sprintf(tmp,"%s",enroll.pwd.content);
	const char *chr2 = tmp;
	offset = al_get_text_width(font_pirulen_32,tmp);
    al_draw_textf(font_pirulen_32,al_map_rgb(0,0,0), 5+enroll.pwd.body.x, 
                enroll.pwd.body.y-3, ALLEGRO_ALIGN_LEFT, "%s",chr2);
    if(enroll.pwd.clicked){
        if(((al_get_timer_count(game_tick_timer)>>6) & 1) == 0){
            al_draw_line(5+enroll.pwd.body.x+offset, enroll.pwd.body.y, 
                        5+enroll.pwd.body.x+offset, enroll.pwd.body.y + enroll.pwd.body.h,
                        al_map_rgb(0,0,0),2);
        }
    }
    // enter
    if(!enroll.enter.hovered){
        al_draw_text(font_pirulen_32,al_map_rgb(0,0,0), 
                    enroll.enter.body.x,enroll.enter.body.y-4, ALLEGRO_ALIGN_LEFT,"OK");
        al_draw_rounded_rectangle(
            enroll.enter.body.x, enroll.enter.body.y,
            enroll.enter.body.x + enroll.enter.body.w, enroll.enter.body.y + enroll.enter.body.h,
            10,10,al_map_rgb(0, 0, 0), 2
        );
    }else{
        al_draw_filled_rounded_rectangle(
            enroll.enter.body.x, enroll.enter.body.y,
            enroll.enter.body.x + enroll.enter.body.w, enroll.enter.body.y + enroll.enter.body.h,
            10,10,al_map_rgb(0, 0, 0)
        );
        al_draw_text(font_pirulen_32,al_map_rgb(255,255,255), 
                    enroll.enter.body.x,enroll.enter.body.y-4, ALLEGRO_ALIGN_LEFT,"OK");
    }
    if(retry){
        al_draw_text(font_pirulen_24, al_map_rgb(255,0,0), 
                    enroll.body.x+10, enroll.pwd.body.y+40, 
                    0, "pwd must contain at least one");
        al_draw_text(font_pirulen_24, al_map_rgb(255,0,0),
                    enroll.body.x+10, enroll.pwd.body.y+64,
                    0, "upper letter and digit.");    
    }
}
static void draw(){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    const float scale = 0.7;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameTitleW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameTitleH;
    al_draw_scaled_bitmap(
		gameTitle,
		0, 0,
		gameTitleW, gameTitleH,
		offset_w, offset_h -250,
		gameTitleW * scale, gameTitleH * scale,
		0
	);
    //users
    al_draw_text(
        font_pirulen_32, 
        al_map_rgb(255, 255, 255),
        login.username.body.x - 165, login.username.body.y, 
        ALLEGRO_ALIGN_LEFT,
        "Username:"
    );
    al_draw_rectangle(
        login.username.body.x, login.username.body.y,
        login.username.body.x + login.username.body.w, login.username.body.y + login.username.body.h,
        al_map_rgb(255, 255, 255), 2
    );
    char tmp[21];
	sprintf(tmp,"%s",login.username.content);
	const char *chr = tmp;
	float offset = al_get_text_width(font_pirulen_32,tmp);
    al_draw_textf(font_pirulen_32,al_map_rgb(255,255,255), 5+login.username.body.x, 
                login.username.body.y-3, ALLEGRO_ALIGN_LEFT, "%s",chr);
    if(login.username.clicked){
        if(((al_get_timer_count(game_tick_timer)>>6) & 1) == 0){
            al_draw_line(5+login.username.body.x+offset, login.username.body.y, 
                        5+login.username.body.x+offset, login.username.body.y + login.username.body.h,
                        al_map_rgb(255,255,255),0);
        }
    }
    // pwd
    al_draw_text(
        font_pirulen_32,
        al_map_rgb(255, 255, 255),
        login.pwd.body.x - 165, login.pwd.body.y ,
        ALLEGRO_ALIGN_LEFT,
        " Password:"
    );
    al_draw_rectangle(
        login.pwd.body.x, login.pwd.body.y,
        login.pwd.body.x + login.pwd.body.w, login.pwd.body.y + login.pwd.body.h,
        al_map_rgb(255, 255, 255), 2
    );
	sprintf(tmp,"%s",login.pwd.content);
	const char *chr2 = tmp;
	offset = al_get_text_width(font_pirulen_32,tmp);
    /*
    al_draw_textf(font_pirulen_32,al_map_rgb(255,255,255), 5+login.pwd.body.x, 
                login.pwd.body.y-3, ALLEGRO_ALIGN_LEFT, "%s",chr2);
    */
    int len = strlen(chr2); 
    float asteriskWidth = offset / len;
    for(int i = 0 ; i < len ; i++){
        al_draw_textf(font_pirulen_32,al_map_rgb(255,255,255), 5+login.pwd.body.x+i*asteriskWidth, 
                login.pwd.body.y-3, ALLEGRO_ALIGN_LEFT, "%s","*");
    }
    if(login.pwd.clicked){
        if(((al_get_timer_count(game_tick_timer)>>6) & 1) == 0){
            al_draw_line(5+login.pwd.body.x+offset, login.pwd.body.y, 
                        5+login.pwd.body.x+offset, login.pwd.body.y + login.pwd.body.h,
                        al_map_rgb(255,255,255),0);
        }
    }
    if(login.wrong){
        al_draw_textf(font_pirulen_32, al_map_rgb(255,0,0), 
                    login.username.body.x, login.pwd.body.y+login.pwd.body.h+15, 
                    0,"wrong username or password");
    }
    // enter
    if(!login.enter.hovered){
        al_draw_filled_rounded_rectangle(
            login.enter.body.x, login.enter.body.y,
            login.enter.body.x + login.enter.body.w, login.enter.body.y + login.enter.body.h,
            10,10,al_map_rgb(0x63, 0xC5, 0xDA));
        al_draw_text(font_pirulen_32,al_map_rgb(255,255,255), 
                    login.enter.body.x,login.enter.body.y-4, ALLEGRO_ALIGN_LEFT,"GO");
    }else{
        al_draw_filled_rounded_rectangle(
            login.enter.body.x, login.enter.body.y,
            login.enter.body.x + login.enter.body.w, login.enter.body.y + login.enter.body.h,
            10,10,al_map_rgb(255,255,255));
        al_draw_text(font_pirulen_32,al_map_rgb(0x63, 0xC5, 0xDA), 
                    login.enter.body.x,login.enter.body.y-4, ALLEGRO_ALIGN_LEFT,"GO");
    }
    // enroll
    if(!login.enroll.hovered){
        al_draw_filled_rounded_rectangle(
            login.enroll.body.x, login.enroll.body.y,
            login.enroll.body.x + login.enroll.body.w, login.enroll.body.y + login.enroll.body.h,
            10,10,al_map_rgb(0x63, 0xC5, 0xDA)
        );
        al_draw_text(font_pirulen_32,al_map_rgb(255,255,255), 
                    login.enroll.body.x,login.enroll.body.y-4, ALLEGRO_ALIGN_LEFT,"ENROLL");
    }else{
        al_draw_filled_rounded_rectangle(
            login.enroll.body.x, login.enroll.body.y,
            login.enroll.body.x + login.enroll.body.w, login.enroll.body.y + login.enroll.body.h,
            10,10,al_map_rgb(255,255,255)
        );
        al_draw_text(font_pirulen_32,al_map_rgb(0x63, 0xC5, 0xDA), 
                    login.enroll.body.x,login.enroll.body.y-4, ALLEGRO_ALIGN_LEFT,"ENROLL");
    }
    if(login.enroll_open){
        draw_enroll();
    }
}
static void on_mouse_move(){
    if(!login.enroll_open){
        login.enroll.hovered = pnt_in_rect(mouse_x, mouse_y, login.enroll.body);
        login.enter.hovered = pnt_in_rect(mouse_x, mouse_y, login.enter.body);
        login.pwd.hovered = pnt_in_rect(mouse_x, mouse_y, login.pwd.body);
        login.username.hovered = pnt_in_rect(mouse_x, mouse_y, login.username.body);
    }else{
        enroll.enter.hovered = pnt_in_rect(mouse_x, mouse_y, enroll.enter.body);
        enroll.pwd.hovered = pnt_in_rect(mouse_x, mouse_y, enroll.pwd.body);
        enroll.username.hovered = pnt_in_rect(mouse_x, mouse_y, enroll.username.body);
    }
}
static void on_mouse_down(){
    if(login.enroll_open){
        game_log("enroll open");
        if(enroll.username.hovered){
            enroll.username.clicked = 1;
            enroll.pwd.clicked = 0;
        }else if(enroll.pwd.hovered){
            enroll.pwd.clicked = 1;
            enroll.username.clicked = 0;
        }else if(enroll.enter.hovered){
            game_log("enroll close");
            fclose(fptr);
            fptr = fopen("Assets/login_record.txt","a");
            const char* number = ".*[0-9].*";
            const char* letter = ".*[A-Za-z].*";
            regex_t regex;
            regex_t regex2;
            regcomp(&regex, number, REG_EXTENDED);
            regcomp(&regex2, letter, REG_EXTENDED);
            //const char* tmp = enroll.pwd.content;
            const char*str = enroll.pwd.content;
            int flag = regexec(&regex, str, 0,NULL, 0);
            int flag2 = regexec(&regex2, str, 0, NULL, 0);
            if(strlen(enroll.username.content) != 0 && strlen(enroll.pwd.content)!=0 && !flag && !flag2){
                game_log("success enroll");
                fprintf(fptr,"%s %s\n",enroll.username.content, enroll.pwd.content);
                retry = 0;
            }else{
                retry = 1;
            }
            regfree(&regex);
            regfree(&regex2);
            if(retry == 0){
                login.enroll_open = 0;
            }
            fclose(fptr);
            fptr = fopen("Assets/login_record.txt","r");
        }else{
            enroll.username.clicked = 0;
            enroll.pwd.clicked = 0;
        }
    }else
    if(login.enter.hovered){
        //game_change_scene(scene_menu_create());
        char usr[21] = {'\0'};
        char pwd[21] = {'\0'};
        while(~fscanf(fptr,"%s %s",usr,pwd)){
            game_log("%s %s",usr, pwd);
            if(!strcmp(usr,login.username.content) && !strcmp(pwd, login.pwd.content)){
                game_change_scene(scene_menu_create());
                return;
            }
        }
        login.wrong = 1;
        fseek(fptr, 0, SEEK_SET);
    }else if(login.username.hovered){
        login.username.clicked = true;
        login.pwd.clicked = false;
    }else if(login.pwd.hovered){
        login.pwd.clicked = true;
        login.username.clicked = false;
    }else if(login.enroll.hovered){
        login.enroll_open = 1;
        init_enroll();
    }else{
        login.username.clicked = 0;
        login.pwd.clicked = 0;
    }
}
static void on_key_down(int key_code){
    if(!login.enroll_open){
        if(login.username.clicked){
            int len = strlen(login.username.content);
            if(key_code == ALLEGRO_KEY_BACKSPACE && len > 0){
                login.username.content[len - 1] = '\0';
            }else{
                if(key_code == ALLEGRO_KEY_BACKSPACE || key_code == ALLEGRO_KEY_ENTER) return;
                const char *tmp = al_keycode_to_name(key_code);
                login.username.content[len] = *tmp;
                login.username.content[len+1] = '\0';
            }
        }else if(login.pwd.clicked){
            int len = strlen(login.pwd.content);
            if(key_code == ALLEGRO_KEY_BACKSPACE && len > 0){
                login.pwd.content[len - 1] = '\0';
            }else{
                if(key_code == ALLEGRO_KEY_BACKSPACE || key_code == ALLEGRO_KEY_ENTER) return;
                const char *tmp = al_keycode_to_name(key_code);
                login.pwd.content[len] = *tmp;
                login.pwd.content[len+1] = '\0';
            }
        }
    }else{
        if(enroll.username.clicked){
            int len = strlen(enroll.username.content);
            if(key_code == ALLEGRO_KEY_BACKSPACE && len > 0){
                enroll.username.content[len - 1] = '\0';
            }else{
                if(key_code == ALLEGRO_KEY_BACKSPACE || key_code == ALLEGRO_KEY_ENTER) return;
                const char *tmp = al_keycode_to_name(key_code);
                enroll.username.content[len] = *tmp;
                enroll.username.content[len+1] = '\0';
            }
        }else if(enroll.pwd.clicked){
            int len = strlen(enroll.pwd.content);
            if(key_code == ALLEGRO_KEY_BACKSPACE && len > 0){
                enroll.pwd.content[len - 1] = '\0';
            }else{
                if(key_code == ALLEGRO_KEY_BACKSPACE || key_code == ALLEGRO_KEY_ENTER) return;
                const char *tmp = al_keycode_to_name(key_code);
                enroll.pwd.content[len] = *tmp;
                enroll.pwd.content[len+1] = '\0';
            }
        }
    }
}
static void destroy(){
    free(login.username.content);
    free(login.pwd.content);
    fclose(fptr);
}
Scene scene_login_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Login";
    scene.initialize = &init;
	scene.draw = &draw;
	scene.on_key_down = &on_key_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	//my
	scene.on_mouse_down = &on_mouse_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.destroy = &destroy;
	game_log("Settings scene created");
	return scene;
}