#include <iostream>
#include <string>
#include <assert.h>
using namespace std;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

// Tamanho do tile (proporção 2:1)
const int TILE_WIDTH = 64;
const int TILE_HEIGHT = 32;

const int MAP_WIDTH = 3;
const int MAP_HEIGHT = 3;

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 4},
    {4, 1, 4},
    {4, 4, 1}
};

int selectedX = 1, selectedY = 1;

GLuint texture;

bool loadTexture(const char* path) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);
    if (!data) {
        std::cerr << "Erro ao carregar imagem: " << path << std::endl;
        return false;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);    

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return true;
}

void drawTile(int i, int j, int tileIndex, bool isSelected) {
    // Centraliza o mapa no meio da tela (800x600)
    int screenX = 400 + (i - j) * (TILE_WIDTH / 2);
    int screenY = 300 - (i + j) * (TILE_HEIGHT / 2);

    float u0 = tileIndex / 7.0f;
    float u1 = (tileIndex + 1) / 7.0f;

    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        if (isSelected) glColor3f(1.0f, 1.0f, 1.0f);
        else glColor3f(0.8f, 0.8f, 0.8f);

        glTexCoord2f((u0 + u1) / 2, 1.0f);                glVertex2f(screenX, screenY + TILE_HEIGHT / 2); // Top
        glTexCoord2f(u1, 0.5f);                           glVertex2f(screenX + TILE_WIDTH / 2, screenY);  // Right
        glTexCoord2f((u0 + u1) / 2, 0.0f);                glVertex2f(screenX, screenY - TILE_HEIGHT / 2); // Bottom
        glTexCoord2f(u0, 0.5f);                           glVertex2f(screenX - TILE_WIDTH / 2, screenY);  // Left
    glEnd();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_W: if (selectedY > 0) selectedY--; break;             // N
        case GLFW_KEY_S: if (selectedY < MAP_HEIGHT - 1) selectedY++; break; // S
        case GLFW_KEY_A: if (selectedX > 0) selectedX--; break;             // W
        case GLFW_KEY_D: if (selectedX < MAP_WIDTH - 1) selectedX++; break; // E

        case GLFW_KEY_Q: // NW
            if (selectedX > 0 && selectedY > 0) { selectedX--; selectedY--; }
            break;
        case GLFW_KEY_E: // NE
            if (selectedX < MAP_WIDTH - 1 && selectedY > 0) { selectedX++; selectedY--; }
            break;
        case GLFW_KEY_Z: // SW
            if (selectedX > 0 && selectedY < MAP_HEIGHT - 1) { selectedX--; selectedY++; }
            break;
        case GLFW_KEY_C: // SE
            if (selectedX < MAP_WIDTH - 1 && selectedY < MAP_HEIGHT - 1) { selectedX++; selectedY++; }
            break;
    }
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Tilemap Isometrico - Diamond", NULL, NULL);
    if (!window) {
        std::cerr << "Erro ao criar janela GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetKeyCallback(window, key_callback);

    // Projeção ortográfica com origem no canto inferior esquerdo
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!loadTexture("../assets/tilesets/tilesetIso.png")) {
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        for (int j = 0; j < MAP_HEIGHT; ++j) {
            for (int i = 0; i < MAP_WIDTH; ++i) {
                bool isSelected = (i == selectedX && j == selectedY);
                int tileIndex = isSelected ? 6 : map[j][i]; // 6 = tile rosa
                drawTile(i, j, tileIndex, isSelected);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}