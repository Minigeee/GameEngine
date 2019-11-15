#ifndef EVENT_LISTENER_H
#define EVENT_LISTENER_H

#include <Core/DataTypes.h>
#include <Core/Macros.h>
#include <Core/TypeInfo.h>

///////////////////////////////////////////////////////////////////////////////

class Scene;

class EventListener
{
public:
	/* Handles all incoming events */
	virtual void HandleEvent(const void* e, Uint32 type) = 0;
	/* Register listener for events */
	virtual void RegisterEvents(Scene* scene) = 0;
};

///////////////////////////////////////////////////////////////////////////////

#include <Scene/Scene.h>

#define HANDLE_EVENT_FUNC_NAME HandleEvent
#define HANDLE_EVENT_FUNC(x) if (type == x::StaticTypeID()) { HANDLE_EVENT_FUNC_NAME(*(const x*)e); return; }
#define REGISTER_EVENT_FUNC(x) scene->RegisterListener<x>(this);

#define HANDLE_EVENTS_IMPL(...) \
public: \
	void HandleEvent(const void* e, Uint32 type) override { LOOP(HANDLE_EVENT_FUNC, __VA_ARGS__) } \
	void RegisterEvents(Scene* scene) override { LOOP(REGISTER_EVENT_FUNC, __VA_ARGS__) }

/*
* Add this macro to class definition
* To use, list all events you want to handle
*/
#define HANDLE_EVENTS(...) HANDLE_EVENTS_IMPL(__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////

#endif