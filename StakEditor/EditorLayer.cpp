#include "EditorLayer.h"

void *EditorLayerInit() {
  editor_data *Data = (editor_data *)malloc(sizeof(editor_data));

  if (!Data) {
    LogError("Failed to allocate space for editor data");
    return NULL;
  }

  Data->WindowWidth = 1280;
  Data->WindowHeight = 720;

  Data->Tex1 = CreateTexture2D(IMAGE_DIR "VerticalTest.bmp");
  Data->Tex2 = CreateTexture2D(IMAGE_DIR "WhiteImage.bmp");
  Data->Tex3 = CreateTexture2D(IMAGE_DIR "BigTexture.bmp");

  SetDefaultOrthoCameraSettings(&Data->Cam);

  SetCameraViewportSize(&Data->Cam, Data->WindowWidth, Data->WindowHeight);
  RecalculateCameraView(&Data->Cam);
  RecalculateCameraProjection(&Data->Cam);
  RecalculateCameraViewProj(&Data->Cam);

  Data->QuadRotation = 0;
  Data->Vec1 = { -0.25f, -0.25f };
  Data->Vec2 = {  0.25f,  0.25f };
  Data->Vec3 = {  0.00f,  0.00f };

  return (void *)Data;
}

void EditorLayerShutdown(void **Data) {
  editor_data **EData = (editor_data **)Data;
  if (*EData) {
    DestroyTexture2D(&(*EData)->Tex1);
    DestroyTexture2D(&(*EData)->Tex2);
    DestroyTexture2D(&(*EData)->Tex3);
  
    free(*EData);

    EData = NULL;
  }
}

void EditorLayerUpdate(void *Data) {
  editor_data *EData = (editor_data *)Data;
  EData->QuadRotation += 1.0f;
}

void EditorLayerRenderSystem(editor_data *EData) {
  RenderCommandSetClearColor(1, 0, 1, 1);
  RenderCommandClear();

  Renderer2DBeginScene(&EData->Cam);
  {
    Renderer2DDrawQuad(&EData->Vec1, &EData->Vec2, EData->QuadRotation * (PI / 180.0f), EData->Tex3, false);
    Renderer2DDrawQuad(&EData->Vec3, &EData->Vec2, EData->Tex1, false);
    Renderer2DDrawQuad(&EData->Vec2, &EData->Vec2, EData->Tex2, false);
  }
  Renderer2DEndScene();
  RenderCommandSwapBuffers();
}

void EditorLayerOnEvent(void *Parent, void *Data, const event *Event) {
  editor_data *EData = (editor_data *)Data;

  if (Event->Type == WINDOW_RESIZED) {
    SetCameraViewportSize(&EData->Cam,
                          Event->WinResizeEvent.Width,
                          Event->WinResizeEvent.Height);
    RecalculateCameraProjection(&EData->Cam);
    RecalculateCameraViewProj(&EData->Cam);
    return;
  }
  if (Event->Type == WINDOW_PAINT) {
    EditorLayerRenderSystem(EData);
  }
}
