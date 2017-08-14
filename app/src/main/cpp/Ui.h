#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include <glm/glm.hpp>

#include "TextRenderer.h"
#include "Utilities.h"
#include "GameTypes.h"


class Ui {

public:
    using TextRendererMap = EnumKeyUnorderedMap<GameState, std::shared_ptr<TextRenderer>>;


public:
    Ui() = default;
    ~Ui() = default;

    void LoadResources(std::string const & uiFilePath);
    void Update();
    void Draw();

private:
    TextRendererMap mTextRenderers;
};
