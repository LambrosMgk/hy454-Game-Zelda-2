#pragma once

#include "GameObjectsClasses.h"

extern ALLEGRO_EVENT_QUEUE* CollisionQueue;
extern ALLEGRO_TIMER* PlayerHurtTimer;

void Collisions_init();

void CheckCollisions();