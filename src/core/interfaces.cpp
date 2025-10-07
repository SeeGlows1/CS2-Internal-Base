#include "interfaces.hpp"
#include <stdexcept>
#include <format>
#include <iostream>

namespace interfaces {
    template <class T>
    T* capture_interface(const char* module_name, const char* interface_name) {
        const HMODULE module_handle = GetModuleHandleA(module_name);
        if (module_handle == nullptr) {
            throw std::runtime_error(
                std::format("Failed to get handle for module \"{}\"", module_name));
        }

        using create_interface_fn = T * (*)(const char*, int*);
        const auto create_interface =
            reinterpret_cast<create_interface_fn>(GetProcAddress(module_handle, "CreateInterface"));
        if (create_interface == nullptr) {
            throw std::runtime_error(std::format(
                "Failed to get CreateInterface for module \"{}\"", module_name));
        }
        T* interface_ptr = create_interface(interface_name, nullptr);
        if (interface_ptr == nullptr) {
            throw std::runtime_error(std::format(
                "Failed to capture interface \"{}\" from \"{}\"",
                interface_name, module_name));
        }
        return interface_ptr;
    }

    static void create_d3d11_resources() {
        {
            uintptr_t address = patterns::find("rendersystemdx11.dll",
                "48 89 2D ? ? ? ? 48 C7 05");

            swap_chain_dx11 = **reinterpret_cast<sdk::interface_swap_chain_dx11***>(
                patterns::find_absolute_address(address, 3, 7));

            if (swap_chain_dx11 == nullptr) {
                throw std::runtime_error("Failed to capture Swap Chain");
            }
        }

        if (swap_chain_dx11->swap_chain == nullptr) {
            throw std::runtime_error("Swap Chain padding is outdated.");
        }

        IDXGISwapChain* swap_chain = swap_chain_dx11->swap_chain;

        if (FAILED(swap_chain->GetDevice(__uuidof(ID3D11Device),
            reinterpret_cast<void**>(&d3d11_device)))) {
            throw std::runtime_error("Failed to get D3D11 Device from Swap Chain");
        }

        if (d3d11_device == nullptr) {
            throw std::runtime_error("D3D11 Device is null");
        }

        d3d11_device->GetImmediateContext(&d3d11_device_context);

        if (d3d11_device_context == nullptr) {
            throw std::runtime_error("D3D11 Device Context is null");
        }

        {
            DXGI_SWAP_CHAIN_DESC swap_chain_desc;
            if (FAILED(swap_chain->GetDesc(&swap_chain_desc))) {
                throw std::runtime_error("Failed to get Swap Chain description");
            }

            hwnd = swap_chain_desc.OutputWindow;

            if (hwnd == nullptr) {
                throw std::runtime_error("HWND is null");
            }
        }

        create_render_target();
    }

    void create() {
        std::cout << "Initializing interfaces...\n";

        // Capture interfaces
        {
            uintptr_t address = patterns::find("client.dll",
                "48 8B 0D ? ? ? ? 4C 8D 8F ? ? ? ? 45 33 F6");

            csgo_input = *reinterpret_cast<sdk::interface_csgo_input**>(
                patterns::find_absolute_address(address, 3, 7));

            if (csgo_input == nullptr) {
                throw std::runtime_error("Failed to capture CInput");
            }
            std::cout << "CInput captured successfully\n";
        }

        input_system = capture_interface<sdk::interface_input_system>("inputsystem.dll",
            "InputSystemVersion001");
        std::cout << "InputSystem captured successfully\n";

        // Capture Local Player
        {
            uintptr_t address = patterns::find("client.dll",
                "4C 8D 05 ? ? ? ? 48 8D 54 24 ? 48 8B 08 48 89 8F ? ? ? ? 48 8B CB 48 8B 03 FF 50 ? 4C 8B 13 4C 8D 05 ? ? ? ? 41 B9 ? ? ? ? 48 8D 54 24 ? 48 8B CB 48 8B 00 48 89 44 24 ? 44 88 4C 24 ? 41 FF 52 ? 48 8B 44 24 ? 4C 8D 05");

            interfaces::local_player = reinterpret_cast<void**>(
                patterns::find_absolute_address(address, 3, 7));

            if (interfaces::local_player == nullptr) {
                throw std::runtime_error("Failed to capture Local Player");
            }
            std::cout << "Local Player captured successfully\n";
        }

        // Capture Entity System
        {
            uintptr_t address = patterns::find("client.dll",
                "48 8D 05 ? ? ? ? 48 C7 05 ? ? ? ? ? ? ? ? 48 89 05 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C3");

            interfaces::entity_system = reinterpret_cast<void**>(
                patterns::find_absolute_address(address, 3, 7));

            if (interfaces::entity_system == nullptr) {
                throw std::runtime_error("Failed to capture Entity System");
            }
            std::cout << "Entity System captured successfully\n";
        }

        // Create D3D11 resources
        create_d3d11_resources();
        std::cout << "D3D11 resources created successfully\n";
    }

    void destroy() {
        std::cout << "Shutting down interfaces...\n";

        destroy_render_target();

        if (d3d11_render_target_view != nullptr) {
            d3d11_render_target_view->Release();
            d3d11_render_target_view = nullptr;
        }
        if (d3d11_device_context != nullptr) {
            d3d11_device_context->Release();
            d3d11_device_context = nullptr;
        }
        if (d3d11_device != nullptr) {
            d3d11_device->Release();
            d3d11_device = nullptr;
        }
        swap_chain_dx11 = nullptr;
        input_system = nullptr;
        csgo_input = nullptr;
        interfaces::local_player = nullptr;
        interfaces::entity_system = nullptr;
        hwnd = nullptr;

        std::cout << "Interfaces shutdown complete\n";
    }

    void create_render_target() {
        if (!d3d11_device || !swap_chain_dx11 || !swap_chain_dx11->swap_chain) {
            throw std::runtime_error("D3D11 Device or Swap Chain is null");
        }
        ID3D11Texture2D* back_buffer = nullptr;
        if (FAILED(swap_chain_dx11->swap_chain->GetBuffer(
            0, __uuidof(ID3D11Texture2D),
            reinterpret_cast<void**>(&back_buffer)))) {
            throw std::runtime_error("Failed to get back buffer from Swap Chain");
        }
        if (back_buffer == nullptr) {
            throw std::runtime_error("Back buffer is null");
        }

        if (FAILED(d3d11_device->CreateRenderTargetView(back_buffer, nullptr,
            &d3d11_render_target_view))) {
            back_buffer->Release();
            throw std::runtime_error("Failed to create render target view for back buffer");
        }

        back_buffer->Release();

        if (d3d11_render_target_view == nullptr) {
            throw std::runtime_error("Render target view is null");
        }
    }

    void destroy_render_target() {
        if (d3d11_render_target_view != nullptr) {
            d3d11_render_target_view->Release();
            d3d11_render_target_view = nullptr;
        }
    }
}