#pragma once

#include <imgui/imgui.h>

namespace visuals {
    struct Vector3 {
        float x, y, z;
        
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        
        Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }
        
        Vector3 operator-(const Vector3& other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        
        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
    };

    struct Vector2 {
        float x, y;
        
        Vector2() : x(0), y(0) {}
        Vector2(float x, float y) : x(x), y(y) {}
    };

    struct ViewMatrix {
        float matrix[4][4];
    };

    struct Player {
        Vector3 origin;
        Vector3 viewOffset;
        int health;
        int team;
        bool valid;
        char name[32];
    };

    // ESP Functions
    bool world_to_screen(const Vector3& world_pos, Vector2& screen_pos);
    void render_box_esp(const Player& player, const Vector2& screen_pos, float distance);
    void get_player_info(void* entity, Player& player);
    void render_esp();
    
    // Utility functions
    void get_view_matrix(ViewMatrix& matrix);
    void* get_local_player();
    void* get_entity_by_index(int index);
    int get_max_entities();
}
