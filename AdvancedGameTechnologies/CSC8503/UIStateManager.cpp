#ifdef _WIN32



#include "UIStateManager.h"
#include "iostream"

UIStateManager::UIStateManager() {

}

UIStateManager::~UIStateManager() {

}

void UIStateManager::States() {

    bool test = false;

    switch (GetCurrentState()) {

        // --- Main Menu ---
    case UIState::MainMenu: {

        ImGui::Begin("Main Menu", NULL,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove);

        ImGui::SetWindowFontScale(2.0f);
        ImGui::SetWindowSize(ImVec2(843,488));
        ImGui::SetWindowPos(ImVec2(185,100));
        std::cout << ImGui::GetWindowWidth() << "\n";

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
        }

        //Multiplayer
        std::cout << "Multiplayer txt " << ImGui::GetWindowWidth() << "\n";
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Hight Score : 00").x) * 0.5f - 25.0f);
        if (ImGui::Button("Multiplayer", ImVec2(300, 60))) {

            SetCurrentState(UIState::Multiplayer);
        }

        ImGui::End();
        break;
    }

    case UIState::InGame: {

        ImGui::Begin("In Game", NULL,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize);

        ImGui::SetWindowFontScale(2.0f);

        //TODO: Set the text position little above of the center of the full screen
        ImGui::SetWindowPos(ImVec2(500, 100));
        ImGui::SetWindowSize(ImVec2(300, 100));
        ImGui::TextColored(ImVec4(255, 0, 0, 1), "Wave 1 Begins !!");

        ImGui::End();
        break;
    }

                        //  Pause Menu 
    case UIState::Paused: {


        ImGui::SetWindowPos(ImVec2(500, 100));
        ImGui::SetWindowSize(ImVec2(300, 100));

        /*ImGui::SetWindowSize(ImVec2(843, 488));
        ImGui::SetWindowPos(ImVec2(185, 100));*/
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = 5.0f;
        style.WindowRounding = 5.0f;

        ImGui::Begin("Paused", NULL,
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize);

        ImGui::SetWindowFontScale(2.0f);

        //ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Resume").x) * 0.5f );
        if (ImGui::Button("Resume", ImVec2(300, 60))) {

            SetCurrentState(UIState::InGame);

        }

        //ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Quit").x) * 0.5f);
        if (ImGui::Button("Quit", ImVec2(300, 60))) {

            SetCurrentState(UIState::MainMenu);
        }

        if (ImGui::Button("Settings", ImVec2(300, 60))) {

            SetCurrentState(UIState::Settings);
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
        ImGui::Begin("Game Over", &isWindowOpen);

        //TODO: add win or loose based on enemies and hp. 
        ImGui::Text("You ---- ?");
        if (ImGui::Button("Return to Menu")) {
            SetCurrentState(UIState::MainMenu);
        }
        ImGui::End();
        break;
    }
    }
}


void UIStateManager::HandleInput() {
    if (ImGui::IsKeyPressed(ImGuiKey_P)) {

        if (GetCurrentState() == UIState::InGame) {

            SetCurrentState(UIState::Paused);
        }
        else {
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
}

void UIStateManager::SetCurrentState(UIState newState) {
    currentState = newState;
}

#endif // _WIN32