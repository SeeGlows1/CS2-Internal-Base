#pragma once
#include <d3d11.h>

#include "../sdk/interfaces/csgo_input.hpp"
#include "../sdk/interfaces/input_system.hpp"
#include "../sdk/interfaces/swap_chain_dx11.hpp"
#include "../sdk/memory_utils.hpp"

namespace interfaces {
    void create();
    void destroy();

    namespace patterns {
        inline uintptr_t find(const char* module, const char* pattern) {
            return reinterpret_cast<uintptr_t>(sdk::find_pattern(module, pattern));
        }

        inline uintptr_t find_absolute_address(uintptr_t address, int offset, int instruction_size) {
            return reinterpret_cast<uintptr_t>(sdk::resolve_absolute_rip_address(
                reinterpret_cast<uint8_t*>(address), offset, instruction_size));
        }
    }

    inline sdk::interface_csgo_input* csgo_input = nullptr;
    inline sdk::interface_input_system* input_system = nullptr;
    inline sdk::interface_swap_chain_dx11* swap_chain_dx11 = nullptr;

    inline void** local_player = nullptr;
    inline void** entity_system = nullptr;

    inline ID3D11Device* d3d11_device = nullptr;
    inline ID3D11DeviceContext* d3d11_device_context = nullptr;
    inline ID3D11RenderTargetView* d3d11_render_target_view = nullptr;

    void create_render_target();
    void destroy_render_target();

    inline HWND hwnd = nullptr;
}