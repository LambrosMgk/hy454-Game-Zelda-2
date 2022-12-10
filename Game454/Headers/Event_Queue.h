#pragma once

#include <queue>


class Event;	//forward declaration

std::queue<Event> EventQueue;

class Event
{
	public :
		float ScrollDistanceX, ScrollDistanceY;
};