#include "visuals.hpp"
#include "../../core/interfaces.hpp"
#include "../../core/Offsets.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <windows.h>

namespace visuals {

    void get_view_matrix(ViewMatrix& matrix) {
        uintptr_t view_matrix_addr = reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll")) + Offsets::ViewMatrix;
        memcpy(&matrix, reinterpret_cast<void*>(view_matrix_addr), sizeof(ViewMatrix));
    }

    void* get_local_player() {
        if (!interfaces::local_player || !*interfaces::local_player) {
            return nullptr;
        }
        return *interfaces::local_player;
    }

    void* get_entity_by_index(int index) {
        if (!interfaces::entity_system || !*interfaces::entity_system) {
            return nullptr;
        }
        
        // Entity system structure: [0] = entity list pointer
        void** entity_list = reinterpret_cast<void**>(*interfaces::entity_system);
        if (!entity_list) {
            return nullptr;
        }
        
        // Each entity is 8 bytes (64-bit pointer)
        return entity_list[index];
    }

    int get_max_entities() {
        return 64; // Standard CS2 entity limit
    }

    bool world_to_screen(const Vector3& world_pos, Vector2& screen_pos) {
        ViewMatrix matrix;
        get_view_matrix(matrix);
        
        float w = matrix.matrix[3][0] * world_pos.x + 
                  matrix.matrix[3][1] * world_pos.y + 
                  matrix.matrix[3][2] * world_pos.z + 
                  matrix.matrix[3][3];
        
        if (w < 0.01f) {
            return false; // Behind camera
        }
        
        float x = matrix.matrix[0][0] * world_pos.x + 
                  matrix.matrix[0][1] * world_pos.y + 
                  matrix.matrix[0][2] * world_pos.z + 
                  matrix.matrix[0][3];
        
        float y = matrix.matrix[1][0] * world_pos.x + 
                  matrix.matrix[1][1] * world_pos.y + 
                  matrix.matrix[1][2] * world_pos.z + 
                  matrix.matrix[1][3];
        
        // Convert to screen coordinates
        float screen_x = (1920.0f / 2.0f) + (1920.0f / 2.0f) * x / w;
        float screen_y = (1080.0f / 2.0f) - (1080.0f / 2.0f) * y / w;
        
        screen_pos = Vector2(screen_x, screen_y);
        return true;
    }

    void get_player_info(void* entity, Player& player) {
        if (!entity) {
            player.valid = false;
            return;
        }
        
        uintptr_t entity_addr = reinterpret_cast<uintptr_t>(entity);
        
        // Read player data using offsets
        player.origin = *reinterpret_cast<Vector3*>(entity_addr + Offsets::OldOrigin);
        player.viewOffset = *reinterpret_cast<Vector3*>(entity_addr + Offsets::ViewOffset);
        player.health = *reinterpret_cast<int*>(entity_addr + Offsets::Health);
        player.team = *reinterpret_cast<int*>(entity_addr + Offsets::Team);
        
        // Read player name
        const char* name_ptr = reinterpret_cast<const char*>(entity_addr + Offsets::Name);
        strncpy_s(player.name, sizeof(player.name), name_ptr, sizeof(player.name) - 1);
        player.name[sizeof(player.name) - 1] = '\0';
        
        player.valid = (player.health > 0 && player.health <= 100);
    }

    void render_box_esp(const Player& player, const Vector2& screen_pos, float distance) {
        if (!player.valid) return;
        
        // Calculate box size based on distance
        float box_height = 2000.0f / distance;
        float box_width = box_height * 0.6f; // Aspect ratio
        
        // Clamp box size
        if (box_height < 10.0f) box_height = 10.0f;
        if (box_height > 200.0f) box_height = 200.0f;
        if (box_width < 6.0f) box_width = 6.0f;
        if (box_width > 120.0f) box_width = 120.0f;
        
        // Calculate box corners
        ImVec2 top_left(screen_pos.x - box_width / 2, screen_pos.y - box_height);
        ImVec2 bottom_right(screen_pos.x + box_width / 2, screen_pos.y);
        
        // Choose color based on team
        ImU32 color;
        if (player.team == 2) { // Terrorist
            color = IM_COL32(255, 100, 100, 255); // Red
        } else if (player.team == 3) { // Counter-Terrorist
            color = IM_COL32(100, 150, 255, 255); // Blue
        } else {
            color = IM_COL32(255, 255, 255, 255); // White for unknown
        }
        
        // Draw box outline
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        draw_list->AddRect(top_left, bottom_right, color, 0.0f, 0, 2.0f);
        
        // Draw health bar
        if (player.health > 0) {
            float health_percent = static_cast<float>(player.health) / 100.0f;
            ImVec2 health_bar_top(top_left.x - 8, top_left.y);
            ImVec2 health_bar_bottom(top_left.x - 4, bottom_right.y);
            
            // Health bar background (red)
            draw_list->AddRectFilled(health_bar_top, health_bar_bottom, IM_COL32(50, 50, 50, 255));
            
            // Health bar fill (green to red based on health)
            float health_height = (bottom_right.y - top_left.y) * health_percent;
            ImVec2 health_fill_bottom(health_bar_bottom.x, health_bar_bottom.y - health_height);
            
            ImU32 health_color = IM_COL32(
                static_cast<int>(255 * (1.0f - health_percent)),
                static_cast<int>(255 * health_percent),
                0,
                255
            );
            draw_list->AddRectFilled(health_bar_top, health_fill_bottom, health_color);
        }
        
        // Draw player name
        if (strlen(player.name) > 0) {
            ImVec2 name_pos(screen_pos.x, top_left.y - 20);
            draw_list->AddText(name_pos, color, player.name);
        }
        
        // Draw distance
        char distance_text[32];
        sprintf_s(distance_text, "%.0fm", distance);
        ImVec2 distance_pos(screen_pos.x, bottom_right.y + 5);
        draw_list->AddText(distance_pos, color, distance_text);
    }

    void render_esp() {
        void* local_player_ptr = get_local_player();
        if (!local_player_ptr) return;
        
        Player local_player;
        get_player_info(local_player_ptr, local_player);
        if (!local_player.valid) return;
        
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        
        // Draw crosshair
        ImVec2 screen_center(1920.0f / 2.0f, 1080.0f / 2.0f);
        draw_list->AddLine(
            ImVec2(screen_center.x - 10, screen_center.y),
            ImVec2(screen_center.x + 10, screen_center.y),
            IM_COL32(255, 255, 255, 255),
            2.0f
        );
        draw_list->AddLine(
            ImVec2(screen_center.x, screen_center.y - 10),
            ImVec2(screen_center.x, screen_center.y + 10),
            IM_COL32(255, 255, 255, 255),
            2.0f
        );
        
        // Loop through entities
        for (int i = 1; i < get_max_entities(); i++) {
            void* entity = get_entity_by_index(i);
            if (!entity || entity == local_player_ptr) continue;
            
            Player player;
            get_player_info(entity, player);
            if (!player.valid) continue;
            
            // Skip same team players (optional - you can remove this)
            if (player.team == local_player.team) continue;
            
            // Calculate distance
            Vector3 delta = local_player.origin - player.origin;
            float distance = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z) / 100.0f; // Convert to meters
            
            // Convert world position to screen position
            Vector2 screen_pos;
            if (world_to_screen(player.origin + player.viewOffset, screen_pos)) {
                render_box_esp(player, screen_pos, distance);
            }
        }
    }
}
