#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <filesystem>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <stb_image.h>

#include "chunk.h"
#include "world.h"
#include "block_type.h"
#include "renderer.h"
#include "terrain.h"

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

    MeshStats current_mesh_stats;

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

/*  
    world.create_grass_chunk(glm::ivec3(2,2,2));
    world.create_air_chunk(glm::ivec3(1,1,1));
    world.create_air_chunk(glm::ivec3(0,0,0));
    world.create_grass_chunk(glm::ivec3(-1,0,0));
    world.set_block(glm::ivec3(-203,30,30),Block_Type::GRASS);
    world.print_all_loaded_chunks();

    world.set_block(glm::ivec3(0,-10,0),Block_Type::DIRT);
*/

/*
    int land_size = 2;	
    for (int x = 0; x < land_size; x++) {
    	for (int y = 0; y < land_size; y++) {
	    for (int z = 0; z < land_size; z++) {
	    	world.create_grass_chunk(glm::ivec3(x,y,z));
	    }
*/	
    

    Terrain terrain = Terrain(CHUNK_SIZE);
    terrain.noise_.SetFrequency(0.17);
    spdlog::info("about to generate terrain...");
    terrain.generate_terrain();
    spdlog::info("terrain generated... normalizing data");
    std::vector<float> current_terrain = terrain.get_normalized_terrain();
    for (auto i : current_terrain) {
	    std::cout << "terrain data... " << i << std::endl;
    }

    int world_size = 40;
    for (int x = 0; x < world_size; x++) {
        for (int z = 0; z < world_size; z++) {
            terrain.noise_.SetSeed(x*2 + z*384 + 6); //change seed every it or patterns will repeat
            terrain.generate_terrain();
            world.create_terrained_chunk(glm::ivec3(x,0,z), terrain.get_normalized_terrain());
        }
    }

    //world.create_terrained_chunk(glm::ivec3(3,0,3), current_terrain);

    renderer.init(0); 
	
    renderer.mesher_.generate_mesh();

    std::filesystem::path current = std::filesystem::current_path();
    std::filesystem::path fs_shader_path = current / "src" / "shaders" / "mainfragmentshader.fs";
    std::cout << "Path to fragment shader... " << fs_shader_path << std::endl;


    std::cout << "World memory usage... " << world.measure_memory() << " bytes" << std::endl;
    //Game game = Game();
    //uint8_t ran = game.run();

    //game.close();

    auto last_log_time = std::chrono::system_clock::now();
    while (!glfwWindowShouldClose(renderer.window_)) {
        renderer.process_input(renderer.window_, renderer.camera_);
        renderer.render(renderer.world_, *renderer.camera_);
        glfwSwapBuffers(renderer.window_);
        auto current_time = std::chrono::system_clock::now();
	current_mesh_stats = renderer.mesher_.get_mesh_stats();
	if (current_time - last_log_time >= std::chrono::seconds(1)){
            spdlog::info("*** mesh stats ***");
            spdlog::info("vertices              : {}", current_mesh_stats.total_vertices);
            spdlog::info("faces (visible sides) : {}", current_mesh_stats.total_faces);
    	    spdlog::info("blocks processed      : {}", current_mesh_stats.total_blocks_processed);
  	        spdlog::info("air blocks skipped    : {}", current_mesh_stats.air_blocks_skipped);
            spdlog::info("chunks loaded         : {}", renderer.world_.chunks_.size());
            last_log_time = std::chrono::system_clock::now();
	}
	glfwPollEvents();
    }

    return 0;
    }
