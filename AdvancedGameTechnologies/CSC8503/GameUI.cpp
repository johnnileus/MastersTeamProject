#include "GameUI.h"
#include "Win32Window.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_win32.h"

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

	//std::cout << "window : " << windowHandle << "\n";


}

GameUI::~GameUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GameUI::AmmoCounter() {

    static int currentAmmo = 25;
    static int totalAmmo = 100;
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

void GameUI::Crosshair() {
    
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


void GameUI::RenderUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//TODO: Make it dynamic based on window size (if needed)

	uiState.States();
	uiState.HandleInput();

    //Ammo counter 
    AmmoCounter();
    Crosshair();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

