/*** BIBLIOTECAS ***/
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/mouse.h>
#include <allegro5/mouse_cursor.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/*** LARGURA E ALTURA ***/
#define width 800
#define height 600

/*** SCREENSHOT ***/
void screenshot(ALLEGRO_DISPLAY* display){
  ALLEGRO_BITMAP* backbuffer = al_get_backbuffer(display);
  if(backbuffer == NULL) {
    printf("erro: backbuffer");
    return;
  }  
  ALLEGRO_BITMAP* screenshot = al_create_bitmap(width, height);
  al_set_target_bitmap(screenshot);
  al_draw_bitmap(backbuffer, 0, 0, 0);
  al_set_target_backbuffer(display);

  char filename[64];
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  strftime(filename, sizeof(filename), "screenshot_%Y%m%d_%H%M%S.png", timeinfo);

  if (al_save_bitmap(filename, screenshot)) {
        printf("Screenshot salva como %s\n", filename);
    }
  else {
        printf("Erro ao salvar screenshot.\n");
    }
    al_destroy_bitmap(screenshot);
}

/*** INIT ALEGRO ***/
bool init_func(){
  if (!al_init()){
    printf("erro ao iniciar biblioteca\n");
    return false;
  }
  al_install_keyboard();
  al_install_audio();
  al_init_acodec_addon();
  al_reserve_samples(2);
  al_init_image_addon();
  al_init_font_addon();
  al_init_ttf_addon();
  al_install_mouse();  
  return true;
}


/*** MENU PRINCIPAL ***/
int mainmenu(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_EVENT_QUEUE* queue) {
    bool teclas[ALLEGRO_KEY_MAX] = { false };
    ALLEGRO_BITMAP* menu = al_load_bitmap("./assets/mainmenu.png");
    if (!menu) {
        printf("Erro: menu\n");
        exit(EXIT_FAILURE);             
    }

    ALLEGRO_SAMPLE* menu_music = al_load_sample("./sfx/8bitmenu.mp3");
    if (!menu_music) {
        printf("Erro: música menu\n");
    }

    ALLEGRO_SAMPLE_INSTANCE* music_instance = al_create_sample_instance(menu_music);
    al_attach_sample_instance_to_mixer(music_instance, al_get_default_mixer());
    al_set_sample_instance_playmode(music_instance, ALLEGRO_PLAYMODE_LOOP);
    al_play_sample_instance(music_instance);

    bool sair = false;
    int retorno = 0;
    while (!sair) {
        ALLEGRO_EVENT event;
        while (al_get_next_event(queue, &event)) {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                sair = true;
                retorno = 0;
            }
            else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                teclas[event.keyboard.keycode] = true;
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    sair = true;
                    retorno = 0;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    sair = true;
                    retorno = 1;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_P) {
                    screenshot(display);
                }
            }
            else if (event.type == ALLEGRO_EVENT_KEY_UP) {
                teclas[event.keyboard.keycode] = false;
            }
        }

        al_draw_bitmap(menu, 0, 0, 0);
        al_draw_text(font, al_map_rgb(255, 165, 0), width / 2, 250, 
                    ALLEGRO_ALIGN_CENTER, "Pressione enter para iniciar...");
        al_flip_display();
    }

    al_destroy_sample(menu_music);
    al_destroy_bitmap(menu);
    al_destroy_sample_instance(music_instance);
    
} 

/*** MAIN FUNC ***/ 
int main(int argc, char *argv[]){
  int tp = 0;
  double init_time = 0;
  bool on_menu = true;
  float sprite_x = 100;
  float sprite_y = 100;
  float velocidade = 4.0;
  
  if (tp == 0){
    if (init_func() == false){
      printf("erro: iniciar a biblioteca\n");
      return 0;
    }
    tp++;
  }
  
  ALLEGRO_DISPLAY* display = al_create_display(width, height);
  al_set_window_title(display, "highway");
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  al_register_event_source(queue, al_get_display_event_source(display));
  al_register_event_source(queue, al_get_keyboard_event_source());
  ALLEGRO_FONT* font = al_create_builtin_font();
  bool teclas[ALLEGRO_KEY_MAX] = { false };
  mainmenu(display, font, queue);
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_start_timer(timer);
  ALLEGRO_BITMAP* sprite = al_load_bitmap("./assets/chsprt.png");
    if (!sprite) {
        printf("Erro ao carregar sprite personagem!\n");
        exit(EXIT_FAILURE);; 
    }
  ALLEGRO_BITMAP* fundo = al_load_bitmap("./assets/background.png");
  al_clear_to_color(al_map_rgb(0, 0, 0));   
  al_draw_scaled_bitmap(fundo, 0, 0, 320, 200, 0, 0, 640, 480, 0);
  al_flip_display();

while(true){
    ALLEGRO_EVENT event;
    al_wait_for_event(queue, &event);
    if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){ 
      break;
    }
    else if (event.type == ALLEGRO_EVENT_KEY_DOWN) { 
      teclas[event.keyboard.keycode] = true;
    }
    else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
      mainmenu(display, font, queue);
    }
    else if (event.type == ALLEGRO_EVENT_KEY_UP) {
      teclas[event.keyboard.keycode] = false; 
    }
    else if (event.type == ALLEGRO_EVENT_TIMER) {
      if (teclas[ALLEGRO_KEY_UP]) sprite_y -= velocidade;
      if (teclas[ALLEGRO_KEY_LEFT]) sprite_x -= velocidade;
      if (teclas[ALLEGRO_KEY_RIGHT]) sprite_x += velocidade;
      
      if (sprite_x < 0) sprite_x = 0;
      else if (sprite_x > 800 - al_get_bitmap_width(sprite)) sprite_x = 0;
      if (sprite_y < 500 && (teclas[ALLEGRO_KEY_UP] == false)) sprite_y = 500;
      else if (teclas[ALLEGRO_KEY_UP] == true){
        sprite_y += -30;
        velocidade ++;
        teclas[ALLEGRO_KEY_UP] = false;
      }      
      if (sprite_x > width - al_get_bitmap_width(sprite)) 
          sprite_x = width - al_get_bitmap_width(sprite);
      if (sprite_y > height - al_get_bitmap_height(sprite)) 
          sprite_y = height - al_get_bitmap_height(sprite);


      al_clear_to_color(al_map_rgb(0, 0, 0));   
      al_draw_scaled_bitmap(fundo, 0, 0, 400, 246, 0, 0, 800, 600, 0);

      
      double tempo_atual = al_get_time() - init_time;
      char tempo_texto[32];
      snprintf(tempo_texto, sizeof(tempo_texto), "Tempo: %.1f s", tempo_atual);
      al_draw_text(font, al_map_rgb(0, 0, 0), 20, 20, 0, tempo_texto);

      al_draw_bitmap(sprite, sprite_x, sprite_y, 0);
      
      al_flip_display();
    }
  }  al_destroy_bitmap(fundo);
  al_destroy_bitmap(sprite);
  al_destroy_font(font);
  al_destroy_display(display);
  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  return 0;
}
