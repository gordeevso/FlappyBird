#include "Events.h"

namespace Events {
    GenericObjectFactory<IEventData, EventType> gEventFactory;

    EventType const EventLoadResources::sEventType(0xa3814acd);
    EventType const EventUnloadResources::sEventType(0x8d3dbd7a);
    EventType const EventChangeGameState::sEventType(0x9a45779a);
    EventType const EventInputXY::sEventType(0x8e2850f8);


//    EventType const EventBackKeyPressed::sEventType(0xd9d101ea);
//    EventType const EventMenuKeyPressed::sEventType(0xf066a9dd);
//    EventType const EventSwitchAppKeyPressed::sEventType(0x507d5144);
}
