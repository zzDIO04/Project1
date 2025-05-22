#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <time.h>
#include <stdio.h>
#define WIDTH 800
#define HEIGHT 600

void tirar_screenshot(ALLEGRO_DISPLAY* display) {
    ALLEGRO_BITMAP* screenshot = al_create_bitmap(WIDTH, HEIGHT);
    if (!screenshot) {
        printf("Erro ao criar bitmap para screenshot!\n");
        return;
    }

    al_set_target_bitmap(screenshot);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(al_get_backbuffer(display), 0, 0, 0);
    al_set_target_backbuffer(display);

    char filename[64];
    time_t now = time(NULL);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    strftime(filename, sizeof(filename), "screenshot_%Y%m%d_%H%M%S.png", &timeinfo);

    if (al_save_bitmap(filename, screenshot)) {
        printf("Screenshot salva como %s\n", filename);
    }
    else {
        printf("Erro ao salvar screenshot.\n");
    }

    al_destroy_bitmap(screenshot);
}

int main() {
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(2);

    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
    al_set_window_position(display, 200, 200);
    al_set_window_title(display, "titulo teste");

    ALLEGRO_FONT* font = al_load_ttf_font("./Fonts/Font1.ttf", 48, 0);
    if (!font) {
        printf("Erro ao carregar a fonte!\n");
        return -1;
    }

    ALLEGRO_BITMAP* sprite = al_load_bitmap("./img/sprite.png");
    if (!sprite) {
        printf("Erro ao carregar sprite1.png!\n");
        return -1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_BITMAP* menu = al_load_bitmap("./img/menu1.png");
    ALLEGRO_BITMAP* fundo = al_load_bitmap("./img/fundo1.png");
    ALLEGRO_SAMPLE* menu_music = al_load_sample("./8bitmenu.mp3");
    ALLEGRO_SAMPLE_INSTANCE* music_instance = al_create_sample_instance(menu_music);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_attach_sample_instance_to_mixer(music_instance, al_get_default_mixer());
    al_start_timer(timer);

    double tempo_inicio = 0;
    bool on_menu = true;
    float sprite_x = 100;
    float sprite_y = 100;
    float velocidade = 4.0;
    bool teclas[ALLEGRO_KEY_MAX] = { false };

    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            teclas[event.keyboard.keycode] = true;

            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                break;
            }

            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER && on_menu) {
                al_stop_sample_instance(music_instance);
                tempo_inicio = al_get_time();
                on_menu = false;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_P) {
                tirar_screenshot(display);
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            teclas[event.keyboard.keycode] = false;
        }
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            // Atualiza posição do sprite se estiver fora do menu
            if (!on_menu) {
                if (teclas[ALLEGRO_KEY_UP]) sprite_y -= velocidade;
                if (teclas[ALLEGRO_KEY_DOWN]) sprite_y += velocidade;
                if (teclas[ALLEGRO_KEY_LEFT]) sprite_x -= velocidade;
                if (teclas[ALLEGRO_KEY_RIGHT]) sprite_x += velocidade;
            }

            al_clear_to_color(al_map_rgb(255, 255, 255));
            ALLEGRO_BITMAP* imagem_atual = on_menu ? menu : fundo;
            al_draw_bitmap(imagem_atual, 0, 0, 0);

            if (on_menu) {
                al_set_sample_instance_playmode(music_instance, ALLEGRO_PLAYMODE_LOOP);
                al_play_sample_instance(music_instance);
                al_draw_text(font, al_map_rgb(0, 0, 0), WIDTH / 2 + 2, 252, ALLEGRO_ALIGN_CENTER, "Pressione Enter para Iniciar...");
                al_draw_text(font, al_map_rgb(255, 165, 0), WIDTH / 2, 250, ALLEGRO_ALIGN_CENTER, "Pressione Enter para Iniciar...");
            }
            else {
                // Tempo
                double tempo_atual = al_get_time() - tempo_inicio;
                char tempo_texto[32];
                snprintf(tempo_texto, sizeof(tempo_texto), "Tempo: %.1f s", tempo_atual);
                al_draw_text(font, al_map_rgb(0, 0, 0), 20, 20, 0, tempo_texto);

                // Desenha o sprite
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
    al_destroy_event_queue(event_queue);
    al_destroy_sample_instance(music_instance);
    al_destroy_sample(menu_music);

    return 0;
}
