// openGL setup, shader management, drawing all visible chunks, frustrum
// culling, sorting
#pragma once
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "mesher.h"
#include "player.h"
#include "shader.h"
#include "skybox.h"
#include "world.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

bool first_mouse = true;
float last_x = 400, last_y = 300;

unsigned int load_texture(char const *path);
unsigned int load_cubemap(std::vector<std::filesystem::path> faces);

class Renderer {
public:
  unsigned int SCR_WIDTH_ = 1920;
  unsigned int SCR_HEIGHT_ = 1080;
  GLFWwindow *window_;
  Player *player_;
  World &world_;
  Mesher mesher_;

  Renderer(World &world);
  ~Renderer();

  void init(int wf);
  void init_skybox();
  void draw_skybox();
  void render();
  void upload_chunk_mesh(Chunk &chunk);
  void cleanup();
  void loop();
  void process_input();

  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

private:
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<Shader> skybox_shader_;
  std::vector<const Chunk *> visible_chunks_;
  unsigned int texture_;
  unsigned int skybox_cubemap_;
  unsigned int skybox_VAO_, skybox_VBO_;

  static Renderer *instance_;

  // shader file paths
  std::filesystem::path fs_path_;
  std::filesystem::path vs_path_;
  std::filesystem::path skybox_fs_path_;
  std::filesystem::path skybox_vs_path_;

  // texture file paths
  std::filesystem::path tex_path_;
  std::vector<std::filesystem::path> skybox_tex_path_;

  // helpers
  void start_new_frame();
  void draw_chunk(const Chunk &chunk);

  // updated every frame...
  glm::mat4 view_matrix_;
  glm::mat4 projection_matrix_;
  glm::mat4 model_matrix_;
};

