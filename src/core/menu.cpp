#include "menu.hpp"

#include "globals.h"
#include "hooks.hpp"
#include "interfaces.hpp"
#include "../features/esp/visuals.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include <stdexcept>

static bool espEnabled = false;
static bool boxEspEnabled = false;
static int currentTab = 0; // 0=Aimbot, 1=Players, 2=World, 3=Inventory, 4=Config
static bool aimbotEnabled = false;
static float aimbotFov = 90.0f;
static float aimbotSmooth = 50.0f;
static bool triggerbotEnabled = false;
static float triggerbotDelay = 50.0f;
static bool triggerbotHeadOnly = false;
static bool showNames = false;
static bool showHealth = false;
static bool showDistance = false;
static bool chamsEnabled = false;
static int chamsType = 0; // 0=Normal, 1=Flat, 2=Metallic, 3=Glow
static bool noFlash = false;
static bool noSmoke = false;
static bool wireframe = false;
static bool autoReload = false;
static bool fastSwitch = false;
static bool noRecoil = false;

static WNDPROC original_wndproc = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hook_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_KEYDOWN && wparam == VK_INSERT) {
        globals::menu_opened = !globals::menu_opened;

        hooks::original_set_relative_mouse_mode(
            interfaces::input_system, globals::menu_opened ? false : globals::relative_mouse_mode);
    }

    if (globals::menu_opened) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
        return true;
    }
    else {
        return CallWindowProc(original_wndproc, hwnd, msg, wparam, lparam);
    }
}

namespace menu {

    void create() {
        if (!interfaces::d3d11_device || !interfaces::d3d11_device_context || !interfaces::hwnd) {
            throw std::runtime_error("failed to create menu: invalid interfaces");
        }

        original_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(
            interfaces::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hook_wndproc)));

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        if (!ImGui_ImplWin32_Init(interfaces::hwnd)) {
            throw std::runtime_error("failed to initialize imgui win32 implementation");
        }

        if (!ImGui_ImplDX11_Init(interfaces::d3d11_device, interfaces::d3d11_device_context)) {
            throw std::runtime_error("failed to initialize imgui dx11 implementation");
        }
    }

    void destroy() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (original_wndproc) {
            SetWindowLongPtrA(interfaces::hwnd, GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(original_wndproc));
            original_wndproc = nullptr;
        }
    }

    void render() {
if (!globals::menu_opened) return;

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.97f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.45f, 0.47f, 0.49f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.07f, 0.08f, 0.09f, 0.97f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.11f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.09f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_Border]                = ImVec4(0.18f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.18f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.22f, 0.23f, 0.24f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.33f, 0.70f, 0.98f, 0.80f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.33f, 0.70f, 0.98f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.94f, 0.96f, 0.97f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.14f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.20f, 0.21f, 0.23f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.16f, 0.38f, 0.81f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.16f, 0.17f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.19f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.19f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_Separator]             = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.23f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.10f, 0.44f, 0.99f, 0.90f);

    style.WindowPadding     = ImVec2(18, 16);
    style.FramePadding      = ImVec2(9, 4);
    style.ItemSpacing       = ImVec2(14, 10);
    style.ItemInnerSpacing  = ImVec2(8, 7);
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.WindowRounding    = 6.0f;
    style.FrameRounding     = 4.0f;
    style.TabRounding       = 3.5f;
    style.GrabRounding      = 3.0f;
    style.TabBorderSize     = 1.0f;
    style.ScrollbarRounding = 6.0f;

    ImGui::SetNextWindowSize(ImVec2(580, 390), ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
    ImGui::Begin("Menu", nullptr, window_flags);

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_NoTooltip | ImGuiTabBarFlags_FittingPolicyResizeDown)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            currentTab = 0;
            ImGui::Spacing();
            
            ImGui::Columns(2, "AimbotColumns", false);
            
            ImGui::Text("Aimbot Settings");
            ImGui::Separator();
            ImGui::Checkbox("Enable Aimbot", &aimbotEnabled);
            
            ImGui::PushItemWidth(120.0f);
            ImGui::SliderFloat("FOV", &aimbotFov, 1.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("Smooth", &aimbotSmooth, 1.0f, 30.0f, "%.0f");
            ImGui::PopItemWidth();
            
            ImGui::NextColumn();
            
            ImGui::Text("Triggerbot");
            ImGui::Checkbox("Enable Triggerbot", &triggerbotEnabled);
            if (triggerbotEnabled) {
                ImGui::PushItemWidth(120.0f);
                ImGui::SliderFloat("Delay", &triggerbotDelay, 1.0f, 100.0f, "%.0f");
                ImGui::PopItemWidth();
                ImGui::Checkbox("Head Only", &triggerbotHeadOnly);
            }
            
            ImGui::Columns(1);
            
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();
            float line_x = window_pos.x + window_size.x / 2.0f - 19.0f;
            float line_start_y = window_pos.y + 80.0f;
            float line_end_y = window_pos.y + 280.0f;
            draw_list->AddLine(ImVec2(line_x, line_start_y), ImVec2(line_x, line_end_y), ImGui::GetColorU32(ImGuiCol_Separator), 1.0f);
            
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Players")) {
            currentTab = 1;
            ImGui::Spacing();
            
            ImGui::Columns(2, "PlayerColumns", false);
            
            ImGui::Text("Player ESP");
            ImGui::Separator();
            ImGui::Checkbox("Enable ESP", &boxEspEnabled);
            ImGui::Checkbox("Show Names", &showNames);
            ImGui::Checkbox("Show Health", &showHealth);
            ImGui::Checkbox("Show Distance", &showDistance);
            
            ImGui::NextColumn();
            
            ImGui::Text("Chams");
            ImGui::Checkbox("Enable Chams", &chamsEnabled);
            if (chamsEnabled) {
                const char* chamsTypes[] = { "Normal", "Flat", "Metallic", "Glow" };
                ImGui::PushItemWidth(120.0f);
                ImGui::Combo("Chams Type", &chamsType, chamsTypes, IM_ARRAYSIZE(chamsTypes));
                ImGui::PopItemWidth();
            }
            
            ImGui::Columns(1);
            
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 window_size = ImGui::GetWindowSize();
            float line_x = window_pos.x + window_size.x / 2.0f - 19.0f;
            float line_start_y = window_pos.y + 80.0f;
            float line_end_y = window_pos.y + 280.0f;
            draw_list->AddLine(ImVec2(line_x, line_start_y), ImVec2(line_x, line_end_y), ImGui::GetColorU32(ImGuiCol_Separator), 1.0f);
            
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World")) {
            currentTab = 2;
            ImGui::Spacing();
            ImGui::Text("World Settings");
            ImGui::Separator();
            ImGui::Checkbox("No Flash", &noFlash);
            ImGui::Checkbox("No Smoke", &noSmoke);
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Inventory")) {
            currentTab = 3;
            ImGui::Spacing();
            ImGui::Text("Inventory");
            ImGui::Separator();
            ImGui::Checkbox("Auto Reload", &autoReload);
            ImGui::Checkbox("Fast Switch", &fastSwitch);
            ImGui::Checkbox("No Recoil", &noRecoil);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            currentTab = 4;
            ImGui::Spacing();
            ImGui::Text("Configuration");
            ImGui::Separator();
            if (ImGui::Button("Save Config")) {
            
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Config")) {
            
            }
            ImGui::Spacing();
            ImGui::Text("Menu Key: INSERT");
            ImGui::Text("Status: %s", globals::menu_opened ? "Open" : "Closed");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    if (boxEspEnabled) {
        visuals::render_esp();
    }
    
    ImGui::End();
}

}
