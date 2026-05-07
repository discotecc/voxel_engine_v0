#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <stb_image.h>

#include "chunk.h"
#include "world.h"
#include "block_type.h"

#include "renderer.h"

/* note on logical organization of the game world
    world_pos   - world space   - integer valued    - absolute position of particular block in world
    chunk_pos   - chunk space   - integer valued    - absolute position of particular chunk in the world
    block_pos   - block space   - integer valued    - relative position of particular block within its chunk, also labeled as local space
    world_pos_f - world space   - real valued       - raw gl floating point world space coordinates... for meshing and stuff

    relational mapping... 
    world_pos = (chunk_pos * CHUNK_SIZE) + block_pos
    local_pos = world_pos mod CHUNK_SIZE
    chunk_pos = world_pos / CHUNK_SIZE                  <- use floor division here if meshing / dealing with world_pos_f
*/

int main() {
    
    spdlog::info("spdlog test. Hello...");

    World world = World(); 
    Renderer renderer = Renderer(world);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }    


    std::ios::sync_with_stdio(false);

    //console + rotating file log set up...
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logs/voxel_debug.log",          
        10 * 1024 * 1024,                //10megs max size per file
        5                                //keep 5 old files
    );

    std::cout << "yooo...." << std::endl;
    glm::ivec3 current_position(0,0,0);
    Chunk first_chunk = Chunk((current_position));
    std::cout << "printing first_chunk.blocks[5]..." << std::endl;
    std::cout << first_chunk.blocks_[5] << std::endl;

  
    world.create_grass_chunk(glm::ivec3(2,2,2));
    world.create_air_chunk(glm::ivec3(1,1,1));
    world.create_air_chunk(glm::ivec3(0,0,0));
    world.create_grass_chunk(glm::ivec3(-1,0,0));
    world.set_block(glm::ivec3(-203,30,30),Block_Type::GRASS);
    world.print_all_loaded_chunks();

    world.set_block(glm::ivec3(0,-10,0),Block_Type::DIRT);

    renderer.init(0); //mesh is static for now...initialize after world creation is prob going to be necessary.. pass 1 for wireframe
	
    std::filesystem::path current = std::filesystem::current_path();
    std::filesystem::path fs_shader_path = current / "src" / "shaders" / "mainfragmentshader.fs";
    std::cout << "Path to fragment shader... " << fs_shader_path << std::endl;


    std::cout << "World memory usage... " << world.measure_memory() << " bytes" << std::endl;
    //Game game = Game();
    //uint8_t ran = game.run();

    //game.close();

    renderer.loop();

    return 0;
}
