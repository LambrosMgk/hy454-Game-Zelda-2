#pragma once
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

int allegro_startup(void);

void allegro_shut_down(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue);