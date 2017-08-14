#include <tinyxml2.h>

#include "Ui.h"
#include "Utilities.h"
#include "GLState.h"
#include "ResourceManager.h"
#include "FlappyEngine.h"

void Ui::LoadResources(std::string const & uiFilePath) {
    tinyxml2::XMLDocument sceneXml;
    std::vector<uint8_t> xmlBuffer;
    ResourceManager::Read(uiFilePath, xmlBuffer);
    auto result = sceneXml.Parse(std::string(xmlBuffer.begin(), xmlBuffer.end()).c_str());

    Log::debug("LOAD Settings %s", uiFilePath.c_str());
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        assert(result == tinyxml2::XMLError::XML_SUCCESS);
    }

    tinyxml2::XMLElement *uiXmlRoot = sceneXml.RootElement();
    for (tinyxml2::XMLElement *ptrNodeState = uiXmlRoot->FirstChildElement();
         ptrNodeState;
         ptrNodeState = ptrNodeState->NextSiblingElement()) {

        assert(ptrNodeState);
        size_t fontSize = ptrNodeState->IntAttribute("FontSize");
        assert(fontSize > 0);
        std::string fontFile = "fonts/";
        fontFile += ptrNodeState->Attribute("FontType");

        tinyxml2::XMLElement *ptrNodeString = ptrNodeState->FirstChildElement();
        assert(ptrNodeString);

        std::string gameStateStr = ptrNodeString->Attribute("Type");
        GameState gameState = StrToGameState(gameStateStr);
        Log::debug("1111");

        auto textRenderer = std::shared_ptr<TextRenderer>(new TextRenderer);
        auto resTextRenderersIt = mTextRenderers.emplace(std::make_pair(gameState, textRenderer));
        if(!resTextRenderersIt.second) {
            Log::error("Ui ERROR : Renderer for this state already exist");
            assert(false);
        }
        else {
            resTextRenderersIt.first->second->Init(fontFile, fontSize);
        }
    }

    for(auto & textRenderer : mTextRenderers) {
        std::vector<UiString> const & uiStrRef = ResourceManager::GetUiStrings(textRenderer.first);
        for(auto const & uiStr: uiStrRef) {
            textRenderer.second->AddRenderString(uiStr);
        }
    }


}

void Ui::Update() {

}

void Ui::Draw() {
    mTextRenderers[FlappyEngine::GetGameState()]->DrawStrings();
}

