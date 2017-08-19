#include <tinyxml2.h>

#include "Ui.h"
#include "Utilities.h"
#include "GLState.h"
#include "ResourceManager.h"
#include "FlappyEngine.h"


Ui::Ui() {
    Events::EventListenerDelegate delegate;
    delegate = std::bind(&Ui::UpdateScoreDelegate, this, std::placeholders::_1);;
    Events::EventManager::Get().AddListener(delegate, Events::EventUpdateScore::sEventType);
    delegate = std::bind(&Ui::FinalScoreDelegate, this, std::placeholders::_1);;
    Events::EventManager::Get().AddListener(delegate, Events::EventFinalScore::sEventType);
}

Ui::~Ui() {
    Events::EventListenerDelegate delegate;
    delegate = std::bind(&Ui::UpdateScoreDelegate, this, std::placeholders::_1);
    Events::EventManager::Get().RemoveListener(delegate, Events::EventUpdateScore::sEventType);
    delegate = std::bind(&Ui::FinalScoreDelegate, this, std::placeholders::_1);
    Events::EventManager::Get().RemoveListener(delegate, Events::EventFinalScore::sEventType);
}


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
            FTString ftStr {};
            ftStr.state = textRenderer.first;
            textRenderer.second->CalcUiString(uiStr, ftStr);
            mStringsMap.insert(std::make_pair(uiStr.name, ftStr));
        }
    }


}

void Ui::Update() {

}

void Ui::Draw() {
    for(auto const & ftStr: mStringsMap) {
        if(FlappyEngine::GetGameState() == ftStr.second.state) {
            ftStr.second.Draw(mTextRenderers[FlappyEngine::GetGameState()]);
        }
    }
}


void Ui::UpdateScoreDelegate(Events::IEventDataPtr ptrEvent) {
    std::shared_ptr<Events::EventUpdateScore> ptrCastedEvent = std::static_pointer_cast<Events::EventUpdateScore>(ptrEvent);

    auto resIt = mStringsMap.find("ScoreActive");
    GameState strState {};
    if(resIt != mStringsMap.end()) {
        strState = resIt->second.state;
        mStringsMap.erase(resIt);
    }
    else {
        Log::debug("UiString not found !");
        assert(false);
    }

    std::vector<UiString> & uiStrRefA = ResourceManager::GetUiStrings(strState);
    for(auto & uiStr: uiStrRefA) {

        if(uiStr.name == "ScoreActive") {
            uiStr.text = ptrCastedEvent->GetScore();
            FTString ftStr{};
            ftStr.state = strState;
            mTextRenderers[strState]->CalcUiString(uiStr, ftStr);
            mStringsMap.insert(std::make_pair(uiStr.name, ftStr));
            break;
        }
    }
}

void Ui::FinalScoreDelegate(Events::IEventDataPtr ptrEvent) {
    std::shared_ptr<Events::EventFinalScore> ptrCastedEvent = std::static_pointer_cast<Events::EventFinalScore>(ptrEvent);

    auto resIt = mStringsMap.find("ScoreFinish");
    GameState strState {};
    if(resIt != mStringsMap.end()) {
        strState = resIt->second.state;
        mStringsMap.erase(resIt);
    }
    else {
        Log::debug("UiString not found !");
        assert(false);
    }

    std::vector<UiString> & uiStrRefF = ResourceManager::GetUiStrings(strState);
    for(auto & uiStr: uiStrRefF) {
        if(uiStr.name == "ScoreFinish") {
            uiStr.text = ptrCastedEvent->GetScore();
            FTString ftStr{};
            ftStr.state = strState;
            mTextRenderers[strState]->CalcUiString(uiStr, ftStr);
            mStringsMap.insert(std::make_pair(uiStr.name, ftStr));
            break;
        }
    }
}

