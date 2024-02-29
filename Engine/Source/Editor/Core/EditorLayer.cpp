#include "EditorLayer.h"

#include <iostream>
#include <Runtime/Platform/OpenGL.h>

EditorLayer::EditorLayer() {
  float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
  };
  unsigned int indices[] = {  // note that we start from 0!
      0, 1, 3,  // first Triangle
      1, 2, 3   // second Triangle
  };

  shader = std::make_shared<Shader>(SHADER_DIR "Basic.glsl");
  vao = std::make_shared<VertexArray>();
  vbo = std::make_shared<VertexBuffer>(vertices, sizeof(vertices));
  vbo->setLayout({
    {ShaderDataType::FLOAT3, "aPosition", false}
  });
  vao->addVertexBuffer(vbo);
  ibo = std::make_shared<IndexBuffer>(indices, 6);
  vao->setIndexBuffer(ibo);
}

EditorLayer::~EditorLayer() {

}

void EditorLayer::update() {
  RenderCommand::SetClearColor(1, 0, 1, 1);
  RenderCommand::Clear();

  Renderer::BeginScene();
  {
    Renderer::Submit(shader, vao);
  }
  Renderer::EndScene();
}

void EditorLayer::onEvent(Event &e) {
  if (e.type == WINDOW_RESIZED) {
    Log::Trace("Window Resized");
  }
}