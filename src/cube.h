#pragma once

#include <vector>

class Cube {
public:
   
    std::vector<float> cubeVertices;
    std::vector<unsigned int> cubeIndices;

	Cube() {
        cubeVertices = {
            // Front face
            -0.5f, -0.5f,  0.5f,  // bottom left
             0.5f, -0.5f,  0.5f,  // bottom right
             0.5f,  0.5f,  0.5f,  // top right
            -0.5f,  0.5f,  0.5f,  // top left

            // Back face
            -0.5f, -0.5f, -0.5f,  // bottom left
             0.5f, -0.5f, -0.5f,  // bottom right
             0.5f,  0.5f, -0.5f,  // top right
            -0.5f,  0.5f, -0.5f,  // top left

            // Right face
             0.5f, -0.5f,  0.5f,  // bottom front
             0.5f, -0.5f, -0.5f,  // bottom back
             0.5f,  0.5f, -0.5f,  // top back
             0.5f,  0.5f,  0.5f,  // top front

             // Left face
             -0.5f, -0.5f,  0.5f,  // bottom front
             -0.5f, -0.5f, -0.5f,  // bottom back
             -0.5f,  0.5f, -0.5f,  // top back
             -0.5f,  0.5f,  0.5f,  // top front

             // Top face
             -0.5f,  0.5f,  0.5f,  // front left
              0.5f,  0.5f,  0.5f,  // front right
              0.5f,  0.5f, -0.5f,  // back right
             -0.5f,  0.5f, -0.5f,  // back left

             // Bottom face
             -0.5f, -0.5f,  0.5f,  // front left
              0.5f, -0.5f,  0.5f,  // front right
              0.5f, -0.5f, -0.5f,  // back right
             -0.5f, -0.5f, -0.5f   // back left
        };
        // Indices to define triangles (for glDrawElements)
        cubeIndices = {
            // Front face
            0, 1, 2,
            2, 3, 0,

            // Back face
            4, 6, 5,
            6, 4, 7,

            // Right face
            8, 9, 10,
            10, 11, 8,

            // Left face
            12, 13, 14,
            14, 15, 12,

            // Top face
            16, 17, 18,
            18, 19, 16,

            // Bottom face
            20, 21, 22,
            22, 23, 20
        };
	}

    std::vector<float> getVertices() {
        return cubeVertices;
    }

    std::vector<unsigned int> getIndices() {
        return cubeIndices;
    }
};
