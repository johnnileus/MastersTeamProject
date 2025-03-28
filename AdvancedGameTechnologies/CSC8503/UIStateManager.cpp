#ifdef _WIN32



#include "UIStateManager.h"
#include "TutorialGame.h"
#include "AudioManager.h"

//extern TutorialGame* g;
//extern std::unique_ptr<TutorialGame> g;
extern TutorialGame* gRaw;

UIStateManager::UIStateManager() {

}

UIStateManager::~UIStateManager() {

}

void UIStateManager::States() {

    bool test = false;
    
    static bool hoverStartGame = false;
    static bool hoverMultiplayer = false;
	static bool hoverResume = false;
    static bool hoverSettings = false;
    static bool hoverQuit = false;

    switch (GetCurrentState()) {

        // --- Main Menu ---
    case UIState::MainMenu: {
#ifdef USEAGC

#else
        ImGui::Begin("Main Menu", NULL,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove);

        ImGui::SetWindowFontScale(2.0f);
        ImGui::SetWindowSize(ImVec2(843, 488));
        ImGui::SetWindowPos(ImVec2(185, 100));

        if (g->GetCursorLocked() == true) {
            g->ToggleCursor();
        }

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Hight Score : 00").x) * 0.5f);

        ImGui::TextColored(ImVec4(0, 255, 150, 1), "High Score : 0");

        ImGui::Spacing();
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = 5.0f;
        style.WindowRounding = 5.0f;
        style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.30f, 0.50f, 1.0f);

        //Start Game 
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Hight Score : 00").x) * 0.5f - 25.0f);
        if (ImGui::Button("Start Game", ImVec2(300, 60))) {

            SetCurrentState(UIState::InGame);
            g->setGamePaused(false);
            AudioManager::GetInstance().PlayEvent("event:/Game Start");
            g->ToggleCursor();
        }
        if (ImGui::IsItemHovered()) {
            if (!hoverStartGame) {
                AudioManager::GetInstance().PlayEvent("event:/Menu Option Slide");
                hoverStartGame = true;
            }
        }
        else {
            hoverStartGame = false;
        }

        //Multiplayer
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Hight Score : 00").x) * 0.5f - 25.0f);
        if (ImGui::Button("Multiplayer", ImVec2(300, 60))) {

            SetCurrentState(UIState::Multiplayer);
            AudioManager::GetInstance().PlayEvent("event:/Game Start");
        }
        if (ImGui::IsItemHovered()) {
            if (!hoverMultiplayer) {
                AudioManager::GetInstance().PlayEvent("event:/Menu Option Slide");
                hoverMultiplayer = true;
            }
        }
        else {
            hoverMultiplayer = false;
        }

        ImGui::End();
#endif // USEAGC

        
        break;
    }

    case UIState::InGame: {

        ImGui::Begin("In Game", NULL,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize);

        ImGui::SetWindowFontScale(2.0f);
        ImGui::SetWindowPos(ImVec2(500, 100));
        ImGui::SetWindowSize(ImVec2(300, 100));
        ImGui::TextColored(ImVec4(255, 0, 0, 1), "Wave 1 Begins !!");

        ImGui::End();
        break;
    }

    //  Pause Menu 
    case UIState::Paused: {

        g->ToggleCursor();


        ImGui::SetWindowPos(ImVec2(500, 100));
        ImGui::SetWindowSize(ImVec2(300, 100));

        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = 5.0f;
        style.WindowRounding = 5.0f;

        ImGui::Begin("Paused", NULL,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize);

        ImGui::SetWindowFontScale(2.0f);

        if (ImGui::Button("Resume", ImVec2(300, 60))) {

            SetCurrentState(UIState::InGame);
            g->setGamePaused(false);

        }
        if (ImGui::IsItemHovered()) {
            if (!hoverResume) {
                AudioManager::GetInstance().PlayEvent("event:/Menu Option Slide");
                hoverResume = true;
            }
        }
        else {
            hoverResume = false;
        }

        if (ImGui::Button("Quit", ImVec2(300, 60))) {

            SetCurrentState(UIState::MainMenu);
            g->setGamePaused(true);
        }
        if (ImGui::IsItemHovered()) {
            if (!hoverQuit) {
                AudioManager::GetInstance().PlayEvent("event:/Menu Option Slide");
                hoverQuit = true;
            }
        }
        else {
            hoverQuit = false;
        }

        if (ImGui::Button("Settings", ImVec2(300, 60))) {

            SetCurrentState(UIState::Settings);
        }
        if (ImGui::IsItemHovered()) {
            if (!hoverSettings) {
                AudioManager::GetInstance().PlayEvent("event:/Menu Option Slide");
                hoverSettings = true;
            }
        }
        else {
            hoverSettings = false;
        }
        ImGui::End();
        break;
    }

    case UIState::Settings: {



        ImGui::Begin("Audio", NULL,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize);

        ImGui::SetWindowFontScale(2.0f);
        ImGui::SetWindowPos(ImVec2(500, 100));
        ImGui::SetWindowSize(ImVec2(300, 100));

        ImGui::SliderInt("Audio", &audio, 0, 100);

        if (ImGui::Button("Save", ImVec2(200, 40))) {
            SetCurrentState(UIState::Paused);
            AudioManager::GetInstance().SetMasterVolume(audio);
        }

        ImGui::End();

        break;
    }

    case UIState::Multiplayer: {


        ImGui::Begin("Multiplayer", NULL,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize);

        ImGui::SetWindowFontScale(2.0f);
        ImGui::SetWindowPos(ImVec2(500, 100));
        ImGui::SetWindowSize(ImVec2(400, 200));

        //TODO: Set the text position little above of the center of the full screen
        ImGui::TextColored(ImVec4(0, 0, 255, 1), "YET TO COMPLETE !");

        if (ImGui::Button("Back", ImVec2(300, 60))) {
            SetCurrentState(UIState::MainMenu);
        }

        ImGui::End();

        break;
    }

    case UIState::InventorySys: {
        BuildInventoryUI();
        break;
    }


    //  End Game Screen 
    case UIState::EndGame: {

        g->ToggleCursor();
       
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.8f));

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetWorkCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 250));

        if (ImGui::Begin("GameOver", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
        {
            // Score text
            ImGui::SetWindowFontScale(2.0f);
            ImVec2 textSize = ImGui::CalcTextSize("Final Score: 0000");
            ImGui::SetCursorPosX((400 - textSize.x) * 0.5f);
            ImGui::Text("Final Score: %d", g->getPlayer()->GetFinalScore());

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15.0f); 
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(30, 20));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f)); 
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f)); 
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f)); 

            ImGui::SetWindowFontScale(1.5f); 
            textSize = ImGui::CalcTextSize("QUIT GAME");
            ImGui::SetCursorPosX((400 - (textSize.x + 60)) * 0.5f); 

            if (ImGui::Button("QUIT GAME")) {
                SetCurrentState(UIState::MainMenu);
                endScreenFlag = true;
                gRaw->InitScene("EnemyTestScene");
                gRaw->setGamePaused(true);
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
            ImGui::SetWindowFontScale(1.0f); 

            ImGui::End();
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        break;
    }
    }
}


void UIStateManager::HandleInput() {
    if (ImGui::IsKeyPressed(ImGuiKey_P)) {
        if (GetCurrentState() == UIState::InGame) {
            g->setGamePaused(true);
            SetCurrentState(UIState::Paused);
        }
        else {
			g->setGamePaused(false);
            SetCurrentState(UIState::InGame);
        }
    }
    if (ImGui::IsKeyPressed(ImGuiKey_I)) {

        if (GetCurrentState() == UIState::InGame) {

            SetCurrentState(UIState::InventorySys);
        }
        else {
            SetCurrentState(UIState::InGame);
        }
    }
}

UIState UIStateManager::GetCurrentState() {
    return currentState;
    if (gRaw->getPlayer()->GetHealth() <= 0) {
		SetCurrentState(UIState::EndGame);
    }
}

void UIStateManager::SetCurrentState(UIState newState) {
    currentState = newState;
}

#endif // _WIN32