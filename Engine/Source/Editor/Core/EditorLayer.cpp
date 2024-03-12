#include "EditorLayer.h"

#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

EditorLayer::EditorLayer(i32 window_width,
                         i32 window_height) {
  this->tex = MakeRef<sk::Texture2D>(ASSET_DIR "Images/VerticalTest.bmp");
  this->tex2 = MakeRef<sk::Texture2D>(ASSET_DIR "Images/WhiteImage.bmp");
  this->tex3 = MakeRef<sk::Texture2D>(ASSET_DIR "Images/BigTexture.bmp");

  this->cam.setOrthographic(1, -1, 1);
  this->cam.setViewportSize(window_width, window_height);
}

EditorLayer::~EditorLayer() {

}

void EditorLayer::update() {
  sk::RenderCommand::SetClearColor(1, 0, 1, 1);
  sk::RenderCommand::Clear();

  sk::Renderer2D::BeginScene(this->cam);
  {
    sk::Renderer2D::DrawQuad({ -0.25f, -0.25f }, { 0.25f, 0.25f }, M_PI / 4, tex3);
    sk::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.25f, 0.25f }, tex);
    sk::Renderer2D::DrawQuad({ 0.25f, 0.25f }, { 0.25f, 0.25f }, tex2);
  }
  sk::Renderer2D::EndScene();
}

void EditorLayer::onEvent(sk::Event &e) {
  switch (e.type) {
  case sk::EventType::WINDOW_RESIZED:
    this->cam.setViewportSize(e.win_resize_event.width, e.win_resize_event.height);
    break;
  default:
    break;
  }
}
