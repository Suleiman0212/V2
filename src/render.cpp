#include "render.hpp"
#include "quad.hpp"
#include "shader.hpp"
#include "window.hpp"
#include <array>
#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <span>

struct Vertex {
  glm::vec2 pos;
  glm::vec2 tex_coords;
};

const std::array verts{
    Vertex(glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // top right
    Vertex(glm::vec2(1.0, 1.0f), glm::vec2(1.0f, 0.0f)),  // bottom right
    Vertex(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)), // top left
    Vertex(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)), // bottom right
    Vertex(glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f)), // bottom left
    Vertex(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f))  // top left
};

Renderer::Renderer(Window &window)
    : shader("shaders/vertex.glsl", "shaders/fragment.glsl") {
  update_projection(window);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coords));
  glEnableVertexAttribArray(1);

  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(),
               GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // window.framebuffer_callback = [&](int width, int height) {
  //   update_projection(window);
  // };
}

Renderer::~Renderer() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

void Renderer::draw_quad(const Quad &quad) {
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  shader.use();
  glBindVertexArray(VAO);

  shader.use();
  glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE,
                     glm::value_ptr(quad.model()));

  glBindTexture(GL_TEXTURE_2D, quad.texture.id);

  glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

void Renderer::draw_quads(const std::vector<Quad> &quads) {
  for (auto &quad : quads) {
    draw_quad(quad);
  }
}

void Renderer::update_projection(const Window &window) {
  shader.use();
  int fb_width, fb_height;
  window.get_framebuffer_size(fb_width, fb_height);
  glm::mat4 projection =
      glm::ortho(0.0f, (float)fb_width, (float)fb_height, 0.0f);
  glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1,
                     GL_FALSE, glm::value_ptr(projection));
}
