#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>

#include "TextRenderer.h"
#include "Utilities.h"
#include "GameTypes.h"
#include "Events.h"

class Ui {

public:
    using TextRendererMap = EnumKeyUnorderedMap<GameState, std::shared_ptr<TextRenderer>>;

public:
    Ui();
    ~Ui();


    void LoadResources(std::string const & uiFilePath);
    void Update();
    void Draw();

    void UpdateScoreDelegate(Events::IEventDataPtr ptrEvent);
    void FinalScoreDelegate(Events::IEventDataPtr ptrEvent);

private:
    std::unordered_map<std::string, FTString> mStringsMap;
    TextRendererMap mTextRenderers;
};
