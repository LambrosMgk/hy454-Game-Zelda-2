#pragma once

#include <queue>

enum EventType { EventType_Scroll, EventType_ShowGrid, EventType_HideGrid};
//maybe create sub classes of event to save memory (instead of allocating a big block for an "Event" object allocate a smaller block for a specific event
class Event
{
	public :
		EventType eventType;
		float ScrollDistanceX, ScrollDistanceY;
};

std::queue<Event> EventQueue;