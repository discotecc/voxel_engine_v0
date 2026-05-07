//openGL setup, shader management, drawing all visible chunks, frustrum culling, sorting
#pragma once
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "world.h"
#include "shader.h"
#include "camera.h"
#include "mesher.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

bool first_mouse = true;
float last_x = 400, last_y = 300;

unsigned int load_texture(char const * path);

class Renderer {
public:
    unsigned int SCR_WIDTH_ = 1920;
    unsigned int SCR_HEIGHT_ = 1080;
    GLFWwindow* window_;
    Camera* camera_;
    Renderer(World& world);
    ~Renderer();

    void init(int wf);
    void render(const World& world, const Camera& camera);
    void upload_chunk_mesh(Chunk& chunk);
    void cleanup();
    void loop();
    void process_input(GLFWwindow* window, Camera* Camera);

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);


private:
    std::unique_ptr<Shader> shader_;
    std::vector<const Chunk*> visible_chunks_;
    unsigned int texture_;

    World& world_;
    Mesher mesher_;

    static Renderer* instance_;   
    //shader file paths
    std::filesystem::path fs_path_;
    std::filesystem::path vs_path_;
    //texture file paths
    std::filesystem::path tex_path_;
    //we only have one big texture atlas and one set of shaders for now... (Noob alert)

    //helpers
    void start_new_frame(const Camera& camera, GLFWwindow* window); //set shader, matrices, clear screen
    void draw_chunk(const Chunk& chunk); // Bind VAO and issue draw call
    bool is_chunk_visible(const Chunk& chunk, const Camera& camera) const; //simple fustrum culling

    //updated every frame...
    glm::mat4 view_matrix_;
    glm::mat4 projection_matrix_;
};

Renderer::Renderer(World& world) : world_(world), mesher_(world)  {

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

   

    camera_ = new Camera(glm::vec3(0.0f,0.0f,3.0f));
    glfwSetCursorPosCallback(window_, mouse_callback);
}

Renderer::~Renderer() {
    glfwDestroyWindow(window_);
    glfwTerminate();
    delete camera_;
}

void Renderer::init(int wf) {
    std::filesystem::path current = std::filesystem::current_path();
    fs_path_ = current / "src" / "shaders" / "mainfragmentshader.fs";
    vs_path_ = current / "src" / "shaders" / "mainvertexshader.vs";
    
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

    mesher_.init();
    //mesher_.generate_full_mesh();
}

void Renderer::start_new_frame(const Camera& camera, GLFWwindow* window) {
    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);   // Nice sky blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view_matrix_ = camera.get_view_matrix();
    projection_matrix_ = camera.get_projection_matrix(SCR_WIDTH_, SCR_HEIGHT_);

    shader_->use();
    shader_->set_mat4("view", view_matrix_);
    shader_->set_mat4("projection", projection_matrix_);
    shader_->set_mat4("model", glm::mat4(1.0f)); //chunk verts already in world space...
    shader_->set_vec3("viewPos", camera.Position);
}

void Renderer::render(const World& world, const Camera& camera) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);
        shader_->set_int("currentTexture", 0);

        //call start_new_frame
        start_new_frame(camera, window_);
        mesher_.generate_mesh(); //todo - good dynamic meshing...
        glBindVertexArray(mesher_.get_VAO());
        glDrawArrays(GL_TRIANGLES, 0, mesher_.get_vertex_count());
        glBindVertexArray(0);
        //collect visible chunks
        //call fulstrum culling probably
        //loop over visible_chunks and call draw_chunk
    }

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int load_texture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Renderer::loop() {

    while(!glfwWindowShouldClose(window_)) {
        process_input(window_, camera_);
        render(world_, *camera_);
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
       
    
}

void Renderer::process_input(GLFWwindow* window, Camera* camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->process_keyboard(FORWARD);       
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->process_keyboard(BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->process_keyboard(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->process_keyboard(RIGHT);
    /*
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera->toggleLock();
     */
}

void Renderer::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // Reversed since y-coordinates go from bottom to top

    last_x = xpos;
    last_y = ypos;

    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    instance_->camera_->process_mouse_movement(xoffset, yoffset);
}

Renderer* Renderer::instance_ = nullptr;

