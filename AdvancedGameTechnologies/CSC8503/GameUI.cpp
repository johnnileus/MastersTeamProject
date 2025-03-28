#ifdef _WIN32
#include "TutorialGame.h"
#include "AssetManager.h"
#include "GameUI.h"
#include "Win32Window.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_win32.h"
#include <stb\stb_image.h>

//extern TutorialGame* g;
//extern std::unique_ptr<TutorialGame> g;
extern TutorialGame* gRaw;

GameUI::GameUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    const HWND windowHandle = NCL::Win32Code::Win32Window::windowHandle;
    ImGui_ImplWin32_InitForOpenGL(windowHandle);
    ImGui_ImplOpenGL3_Init();
    ImGui::GetIO().WantCaptureMouse = true;
    InitTexture();
}

GameUI::~GameUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void GameUI::InitTexture() {
    std::string tp1 = "../Assets/Textures/ak47.png";
    std::string tp2 = "../Assets/Textures/burst.png";
    std::string tp3 = "../Assets/Textures/shotgun.png";
    std::string tp4 = "../Assets/Textures/bg.png";

    ak      = LoadTexture(tp1.c_str());
    burst   = LoadTexture(tp2.c_str());
    shotgun = LoadTexture(tp3.c_str());
	bg      = LoadTexture(tp4.c_str());

}

GLuint GameUI::LoadTexture(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (!data) {
        std::cout << "Failed to load texture: " << filename << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data to GPU
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return textureID;
}

void GameUI::RenderTexture(string fileName) {
    std::string texturePath = "../Assets/Textures/ " + fileName;
    
    GLuint myTexture = LoadTexture(texturePath.c_str());
	
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("Background", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground);
    //Image
    ImGui::Image(
        (void*)(intptr_t)myTexture,
        ImGui::GetWindowSize(),
        ImVec2(0, 0), 
        ImVec2(1, 1),  
        ImVec4(1, 1, 1, 1), // Tint color
        ImVec4(0, 0, 0, 0)  // Border color
    );

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void GameUI::RenderAmmoCounter() {

    if (gRaw != nullptr && gRaw->getPlayer() != nullptr) {
        currentAmmo = gRaw->getPlayer()->currentWeapon->getAmmo();
        
        totalAmmo = gRaw->getPlayer()->currentWeapon->getMaxAmmo();
    }
    static bool isReloading = false;

    // Style setup
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 10));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.5f));

    // Position at bottom-left with offset
    const float DISTANCE = 20.0f;
    ImVec2 window_pos = ImVec2(DISTANCE, ImGui::GetIO().DisplaySize.y - DISTANCE);
    ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowSize(ImVec2(0, 0));

    // Create the window
    ImGui::Begin("AmmoCounter", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav);

    // Current ammo / Total ammo
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "%02d", currentAmmo);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "/");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%02d", totalAmmo);

    // Reloading indicator
    if (isReloading) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), " RELOADING");
    }

    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

void GameUI::RenderCrosshair() {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

    // Crosshair configuration
    float crosshairSize = 10.0f;
    float lineThickness = 2.0f;
    ImU32 crosshairColor = IM_COL32(255, 255, 255, 255); // White color

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();


    drawList->AddLine(
        ImVec2(center.x - crosshairSize, center.y),
        ImVec2(center.x + crosshairSize, center.y),
        crosshairColor, lineThickness);

    drawList->AddLine(
        ImVec2(center.x, center.y - crosshairSize),
        ImVec2(center.x, center.y + crosshairSize),
        crosshairColor, lineThickness);

}

void GameUI::RenderHealthBar() {
    if (gRaw->getPlayer() != nullptr){
        currentHealth = gRaw->getPlayer()->GetHealth();
    }
    
    float healthPercentage = float(currentHealth) / float(maxHealth); 

    ImVec4 healthColor = ImVec4(1.0f - healthPercentage, healthPercentage, 0.0f, 1.0f);

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(220, 50)); 

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove;

    ImGui::Begin("HealthBar", nullptr, flags);


    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Text("%s", "Health");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, healthColor);

    ImVec2 barSize = ImVec2(200, 20);
    ImGui::ProgressBar(healthPercentage, barSize, "");

    ImGui::PopStyleColor();

    ImGui::End();
}