Renderer::Renderer(World &world) : world_(world), mesher_(world) {

  instance_ = this;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window_ = glfwCreateWindow(SCR_WIDTH_, SCR_HEIGHT_, "game", NULL, NULL);
  if (window_ == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  /*
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return;
      }
  */
  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

  // camera_ = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
  glfwSetCursorPosCallback(window_, mouse_callback);

  player_ = new Player();
}

Renderer::~Renderer() {
  glDeleteVertexArrays(1, &skybox_VAO_);
  glDeleteBuffers(1, &skybox_VBO_);
  glfwDestroyWindow(window_);
  glfwTerminate();
  // delete camera_;
}

void Renderer::init(int wf) {
  std::filesystem::path current = std::filesystem::current_path();
  fs_path_ = current / "src" / "shaders" / "mainfragmentshader.fs";
  vs_path_ = current / "src" / "shaders" / "mainvertexshader.vs";

  skybox_fs_path_ = current / "src" / "shaders" / "skybox.fs";
  skybox_vs_path_ = current / "src" / "shaders" / "skybox.vs";

  skybox_tex_path_ = {current / "textures" / "Standard-Cube-Map" / "px.png",
                      current / "textures" / "Standard-Cube-Map" / "nx.png",
                      current / "textures" / "Standard-Cube-Map" / "py.png",
                      current / "textures" / "Standard-Cube-Map" / "ny.png",
                      current / "textures" / "Standard-Cube-Map" / "pz.png",
                      current / "textures" / "Standard-Cube-Map" / "nz.png"};

  shader_ = std::make_unique<Shader>(vs_path_.c_str(), fs_path_.c_str());
  shader_->use();
  shader_->set_int("texture1", 0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  if (wf) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  tex_path_ = current / "textures" / "mc_basic_texture_atlas.png";
  texture_ = load_texture(tex_path_.c_str());

  spdlog::info("renderer::init  - about to call load_cubemap...");
  skybox_cubemap_ = load_cubemap(skybox_tex_path_);
  spdlog::info("renderer::init  - left load_cubemap...");

  skybox_shader_ = std::make_unique<Shader>(skybox_vs_path_.c_str(),
                                            skybox_fs_path_.c_str());
  spdlog::info("renderer::init  - about to enter init_skybox....");
  init_skybox();
  spdlog::info("renderer::init  - about to use skybox_shader...");
  skybox_shader_->use();
  skybox_shader_->set_int("skybox", 0);
  spdlog::info("renderer::init  - about to initialize mesher....");
  mesher_.init();
  spdlog::info("renderer::init  - just initialized mesher...");
}

void Renderer::init_skybox() {
  spdlog::info("renderer::init_skybox  - initializing skybox...");

  glGenVertexArrays(1, &skybox_VAO_);
  glGenBuffers(1, &skybox_VBO_);
  glBindVertexArray(skybox_VAO_);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  spdlog::info("renderer::init_skybox  - skybox initialized...");
}

void Renderer::draw_skybox() {
  // spdlog::info("drawing skybox....");
  glDepthFunc(GL_LEQUAL);
  skybox_shader_->use();
  skybox_shader_->set_mat4("projection", projection_matrix_);
  glm::mat4 skybox_view = glm::mat4(glm::mat3(view_matrix_));
  skybox_shader_->set_mat4("view", skybox_view);
  glBindVertexArray(skybox_VAO_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap_);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
  // spdlog::info("drew skybox...");
}

void Renderer::start_new_frame() {
  // spdlog::info("entered start_new_frame...");
  glClearColor(0.1f, 0.2f, 0.4f, 1.0f); // Nice sky blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  view_matrix_ = player_->camera_->get_view_matrix();
  projection_matrix_ =
      player_->camera_->get_projection_matrix(SCR_WIDTH_, SCR_HEIGHT_);
  model_matrix_ = glm::mat4(1.0f);
  shader_->use();
  shader_->set_mat4("view", view_matrix_);
  shader_->set_mat4("projection", projection_matrix_);
  shader_->set_mat4("model",
                    model_matrix_); // chunk verts already in world space...
  shader_->set_vec3("viewPos", player_->get_player_pos_global_f());
  shader_->set_vec3("lightPos", glm::vec3(145.0f, 279.0f, 200.0f));
  shader_->set_vec3("lightColor", glm::vec3(1.0f));
  // spdlog::info("leaving start_new_frame");
}

void Renderer::render() {
  // spdlog::info("entering render...");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  shader_->set_int("currentTexture", 0);
  if (mesher_.is_dirty()) {
    mesher_.generate_mesh();
    mesher_.mark_clean();
  }

  start_new_frame();
  glBindVertexArray(mesher_.get_VAO());
  glDrawArrays(GL_TRIANGLES, 0, mesher_.get_vertex_count());
  glBindVertexArray(0);

  // draw skybox last...
  // spdlog::info("about to enter draw_skybox...");
  draw_skybox();
  // spdlog::info("leaving render...");
}

void Renderer::loop() {
  while (!glfwWindowShouldClose(window_)) {
    process_input();
    render();
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

unsigned int load_texture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

unsigned int load_cubemap(std::vector<std::filesystem::path> skybox_faces) {
  spdlog::info("entered load_cubemap");
  unsigned int texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  int width, height, nrChannels;
  unsigned char *skybox_data;
  for (unsigned int i = 0; i < skybox_faces.size(); i++) {
    skybox_data =
        stbi_load(skybox_faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (skybox_data) {
      spdlog::info("loaded skybox data from file, about to upload to gpu...");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width,
                   height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skybox_data);
      stbi_image_free(skybox_data);
    } else {
      std::cout << "Cubemap tex failed to load at path: " << skybox_faces[i]
                << std::endl;
      stbi_image_free(skybox_data);
    }
  }

  spdlog::info(
      "finished cubemap loading loops... setting tex parameters for skybox...");
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  spdlog::info("leaving load_cubemap, returning texture_id...");
  return texture_id;
}

void Renderer::process_input() {
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_, true);
    spdlog::info("ESC key press...");
  }
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
    player_->process_keyboard(FORWARD);
    spdlog::info("W key press...");
  }
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    player_->process_keyboard(BACKWARD);
    spdlog::info("S key press...");
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    player_->process_keyboard(LEFT);
    spdlog::info("A key press...");
  }
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    player_->process_keyboard(RIGHT);
    spdlog::info("D key press...");
  }
  /*
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      camera->toggle_lock();
   */
}

void Renderer::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (first_mouse) {
    last_x = xpos;
    last_y = ypos;
    first_mouse = false;
  }
  float xoffset = xpos - last_x;
  float yoffset =
      last_y - ypos; // Reversed since y-coordinates go from bottom to top
  last_x = xpos;
  last_y = ypos;
  float sensitivity = 0.5f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;
  instance_->player_->process_mouse_movement(xoffset, yoffset);
}

Renderer *Renderer::instance_ = nullptr;
