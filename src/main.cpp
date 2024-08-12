#include <SFML/Audio.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SFML/Network.hpp>
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"
#include <vector>

#ifdef __linux__
#include "src/backends/linux/nmbackend.hpp"
#define BACKEND_POLL_DELAY 30000
#else
#error "No supported OS."
#endif

Uint32 timerCallback(Uint32 interval, void *p) {
    Backend* backend = static_cast<Backend*>(p);
    backend->beginDiscovery();
    return interval;
}

int main() {
    // Init SDL

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Init SDL Window
    SDL_WindowFlags wf = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Earfull", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 640, wf);
    if (window == nullptr) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_SetWindowMinimumSize(window, 480, 640);

    // Set up Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Decide which backend to use
    #ifdef __linux__
    Backend *newBackend = new NmBackend();
    #endif

    // main loop
    bool done = false;
    SDL_TimerID pollTimer = -1; // A timer to poll the backend for new peers.
    std::vector<std::string> peerList;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
                done = true;
            }
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Draw window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("Earfull", NULL, ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBackground
        );

        // Make ImGui look like a regular window
        #ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetWindowPos(viewport->GetWorkPos());
        ImGui::SetWindowSize(viewport->GetWorkSize());
        ImGui::SetWindowViewport(viewport->ID);
        #else
        ImGui::SetWindowPos(ImVec2(0.0f,0.0f));
        ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
        #endif

        ImGui::Text("Devices near you...");

        // Make calls to backend, starting with peer list

        if (pollTimer == -1) {
            newBackend->beginDiscovery();
            pollTimer = SDL_AddTimer(BACKEND_POLL_DELAY,
                timerCallback,
                newBackend);

        }

        peerList = newBackend->GetPeers();
        ImGui::BeginChild("Scrolling");
        if ((int)peerList.size() == 0) {
            ImGui::Text("No devices found.");
        } else {
            for (int i = 0; i <(int) peerList.size(); i ++) {
                ImGui::Text("%s", peerList[i].c_str());
            }
        }
        ImGui::EndChild();



        ImGui::End();
        ImGui::PopStyleVar(2);

        // Render window
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    delete(newBackend);

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
