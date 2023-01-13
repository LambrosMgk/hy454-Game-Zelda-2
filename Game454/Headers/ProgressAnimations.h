#pragma once

#include "GameObjectsClasses.h"

extern ALLEGRO_TIMER* AnimationTimer, *AttackTimer;
extern ALLEGRO_EVENT_QUEUE* AnimatorQueue;
extern bool StartAttack;

void Animator_Init();

void Animator();