#pragma once

#include "EventManager.h"
#include "Utilities.h"
#include "FlappyEngine.h"
#include "GameTypes.h"

namespace Events {
    extern GenericObjectFactory<IEventData, EventType> gEventFactory;

    class EventLoadResources : public BaseEventData {
    public:
        static const EventType sEventType;

        EventLoadResources() = default;
        virtual const EventType &VGetEventType(void) const { return sEventType; }
        virtual IEventDataPtr VCopy(void) const { return IEventDataPtr(new EventLoadResources()); }
        virtual const char *GetName(void) const { return "EventLoadResources"; }
    };

    class EventUnloadResources : public BaseEventData {
    public:
        static const EventType sEventType;

        EventUnloadResources() = default;
        virtual const EventType &VGetEventType(void) const { return sEventType; }
        virtual IEventDataPtr VCopy(void) const { return IEventDataPtr(new EventUnloadResources()); }
        virtual const char *GetName(void) const { return "EventUnloadResources"; }
    };


    class EventChangeGameState : public BaseEventData {
    public:
        static const EventType sEventType;

        EventChangeGameState(GameState nextState) : mNextState{nextState}
        {}
        GameState GetNextGameState() const { return mNextState; }

        virtual const EventType &VGetEventType(void) const { return sEventType; }
        virtual IEventDataPtr VCopy(void) const { return IEventDataPtr(new EventChangeGameState(mNextState)); }
        virtual const char *GetName(void) const { return "EventChangeGameState"; }

    private:
        GameState mNextState = GameState::PAUSE;
    };

    class EventInputXY : public BaseEventData {
    public:
        static const EventType sEventType;

        EventInputXY(glm::vec2 inputXY) : mInputXY{inputXY}
        {}
        glm::vec2 const & GetInputXY() const { return mInputXY; }

        virtual const EventType &VGetEventType(void) const { return sEventType; }
        virtual IEventDataPtr VCopy(void) const { return IEventDataPtr(new EventInputXY(mInputXY)); }
        virtual const char *GetName(void) const { return "EventInputXY"; }

    private:
        glm::vec2 mInputXY = {};
    };


//    class EventBackKeyPressed : public BaseEventData {
//    public:
//        static const EventType sEventType;
//
//        EventBackKeyPressed() = default;
//        virtual const EventType &VGetEventType(void) const { return sEventType; }
//        virtual IEventDataPtr VCopy(void) const { return IEventDataPtr(new EventBackKeyPressed()); }
//        virtual const char *GetName(void) const { return "EventBackKeyPressed"; }
//    };
//
//    class EventMenuKeyPressed : public BaseEventData {
//    public:
//        static const EventType sEventType;
//
//        EventMenuKeyPressed() = default;
//        virtual const EventType &VGetEventType(void) const { return sEventType; }
//        virtual IEventDataPtr VCopy(void) const { return IEventDataPtr(new EventMenuKeyPressed()); }
//        virtual const char *GetName(void) const { return "EventMenuKeyPressed"; }
//    };
//
//    class EventSwitchAppKeyPressed : public BaseEventData {
//    public:
//        static const EventType sEventType;
//
//        EventSwitchAppKeyPressed() = default;
//        virtual const EventType &VGetEventType(void) const { return sEventType; }
//        virtual IEventDataPtr VCopy(void) const { return IEventDataPtr(new EventSwitchAppKeyPressed()); }
//        virtual const char *GetName(void) const { return "EventSwitchAppKeyPressed"; }
//    };
}
