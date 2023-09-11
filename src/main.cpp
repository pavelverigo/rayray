#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <vector>
#include <string>

#include "renderer.hpp"

const char* vertexShaderSrc = R"(
    #version 460 core
    layout (location = 0) in vec2 pos;
    layout (location = 1) in vec2 texCoord;
    out vec2 texCoordFrag;
    void main() {
        gl_Position = vec4(pos, 0.0, 1.0);
        texCoordFrag = vec2(texCoord.x, 1.0 - texCoord.y);
    }
)";

const char* fragmentShaderSrc = R"(
    #version 460 core
    in vec2 texCoordFrag;
    out vec4 fragColor;
    uniform sampler2D tex;
    void main() {
        fragColor = texture(tex, texCoordFrag);
    }
)";

int main(int argc, char *argv[]) {
    const int width = 800;
    const int height = 600;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("rayray", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Create shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create VAO, VBO, EBO
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    World world;

    world.spheres.emplace_back(vec3(0, 0, -1), 0.5);
    world.spheres.emplace_back(vec3(0, -100.5, -1), 100);

    ImageRGB image(width, height);
    Renderer renderer;

    renderer.render(world, image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bool running = true;
    SDL_Event event;    

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::Begin("rayray");

        if (ImGui::Button("Render again")) {
            renderer.render(world, image);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.data);
        }

        static int item_current_idx = 0;
        static float sphere_loc[3] = {0};

        if (ImGui::BeginCombo("Objects", std::to_string(item_current_idx).c_str()))
        {
            for (int n = 0; n < world.spheres.size(); n++)
            {
                bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(std::to_string(n).c_str(), is_selected)) {
                    item_current_idx = n;

                    sphere_loc[0] = world.spheres[item_current_idx].center[0];
                    sphere_loc[1] = world.spheres[item_current_idx].center[1];
                    sphere_loc[2] = world.spheres[item_current_idx].center[2];
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        
        ImGui::InputFloat3("Sphere location", sphere_loc);

        if (ImGui::Button("Reset")) {
            sphere_loc[0] = world.spheres[item_current_idx].center[0];
            sphere_loc[1] = world.spheres[item_current_idx].center[1];
            sphere_loc[2] = world.spheres[item_current_idx].center[2];
        } 

        if (ImGui::Button("Apply")) {
            world.spheres[item_current_idx].center[0] = sphere_loc[0];
            world.spheres[item_current_idx].center[1] = sphere_loc[1];
            world.spheres[item_current_idx].center[2] = sphere_loc[2];
        }

        ImGui::SliderInt("Samples per pixel", &renderer.samples_per_pixel, 1, 16);

        {
            ImU32 begin = 0;
            ImU32 end = 16;
            ImGui::SliderScalar("Thread count", ImGuiDataType_U32, &renderer.threads, &begin, &end);
        }
        

        ImGui::End();

        ImGui::Render();
        
        // Clear the screen
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
