#pragma once

#include <queue>


class Event
{
	public :
		float ScrollDistanceX, ScrollDistanceY;
};

std::queue<Event> EventQueue;