#pragma once

//holds gpu-side resources for one rendered chunk
//kept separate from Chunk class so data model has no gl dependency

#include <glad/glad.h>
#include <cstdint>

struct Chunk_Mesh {
    GLuint VAO = 0;
    GLuint VBO = 0;
    int vertex_count = 0;
    bool uploaded = false;

    void init() {
        if (VAO == 0) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
        }
    }

    void destroy() {
        if (VAO) { 
            glDeleteVertexArrays(1, &VAO); 
            VAO = 0;
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        vertex_count = 0;
        uploaded = false;
    }
};