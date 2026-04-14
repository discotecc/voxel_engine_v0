#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <filesystem>

#include "chunk.h"
#include "world.h"
#include "block_type.h"

#include "renderer.h"

int main() {
    World world = World(); //test, constructor autofills chunk (2,2,2) with dirt

    Renderer renderer = Renderer(world);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }    

    renderer.init();

    std::cout << "yooo...." << std::endl;
    glm::ivec3 current_position(0,0,0);
    Chunk first_chunk = Chunk((current_position));
    std::cout << "printing first_chunk.blocks[5]..." << std::endl;
    std::cout << first_chunk.blocks_[5] << std::endl;

  
   
    world.create_dirt_chunk(2,2,2);
    world.create_air_chunk(1,1,1);
    world.create_air_chunk(0,0,0);
    world.set_block(-203,30,30,Block_Type::DIRT);
    world.print_all_loaded_chunks();

    world.create_dirt_chunk(1,0,0);

    std::filesystem::path current = std::filesystem::current_path();
    std::filesystem::path fs_shader_path = current / "src" / "shaders" / "mainfragmentshader.fs";
    std::cout << "Path to fragment shader... " << fs_shader_path << std::endl;


    std::cout << "World memory usage... " << world.measure_world_memory() << " bytes" << std::endl;
    //Game game = Game();
    //uint8_t ran = game.run();

    //game.close();

    renderer.loop();

    return 0;
}