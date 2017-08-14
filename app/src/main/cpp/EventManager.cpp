#include "EventManager.h"
#include "Log.h"
#include "Utilities.h"
#include "TimeManager.h"

#include <cassert>

namespace Events {

//---------------------------------------------------------------------------------------------------------------------
// EventManager::AddListener
//---------------------------------------------------------------------------------------------------------------------
    bool EventManager::AddListener(const EventListenerDelegate &eventDelegate,
                                   const EventType &type) {
        Log::debug("Events Attempting to add delegate function for event type: %s", to_string(type).c_str());

        EventListenerList& eventListenerList = mEventListeners[type];  // this will find or create the entry
        for (auto const & eventListener : eventListenerList) {
            if (eventDelegate.target_type() == eventListener.target_type()) {
                Log::debug("Attempting to double-register a delegate");
                return false;
            }
        }

        eventListenerList.push_back(eventDelegate);
        Log::debug("Events Successfully added delegate for event type: %s ", to_string(type).c_str());

        return true;
    }


//---------------------------------------------------------------------------------------------------------------------
// EventManager::RemoveListener
//---------------------------------------------------------------------------------------------------------------------
    bool EventManager::RemoveListener(const EventListenerDelegate &eventDelegate,
                                      const EventType &type) {
        Log::debug("Events Attempting to remove delegate function from event type: %s ", to_string(type).c_str());
        bool success = false;

        auto findIt = mEventListeners.find(type);
        if (findIt != mEventListeners.end()) {
            EventListenerList& listeners = findIt->second;
            for (auto it = listeners.begin(); it != listeners.end(); ++it) {
                if (eventDelegate.target_type() == (*it).target_type()) {
                    listeners.erase(it);
                    Log::debug("Events Successfully removed delegate function from event type: %s ", to_string(type).c_str());
                    success = true;
                    break;  // we don't need to continue because it should be impossible for the same delegate function to be registered for the same event more than once
                }
            }
        }

        return success;
    }


//---------------------------------------------------------------------------------------------------------------------
// EventManager::VTrigger
//---------------------------------------------------------------------------------------------------------------------
    bool EventManager::TriggerEvent(const IEventDataPtr &pEvent) const {
        Log::debug("Events Attempting to trigger event %s ", pEvent->GetName());
        bool processed = false;

        auto findIt = mEventListeners.find(pEvent->VGetEventType());
        if (findIt != mEventListeners.end()) {
            const EventListenerList& eventListenerList = findIt->second;
            for (EventListenerList::const_iterator it = eventListenerList.begin(); it != eventListenerList.end(); ++it) {
                EventListenerDelegate listener = (*it);
                Log::debug("Events Sending Event %s ", pEvent->GetName(), "to delegate.");
                listener(pEvent);  // call the delegate
                processed = true;
            }
        }

        return processed;
    }


//---------------------------------------------------------------------------------------------------------------------
// EventManager::QueueEvent
//---------------------------------------------------------------------------------------------------------------------
    bool EventManager::QueueEvent(const IEventDataPtr &pEvent) {
        assert(mActiveQueue >= 0);
        assert(mActiveQueue < EVENTMANAGER_NUM_QUEUES);

        // make sure the event is valid
        if (!pEvent) {
            Log::error("Invalid event in QueueEvent()");
            assert(false);
            return false;
        }

        Log::debug("Events Attempting to queue event: %s ", pEvent->GetName());

        auto findIt = mEventListeners.find(pEvent->VGetEventType());
        if (findIt != mEventListeners.end()) {
            mQueues[mActiveQueue].push_back(pEvent);
            Log::debug("Events Successfully queued event: %s", pEvent->GetName());
            return true;
        }
        else {
            Log::error("Events Skipping event since there are no delegates registered to receive it: %s ", pEvent->GetName());
            assert(false);
            return false;
        }
    }



//---------------------------------------------------------------------------------------------------------------------
// EventManager::AbortEvent
//---------------------------------------------------------------------------------------------------------------------
    bool EventManager::AbortEvent(const EventType &inType, bool allOfType) {
        assert(mActiveQueue >= 0);
        assert(mActiveQueue < EVENTMANAGER_NUM_QUEUES);

        bool success = false;
        EventListenerMap::iterator findIt = mEventListeners.find(inType);

        if (findIt != mEventListeners.end()) {
            EventQueue& eventQueue = mQueues[mActiveQueue];
            auto it = eventQueue.begin();
            while (it != eventQueue.end()) {
                // Removing an item from the queue will invalidate the iterator, so have it point to the next member.  All
                // work inside this loop will be done using thisIt.
                auto thisIt = it;
                ++it;

                if ((*thisIt)->VGetEventType() == inType) {
                    eventQueue.erase(thisIt);
                    success = true;
                    if (!allOfType)
                        break;
                }
            }
        }

        return success;
    }


//---------------------------------------------------------------------------------------------------------------------
// EventManager::VTick
//---------------------------------------------------------------------------------------------------------------------
    bool EventManager::Update(float maxSecs, bool processMaxEvents) {
        float currSec = static_cast<float>(TimeManager::GetInstance().GetTimeNow());
        float maxSec =  currSec + maxSecs;

        // swap active queues and clear the new queue after the swap
        int queueToProcess = mActiveQueue;
        mActiveQueue = (mActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
        mQueues[mActiveQueue].clear();

        if(!mQueues[queueToProcess].empty()) {
            Log::debug("EventLoop Processing Event Queue %s ; %s events to process",
                       to_string(queueToProcess).c_str(),
                       to_string(mQueues[queueToProcess].size()).c_str());
        }

        // Process the queue
        while (!mQueues[queueToProcess].empty()) {
            // pop the front of the queue
            IEventDataPtr pEvent = mQueues[queueToProcess].front();
            mQueues[queueToProcess].pop_front();
            Log::debug("EventLoop Processing Event %s ", pEvent->GetName());

            const EventType & eventType = pEvent->VGetEventType();

            // find all the delegate functions registered for this event
            auto findIt = mEventListeners.find(eventType);
            if (findIt != mEventListeners.end()) {
                const EventListenerList& eventListeners = findIt->second;
                Log::debug("EventLoopnFound %s ", to_string(eventListeners.size()).c_str(), " delegates");

                // call each listener
                for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it) {
                    EventListenerDelegate listener = (*it);
                    Log::debug("EventLoop Sending event %s ", pEvent->GetName(), " to delegate");
                    listener(pEvent);
                }
            }

            // check to see if time ran out
            currSec = static_cast<float>(TimeManager::GetInstance().GetTimeNow());
            if (processMaxEvents && currSec >= maxSec) {
                Log::debug("EventLoop Aborting event processing; time ran out");
                break;
            }
        }

        // If we couldn't process all of the events, push the remaining events to the new active queue.
        // Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
        bool queueFlushed = mQueues[queueToProcess].empty();
        if (!queueFlushed) {
            while (!mQueues[queueToProcess].empty()) {
                IEventDataPtr pEvent = mQueues[queueToProcess].back();
                mQueues[queueToProcess].pop_back();
                mQueues[mActiveQueue].push_front(pEvent);
            }
        }

        return queueFlushed;
    }
}