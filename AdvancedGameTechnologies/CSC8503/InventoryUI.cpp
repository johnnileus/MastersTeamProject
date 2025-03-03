#pragma once
#include "InventoryUI.h"
#include "ImGui/imgui.h"

InventoryUI::InventoryUI() {}

InventoryUI::~InventoryUI() {}

//TODO: still features need to be added!

void InventoryUI::BuildInventoryUI() {

    float buttonSize = 50.0f;
    float xOffset = 4.0f;
    int c = 3;

    ImGui::Begin("Inventory System", NULL,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);

    ImGui::SetWindowFontScale(2.0f);
    ImGui::SetWindowSize(ImVec2(800, 600));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {

            ImGui::Columns(3, NULL, false);
            if (ImGui::Button("Sword", ImVec2(150, 40))) {

            }
            ImGui::NextColumn();
            if (ImGui::Button("Heal", ImVec2(150, 40))) {

            }
            ImGui::NextColumn();
            if (ImGui::Button("Ray Gun", ImVec2(150, 40))) {

            }
            ImGui::Columns(1);

        }
        ImGui::NewLine();
    }

    ImGui::End();
}