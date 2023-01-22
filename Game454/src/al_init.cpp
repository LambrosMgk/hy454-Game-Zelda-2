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
					if (al_install_audio())
					{
						if (al_init_acodec_addon())
						{
							if (al_reserve_samples(1))
							{
								return 1;
							}
							else
								fprintf(stderr, "ERROR: Failed to reserve samples:(\n");
							//al_shutdown_acodec_addon(); Does not exist
						}
						else
							fprintf(stderr, "ERROR: Failed to initialize acodec addon\n");
						al_uninstall_audio();
					}
					else
						fprintf(stderr, "ERROR: Failed to install audio\n");
					al_shutdown_image_addon();
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
	al_uninstall_audio();
	//al_shutdown_ttf_addon();
	//al_shutdown_font_addon();
	al_shutdown_image_addon();
	//al_uninstall_mouse();
	al_uninstall_keyboard();
	al_shutdown_primitives_addon();
	al_uninstall_system();
}


//might be useful
/*int allegro_startup(void)
{
	if (al_init())
	{
		if (al_init_primitives_addon())
		{
			if (al_install_keyboard())
			{
				if (al_install_mouse())
				{
					if (al_init_image_addon())
					{
						al_init_font_addon();   //Void
						if (al_init_ttf_addon())
						{
							if (al_install_audio())
							{
								if (al_init_acodec_addon())
								{
									if (al_reserve_samples(1))
									{

										return AL_STARTUP_SUCCESS;


									}
									else
										fprintf(stderr, "ERROR: Failed to reserve samples:(\n");
									//al_shutdown_acodec_addon(); Does not exist
								}
								else
									fprintf(stderr, "ERROR: Failed to initialize acodec addon\n");
								al_uninstall_audio();
							}
							else
								fprintf(stderr, "ERROR: Failed to install audio\n");
							al_shutdown_ttf_addon();
						}
						else
							fprintf(stderr, "ERROR: Failed to initialize ttf addon\n");
						al_shutdown_font_addon();
						al_shutdown_image_addon();
					}
					else
						fprintf(stderr, "ERROR: Failed to initialize image addon\n");
					al_uninstall_mouse();
				}
				else
					fprintf(stderr, "ERROR: Failed to install mouse\n");
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
	return AL_STARTUP_ERROR;
}*/