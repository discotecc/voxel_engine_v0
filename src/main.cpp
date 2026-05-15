#define STB_IMAGE_IMPLEMENTATION
#include <chrono>
#include <filesystem>
#include <iostream>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>

#include "block_type.h"
#include "chunk.h"
#include "renderer.h"
#include "terrain.h"
#include "world.h"

/* note on logical organization of the game world
    global_pos  - absolute position of particular block in world (at its origin)
    chunk_pos   - absolute position of particular chunk in the world
    local_pos   - relative position of particular block within its chunk, also
   labeled as block space

    relational mapping...
    global_pos = (chunk_pos * CHUNK_SIZE) + block_pos
    local_pos = world_pos mod CHUNK_SIZE
    chunk_pos = world_pos / CHUNK_SIZE                  <- use floor division
   here if meshing / dealing with world_pos_f
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

  /*
  //console + rotating file log set up...
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      "logs/voxel_debug.log",
      10 * 1024 * 1024,                //10megs max size per file
      5                                //keep 5 old files
  );
*/
  std::cout << "yooo...." << std::endl;

  world.generate_biome(glm::ivec3(1, 0, 1), FOREST, 40);

  renderer.init(0);
  renderer.mesher_.generate_mesh();

  std::filesystem::path current = std::filesystem::current_path();
  std::filesystem::path fs_shader_path =
      current / "src" / "shaders" / "mainfragmentshader.fs";
  std::cout << "Path to fragment shader... " << fs_shader_path << std::endl;
  std::cout << "World memory usage... " << world.measure_memory() << " bytes"
            << std::endl;

  auto last_log_time = std::chrono::system_clock::now();
  while (!glfwWindowShouldClose(renderer.window_)) {
    renderer.process_input();
    renderer.render();
    glfwSwapBuffers(renderer.window_);
    auto current_time = std::chrono::system_clock::now();
    current_mesh_stats = renderer.mesher_.get_mesh_stats();
    if (current_time - last_log_time >= std::chrono::seconds(10)) {
      spdlog::info("*** mesh stats ***");
      spdlog::info("vertices              : {}",
                   current_mesh_stats.total_vertices);
      spdlog::info("faces (visible sides) : {}",
                   current_mesh_stats.total_faces);
      spdlog::info("blocks processed      : {}",
                   current_mesh_stats.total_blocks_processed);
      spdlog::info("air blocks skipped    : {}",
                   current_mesh_stats.air_blocks_skipped);
      spdlog::info("chunks loaded         : {}",
                   renderer.world_.chunks_.size());
      last_log_time = std::chrono::system_clock::now();
    }
    glfwPollEvents();
  }

  return 0;
}
