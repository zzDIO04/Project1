/*** BIBLIOTECAS ***/
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

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
  
  return true;
}

/*** MAIN FUNC ***/ 
int main(int argc, char *argv[]){
  int tp = 0;
  
  if (tp == 0){
    if (init_func() == false){
      printf("erro: iniciar a biblioteca\n");
      return 0;
    }
    tp++;
  }
  
  ALLEGRO_DISPLAY* display = al_create_display(width, height);
  al_set_window_title(display, "ojogo");
  ALLEGRO_FONT* font = al_create_builtin_font(); 
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
  ALLEGRO_BITMAP* menu = al_load_bitmap("./assets/mainmenu.png");
  ALLEGRO_SAMPLE* menu_music = al_load_sample("./sfx/8bitmenu.mp3");
  ALLEGRO_SAMPLE_INSTANCE* music_instance = al_create_sample_instance(menu_music);
// Arrumar para carregar dps
  ALLEGRO_BITMAP* fundo = al_load_bitmap("./assets/background.png");
  ALLEGRO_BITMAP* sprite = al_load_bitmap("./assets/chsprt.png");
    if (!sprite) {
        printf("Erro ao carregar sprite1.png!\n");
        return -1;
    }

  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  al_register_event_source(queue, al_get_display_event_source(display));
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_attach_sample_instance_to_mixer(music_instance, al_get_default_mixer());
  al_start_timer(timer);
  double init_time = 0;
  bool on_menu = true;
  float sprite_x = 100;
  float sprite_y = 100;
  float velocidade = 4.0;
  bool teclas[ALLEGRO_KEY_MAX] = { false };  
  
  while(true){
    ALLEGRO_EVENT event;
    al_wait_for_event(queue, &event);
    
    if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      break;
    }
    else if(event.type == ALLEGRO_EVENT_KEY_DOWN){
      teclas[event.keyboard.keycode] = true;
      if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
        break;
      }
      else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER && on_menu){
        al_stop_sample_instance(music_instance);
        on_menu = false;
      }
      else if (event.keyboard.keycode == ALLEGRO_KEY_P){
        screenshot(display);
      }
    }
    else if (event.type == ALLEGRO_EVENT_KEY_UP){
      teclas[event.keyboard.keycode] = false;
    }
    else if (event.type == ALLEGRO_EVENT_TIMER) {
      if (!on_menu) {
        if (teclas[ALLEGRO_KEY_UP]) sprite_y -= velocidade;
        if (teclas[ALLEGRO_KEY_DOWN]) sprite_y += velocidade;
        if (teclas[ALLEGRO_KEY_LEFT]) sprite_x -= velocidade;
        if (teclas[ALLEGRO_KEY_RIGHT]) sprite_x += velocidade;
      }
      al_clear_to_color(al_map_rgb(255, 255, 255));
      ALLEGRO_BITMAP* background = on_menu ? menu : fundo;
      al_draw_bitmap(background, 0, 0, 0);

      if (on_menu) {
        al_set_sample_instance_playmode(music_instance, ALLEGRO_PLAYMODE_LOOP);
        al_play_sample_instance(music_instance);
        al_draw_text(font, al_map_rgb(255, 165, 0), width / 2, 250, ALLEGRO_ALIGN_CENTER, "Pressione Enter para Iniciar...");
      }
      else {
        double tempo_atual = al_get_time() - init_time;
        char tempo_texto[32];
        snprintf(tempo_texto, sizeof(tempo_texto), "Tempo: %.1f s", tempo_atual);
        al_draw_text(font, al_map_rgb(0, 0, 0), 20, 20, 0, tempo_texto);

        al_draw_bitmap(sprite, sprite_x, sprite_y, 0);
      }
      al_flip_display();
    }
  }

  al_destroy_bitmap(fundo);
  al_destroy_bitmap(menu);
  al_destroy_bitmap(sprite);
  al_destroy_font(font);
  al_destroy_display(display);
  al_destroy_event_queue(queue);
  al_destroy_sample_instance(music_instance);
  al_destroy_sample(menu_music);

  return 0;
}
