#include "Utilities.h"



void ParseStringWithPunct(std::string const &src,
                          std::list<std::string> &dst) {
    std::string texName;
    for(auto const & c: src) {
        if(!ispunct(c))
            texName.push_back(c);
        else {
            dst.push_back(texName);
            texName.clear();
        }
    }
}

GameState StrToGameState(std::string const &src) {
    if(src == "START") return GameState::START;
    if(src == "ACTIVE") return GameState::ACTIVE;
    if(src == "PAUSE") return GameState::PAUSE;
    if(src == "FINISH") return GameState::FINISH;

    Log::error("XML ERROR: Can't handle string game state : %s", src.c_str());
    assert(false);
}

LayoutType StrToLayout(std::string const &src) {
    if(src == "CENTER") return LayoutType::CENTER;
    if(src == "LEFT") return LayoutType::LEFT;
    if(src == "RIGHT") return LayoutType::RIGHT;

    Log::error("XML ERROR: Can't handle string layout type : %s", src.c_str());
    assert(false);
}
