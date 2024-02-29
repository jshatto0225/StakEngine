#pragma once

#include <Runtime/Runtime.h>

class EditorLayer : public ApplicationLayer {
public:
  EditorLayer();
  ~EditorLayer();

  void update() override;
  void onEvent(Event &e) override;

private:
  std::shared_ptr<Shader> shader;
  std::shared_ptr<VertexArray> vao;
  std::shared_ptr<VertexBuffer> vbo;
  std::shared_ptr<IndexBuffer> ibo;
};