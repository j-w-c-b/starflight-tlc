/*
	STARFLIGHT - THE LOST COLONY
	Events.h - base event class from which all other events derive; this allows an Event pointer
	to be used when sending and receiving events; RTTI should be used to figure out
	the specific derived class for a given event object.
	Author: ?
	Date: ?

   Sub-class this class to create custom events for your module.

*/

#ifndef EVENTS_H
#define EVENTS_H

// all eventType definitions should go here, really.

enum
{
//NOTE: the game engine doesn't treat this as anything special and will happily broadcast it.
    EVENT_NONE                = 55000,
    EVENT_CLOSE,
    EVENT_SAVE_GAME,
    EVENT_LOAD_GAME,
    EVENT_QUIT_GAME,
    EVENT_MOUSEOVER,
    EVENT_HIDE_CONTROLS,
    EVENT_SHOW_CONTROLS,
    EVENT_CHANGE_MODULE,

    CARGO_EVENT_UPDATE,
};

/**
 * Event class
 * The generic Event class is useful for simple events where no data is needed.
 * Use a unique eventType identifier to notify your modules of simple events.
 * For more complex events, where passing data is desired, sub-class your own event.
 **/
class Event
{
protected:
   int eventType;
public:
   Event() {}
   Event(int eventType) { this->eventType = eventType; }
   virtual ~Event() {}
   void setEventType(int eventType) { this->eventType = eventType; }
   int getEventType() { return this->eventType; }
};
#endif
