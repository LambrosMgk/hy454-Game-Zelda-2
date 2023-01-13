#include "..\\Headers\al_init.h"

int allegro_startup(void)
{
	if (al_init())
	{
		if (al_init_primitives_addon())
		{
			if (al_install_keyboard())
			{
				if (al_init_image_addon())
				{
					return 1;
				}
				else
					fprintf(stderr, "ERROR: Failed to initialize image addon\n");
				al_uninstall_keyboard();
			}
			else
				fprintf(stderr, "ERROR: Failed to load primitives addon\n");
			al_shutdown_primitives_addon();
		}
		else
			fprintf(stderr, "ERROR: Failed to install keyboard\n");
		al_uninstall_system();
	}
	else
		fprintf(stderr, "ERROR: Failed to initialize allegro system\n");
	return 0;
}

void allegro_shut_down(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue)
{
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	//al_uninstall_audio();
	//al_shutdown_ttf_addon();
	//al_shutdown_font_addon();
	al_shutdown_image_addon();
	//al_uninstall_mouse();
	al_uninstall_keyboard();
	al_shutdown_primitives_addon();
	al_uninstall_system();
}