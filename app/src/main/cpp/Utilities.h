#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cassert>
#include <memory>
#include <list>

#include "Log.h"
#include "GameTypes.h"
#include "TextRenderer.h"

template<typename T>
std::string to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

void ParseStringWithPunct(std::string const &src,
                          std::list<std::string> &dst);

GameState StrToGameState(std::string const &src);
LayoutType StrToLayout(std::string const &src);

template <class BaseType, class SubType>
std::shared_ptr<BaseType> GenericObjectCreationFunction() {
    if(std::is_base_of<BaseType, SubType>::value)
        return std::shared_ptr<BaseType>(new SubType);
    Log::error("Generic Object Factory Error");
    assert(false);
}

template <class BaseClass, class IdType>
class GenericObjectFactory {
public:

    template <class SubClass>
    bool Register(IdType id) {
        auto findIt = mCreators.find(id);
        if (findIt == mCreators.end()) {
            mCreators[id] = &GenericObjectCreationFunction<BaseClass, SubClass>;
            return true;
        }

        return false;
    }

    std::shared_ptr<BaseClass> Create(IdType id) {
        auto findIt = mCreators.find(id);
        if (findIt != mCreators.end()) {
            ObjectCreatorFn ptrFunc = findIt->second;
            return ptrFunc();
        }

        return nullptr;
    }

private:
    using ObjectCreatorFn = std::function<std::shared_ptr<BaseClass>()>;
    std::unordered_map<IdType, ObjectCreatorFn> mCreators;
};


struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

template <typename Key>
using HashType = typename std::conditional<std::is_enum<Key>::value, EnumClassHash, std::hash<Key>>::type;

template <typename Key, typename T>
using EnumKeyUnorderedMap = std::unordered_map<Key, T, HashType<Key>>;