void GameUI::RenderScoreAndTimer() {
    // Convert timer to minutes and seconds
    int minutes = gRaw->GetTimerMins();
    int seconds = gRaw->GetTimerSecs();

    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d/%d", gRaw->getPlayer()->score, gRaw->getPlayer()->GetScoreGoal());

    char timerText[32];
    snprintf(timerText, sizeof(timerText), "Time: %02d:%02d", minutes, seconds);

    // Get screen size for positioning
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    float padding = 10.0f;
    float boxWidth = 130.0f;
    float boxHeight = 35.0f;
    float spacing = 5.0f;

    // Scale default ImGui font for large text
    ImGuiIO& io = ImGui::GetIO();
    float originalFontScale = io.FontGlobalScale;
    io.FontGlobalScale = 1.8f;  

    // Position for Score Box (Top-Right)
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - boxWidth - padding, padding));
    ImGui::SetNextWindowSize(ImVec2(boxWidth, boxHeight));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f)); 
    ImGui::Begin("ScoreDisplay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::BeginChild("ScoreBackground", ImVec2(boxWidth, boxHeight), true);
    ImGui::SetCursorPosX((boxWidth - ImGui::CalcTextSize(scoreText).x) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); 
    ImGui::Text("%s", scoreText);
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::End();
    ImGui::PopStyleColor();

    // Position for Timer Box (Below Score)
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - boxWidth - padding, padding + boxHeight + spacing));
    ImGui::SetNextWindowSize(ImVec2(boxWidth, boxHeight));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f)); // Background
    ImGui::Begin("TimerDisplay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::BeginChild("TimerBackground", ImVec2(boxWidth, boxHeight), true);
    ImGui::SetCursorPosX((boxWidth - ImGui::CalcTextSize(timerText).x) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow Timer Text
    ImGui::Text("%s", timerText);
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::End();
    ImGui::PopStyleColor();

    // Reset font scale to default
    io.FontGlobalScale = originalFontScale;
}

void GameUI::RenderWeaponUI() {

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 window_pos = ImVec2(20.0f, viewport->Size.y - 100.0f); 
    ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);

    // Window flags
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar;

    // Window styling
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));

    // Set window position and size(Vertical)
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowSize(ImVec2(120, 200)); 

    if (ImGui::Begin("Weapon Selector", nullptr, window_flags))
    {
        const float child_height = 50.0f;
        const float child_width = 100.0f;

        // Weapon 1
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::BeginChild("Weapon1", ImVec2(child_width, child_height), true);
        {
            ImVec4 text_color = selected_weapon == 1 ?
                ImVec4(0.0f, 1.0f, 0.0f, 1.0f) :
                ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

            // Flash effect
            if (selected_weapon == 1 && flash_alpha > 0.0f)
            {
                ImVec2 p0 = ImGui::GetWindowPos();
                ImVec2 p1 = ImVec2(p0.x + child_width, p0.y + child_height);
                ImGui::GetWindowDrawList()->AddRectFilled(
                    p0, p1,
                    ImColor(0.0f, 1.0f, 0.0f, flash_alpha * 0.3f),
                    5.0f
                );
            }

            ImGui::PushStyleColor(ImGuiCol_Text, text_color);
            ImGui::SetCursorPosX((child_width - ImGui::CalcTextSize("Weapon 1").x) * 0.5f);
            ImGui::Text("Weapon 1");
            ImGui::PopStyleColor();

            if (selected_weapon == 1 && fade_alpha > 0.0f)
            {
                ImVec2 p0 = ImGui::GetWindowPos();
                ImVec2 p1 = ImVec2(p0.x + child_width, p0.y + child_height);
                ImGui::GetWindowDrawList()->AddRect(
                    p0, p1,
                    ImColor(0.0f, 1.0f, 0.0f, fade_alpha * 0.5f),
                    5.0f, 0, 2.0f
                );
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Spacing();

        // Weapon 2
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::BeginChild("Weapon2", ImVec2(child_width, child_height), true);
        {
            ImVec4 text_color = selected_weapon == 2 ?
                ImVec4(0.0f, 1.0f, 0.0f, 1.0f) :
                ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

            // Flash effect
            if (selected_weapon == 2 && flash_alpha > 0.0f)
            {
                ImVec2 p0 = ImGui::GetWindowPos();
                ImVec2 p1 = ImVec2(p0.x + child_width, p0.y + child_height);
                ImGui::GetWindowDrawList()->AddRectFilled(
                    p0, p1,
                    ImColor(0.0f, 1.0f, 0.0f, flash_alpha * 0.3f),
                    5.0f
                );
            }

            ImGui::PushStyleColor(ImGuiCol_Text, text_color);
            ImGui::SetCursorPosX((child_width - ImGui::CalcTextSize("Weapon 2").x) * 0.5f);
            ImGui::Text("Weapon 2");
            ImGui::PopStyleColor();

            if (selected_weapon == 2 && fade_alpha > 0.0f)
            {
                ImVec2 p0 = ImGui::GetWindowPos();
                ImVec2 p1 = ImVec2(p0.x + child_width, p0.y + child_height);
                ImGui::GetWindowDrawList()->AddRect(
                    p0, p1,
                    ImColor(0.0f, 1.0f, 0.0f, fade_alpha * 0.5f),
                    5.0f, 0, 2.0f
                );
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Spacing();

        // Weapon 3
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::BeginChild("Weapon3", ImVec2(child_width, child_height), true);
        {
            ImVec4 text_color = selected_weapon == 3 ?
                ImVec4(0.0f, 1.0f, 0.0f, 1.0f) :
                ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

            // Flash effect
            if (selected_weapon == 3 && flash_alpha > 0.0f)
            {
                ImVec2 p0 = ImGui::GetWindowPos();
                ImVec2 p1 = ImVec2(p0.x + child_width, p0.y + child_height);
                ImGui::GetWindowDrawList()->AddRectFilled(
                    p0, p1,
                    ImColor(0.0f, 1.0f, 0.0f, flash_alpha * 0.3f),
                    5.0f
                );
            }

            ImGui::PushStyleColor(ImGuiCol_Text, text_color);
            ImGui::SetCursorPosX((child_width - ImGui::CalcTextSize("Weapon 3").x) * 0.5f);
            ImGui::Text("Weapon 3");
            ImGui::PopStyleColor();

            if (selected_weapon == 3 && fade_alpha > 0.0f)
            {
                ImVec2 p0 = ImGui::GetWindowPos();
                ImVec2 p1 = ImVec2(p0.x + child_width, p0.y + child_height);
                ImGui::GetWindowDrawList()->AddRect(
                    p0, p1,
                    ImColor(0.0f, 1.0f, 0.0f, fade_alpha * 0.5f),
                    5.0f, 0, 2.0f
                );
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::End();

    // Pop styling
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    if (ImGui::IsKeyPressed(ImGuiKey_1)) {
        selected_weapon = 1;
        fade_alpha = 1.0f;
        flash_alpha = 1.0f;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_2)) {
        selected_weapon = 2;
        fade_alpha = 1.0f;
        flash_alpha = 1.0f;
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_3)) { 
        selected_weapon = 3;
        fade_alpha = 1.0f;
        flash_alpha = 1.0f;
    }

    // Update fade alphas
    const float delta = ImGui::GetIO().DeltaTime;
    if (fade_alpha > 0.0f) {
        fade_alpha -= delta * 2.0f;
        fade_alpha = std::max(0.0f, fade_alpha);
    }
    if (flash_alpha > 0.0f) {
        flash_alpha -= delta * 5.0f;
        flash_alpha = std::max(0.0f, flash_alpha);
    }
}

void GameUI::RenderUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//TODO: Make it dynamic based on window size (if needed)

	uiState.States();
	uiState.HandleInput();

    //Ammo counter 
    RenderAmmoCounter();
    RenderCrosshair();
    RenderHealthBar();
    RenderScoreAndTimer();
    RenderWeaponUI();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif // _WIN32


