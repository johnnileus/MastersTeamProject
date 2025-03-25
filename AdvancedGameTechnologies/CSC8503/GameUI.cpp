#ifdef _WIN32
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


void GameUI::RenderUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//TODO: Make it dynamic based on window size (if needed)

	uiState.States();
	uiState.HandleInput();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif // _WIN32


