#include "EditorLayer.h"

#include <iostream>
#include <Runtime/Platform/OpenGL.h>

EditorLayer::EditorLayer() {
  this->tex = MakeShared<Texture2D>(ASSET_DIR "Images/VerticalTest.bmp");
  this->tex2 = MakeShared<Texture2D>(ASSET_DIR "Images/WhiteImage.bmp");
  //this->tex3 = MakeShared<Texture2D>(ASSET_DIR "Images/BigTexture.bmp");
}

EditorLayer::~EditorLayer() {

}

void EditorLayer::update() {
  RenderCommand::SetClearColor(1, 0, 1, 1);
  RenderCommand::Clear();

  Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, tex);
  Renderer2D::DrawQuad({ -1.0f, -1.0f }, { 1.0f, 1.0f }, tex2);
  Renderer2D::DrawQuad({ -0.25f, -0.25f }, { 0.5f, 0.5f }, tex);
  Renderer2D::Flush();
}

void EditorLayer::onEvent(Event &e) {

}