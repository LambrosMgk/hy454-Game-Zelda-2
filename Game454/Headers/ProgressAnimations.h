#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>


#include "GameObjectsClasses.h"

ALLEGRO_TIMER* AnimationTimer, *AttackTimer;
ALLEGRO_EVENT_QUEUE* AnimatorQueue;

bool StartAttack = false;

void Animator_Init();

void Animator();