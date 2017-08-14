#pragma once

#include <memory>
#include <functional>
#include <queue>
#include <list>
#include <unordered_map>

namespace Events {

    class IEventData;

    using EventType = uint64_t;
    using IEventDataPtr = std::shared_ptr<IEventData>;
    using EventListenerDelegate = std::function<void(IEventDataPtr)>;
    using EventListenerList = std::list<EventListenerDelegate>;
    using EventListenerMap = std::unordered_map<EventType, EventListenerList>;
    using EventQueue = std::list<IEventDataPtr>;

//---------------------------------------------------------------------------------------------------------------------
// IEventData
// Base type for event object hierarchy, may be used itself for simplest event notifications such as those that do
// not carry additional payload data. If any event needs to propagate with payload data it must be defined separately.
//---------------------------------------------------------------------------------------------------------------------
    class IEventData {
    public:
        virtual ~IEventData() {}
        virtual const EventType& VGetEventType(void) const = 0;
        virtual float GetTimeStamp() const = 0;
        virtual IEventDataPtr VCopy() const = 0;
        virtual const char* GetName() const = 0;

    };


//---------------------------------------------------------------------------------------------------------------------
// class BaseEventData
//---------------------------------------------------------------------------------------------------------------------
    class BaseEventData : public IEventData {
    public:
        explicit BaseEventData(const float timeStamp = 0.0f) : mTimeStamp(timeStamp)
        {}

        virtual const EventType& VGetEventType() const = 0;
        float GetTimeStamp() const { return mTimeStamp; }

    private:
        const float mTimeStamp;

    };


//---------------------------------------------------------------------------------------------------------------------
// EventManager Description
//
// This is the object which maintains the list of registered events and their listeners.
//
// This is a many-to-many relationship, as both one listener can be configured to process multiple event types and
// of course multiple listeners can be registered to each event type.
//
// The interface to this construct uses smart pointer wrapped objects, the purpose being to ensure that no object
// that the registry is referring to is destroyed before it is removed from the registry AND to allow for the registry
// to be the only place where this list is kept ... the application code does not need to maintain a second list.
//
// Simply tearing down the registry (e.g.: destroying it) will automatically clean up all pointed-to objects (so long
// as there are no other outstanding references, of course).
//---------------------------------------------------------------------------------------------------------------------
    int32_t const EVENTMANAGER_NUM_QUEUES = 2;

    class EventManager {
    public:
        static EventManager & Get() {
            static EventManager instance;
            return instance;
        };

    public:
        explicit EventManager() = default;
        ~EventManager() = default;

        // Registers a delegate function that will get called when the event type is triggered.  Returns true if
        // successful, false if not.
        bool AddListener(const EventListenerDelegate &eventDelegate, const EventType &type);

        // Removes a delegate / event type pairing from the internal tables.  Returns false if the pairing was not found.
        bool RemoveListener(const EventListenerDelegate &eventDelegate, const EventType &type);

        // Fire off event NOW.  This bypasses the queue entirely and immediately calls all delegate functions registered
        // for the event.
        bool TriggerEvent(const IEventDataPtr &pEvent) const;

        // Fire off event.  This uses the queue and will call the delegate function on the next call to VTick(), assuming
        // there's enough time.
        bool QueueEvent(const IEventDataPtr &pEvent);

        // Find the next-available instance of the named event type and remove it from the processing queue.  This
        // may be done up to the point that it is actively being processed ...  e.g.: is safe to happen during event
        // processing itself.
        //
        // if allOfType is true, then all events of that type are cleared from the input queue.
        //
        // returns true if the event was found and removed, false otherwise
        bool AbortEvent(const EventType &type, bool allOfType = false);

        // Allow for processing of any queued messages, optionally specify a processing time limit so that the event
        // processing does not take too long. Note the danger of using this artificial limiter is that all messages
        // may not in fact get processed.
        //
        // returns true if all messages ready for processing were completed, false otherwise (e.g. timeout )
        bool Update(float maxMillis, bool processMaxEvents);

        // Getter for the main global event manager.  This is the event manager that is used by the majority of the
        // engine, though you are free to define your own as long as you instantiate it with setAsGlobal set to false.
        // It is not valid to have more than one global event manager.
    private:
        EventListenerMap mEventListeners;
        EventQueue mQueues[EVENTMANAGER_NUM_QUEUES];
        int32_t mActiveQueue;  // index of actively processing queue; events enque to the opposing queue

    };








}

