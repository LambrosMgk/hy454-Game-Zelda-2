#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>


#include "GameObjectsClasses.h"

extern ALLEGRO_TIMER* PhysicsTimer;
extern ALLEGRO_EVENT_QUEUE* PhysicsQueue;
extern unsigned short jumpCountPixels;
extern bool scrollUp;

void Physics_Init();

void Calculate_Physics();