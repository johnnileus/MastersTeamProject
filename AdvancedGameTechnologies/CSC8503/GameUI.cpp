#ifdef _WIN32
#include "TutorialGame.h"
#include "AssetManager.h"
#include "GameUI.h"
#include "Win32Window.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_win32.h"

extern TutorialGame* g;
extern AssetManager* a;

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

}

GameUI::~GameUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void GameUI::RenderAmmoCounter() {

    if (g->getPlayer() != nullptr) {
        currentAmmo = g->getPlayer()->myWeapon->getAmmo();
        totalAmmo = g->getPlayer()->myWeapon->getMaxAmmo();
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

    // Get the foreground draw list so the crosshair appears on top of other UI elements
    //ImDrawList* drawList = ImGui::GetForegroundDrawList();
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
    if (g->getPlayer() != nullptr){
        currentHealth = g->getPlayer()->GetHealth();
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
    //ImGui::Text("Health");

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, healthColor);

    ImVec2 barSize = ImVec2(200, 20);
    ImGui::ProgressBar(healthPercentage, barSize, "");

    ImGui::PopStyleColor();

    ImGui::End();
}

void GameUI::RenderScoreAndTimer() {
    // Convert timer to minutes and seconds
    int timer = 300;

    int minutes = static_cast<int>(timer) / 60;
    int seconds = static_cast<int>(timer) % 60;

    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", g->getPlayer()->score);

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
    io.FontGlobalScale = 1.8f;  // Increase font size

    // Position for Score Box (Top-Right)
    ImGui::SetNextWindowPos(ImVec2(windowSize.x - boxWidth - padding, padding));
    ImGui::SetNextWindowSize(ImVec2(boxWidth, boxHeight));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f)); // Semi-transparent background
    ImGui::Begin("ScoreDisplay", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::BeginChild("ScoreBackground", ImVec2(boxWidth, boxHeight), true);
    ImGui::SetCursorPosX((boxWidth - ImGui::CalcTextSize(scoreText).x) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green Score Text
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

void GameUI::RenderWeaponUI()
{
    //if(glIsTexture())
    //testing if i can render image from Imgui
    //if (AssetManager::Instance().floorTex != nullptr) {
    //    std::cout << AssetManager::Instance().floorTex->GetAssetID() << std::endl;
    //    GLuint myTexture = AssetManager::Instance().floorTex->GetAssetID();
    //    ImGui::Begin("Weapon UI");
    //    ImVec2 imageSize(128, 128); // Set the size of the image
    //    ImGui::Image((void*)(intptr_t)myTexture, imageSize);

    //    ImGui::End();
    //}
    //else {
    //    std::cout << " IMAGE NOT YET LOADED" << std::endl;




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


