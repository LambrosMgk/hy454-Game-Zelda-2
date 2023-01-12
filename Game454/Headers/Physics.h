#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>


#include "GameObjectsClasses.h"

ALLEGRO_TIMER* PhysicsTimer;
ALLEGRO_EVENT_QUEUE* PhysicsQueue;
unsigned short jumpCountPixels = 0;
boolean scrollUp = false;

void Physics_Init();

void Calculate_Physics();