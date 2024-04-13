#include "EditorLayer.h"

editor_data Data;

void
EditorLayerInit() {
    Data.WindowWidth = 1280;
    Data.WindowHeight = 720;

    Data.Tex1 = CreateTexture2D(IMAGE_DIR "VerticalTest.bmp");
    Data.Tex2 = CreateTexture2D(IMAGE_DIR "WhiteImage.bmp");
    Data.Tex3 = CreateTexture2D(IMAGE_DIR "BigTexture.bmp");

    SetDefaultOrthoCameraSettings(&Data.Cam);
    SetCameraViewportSize(&Data.Cam, Data.WindowWidth, Data.WindowHeight);
    RecalculateCameraView(&Data.Cam);
    RecalculateCameraProjection(&Data.Cam);
    RecalculateCameraViewProj(&Data.Cam);

    Data.QuadRotation = 0;
    Data.Vec1 = { -0.25f, -0.25f };
    Data.Vec2 = {  0.25f,  0.25f };
    Data.Vec3 = {  0.00f,  0.00f };
    Data.Color = { 1, 1, 0, 1 };
}

// TODO: Do something about this
void
EditorLayerShutdown() {
      DestroyTexture2D(&Data.Tex1);
      DestroyTexture2D(&Data.Tex2);
      DestroyTexture2D(&Data.Tex3);
}

void
EditorLayerUpdate() {
    Data.QuadRotation += 1.0f;
    if (GetKeyDown(K_W)) {
        LogTrace("Forward");
        Data.Cam.Pos.y += 0.005f;
    }
    if (GetKeyDown(K_A)) {
        LogTrace("Left");
        Data.Cam.Pos.x -= 0.005f;
    }
    if (GetKeyDown(K_S)) {
        LogTrace("Backwards");
        Data.Cam.Pos.y -= 0.005f;
    }
    if (GetKeyDown(K_D)) {
        LogTrace("Right");
        Data.Cam.Pos.x += 0.005f;
    }

    RecalculateCameraView(&Data.Cam);
    RecalculateCameraProjection(&Data.Cam);
    RecalculateCameraViewProj(&Data.Cam);
}

void
EditorLayerRenderSystem() {
    RenderCommandSetClearColor(1, 0, 1, 1);
    RenderCommandClear();

    Renderer2DBeginScene(&Data.Cam);
    {
        Renderer2DDrawCircle(&Data.Vec1, &Data.Vec2, Data.QuadRotation * (PI / 180.0f), &Data.Tex3, false);
        Renderer2DDrawQuad(&Data.Vec3, &Data.Vec2, &Data.Color);
        Renderer2DDrawQuad(&Data.Vec2, &Data.Vec2, &Data.Tex1, false);
    }
    Renderer2DEndScene();

    RenderCommandSwapBuffers();
}

void
EditorLayerOnEvent(const event *Event) {
    if (Event->Type == WINDOW_RESIZED) {
        SetCameraViewportSize(&Data.Cam, Event->WinResizeEvent.Width, Event->WinResizeEvent.Height);
        RecalculateCameraProjection(&Data.Cam);
        RecalculateCameraViewProj(&Data.Cam);
        return;
    }
    if (Event->Type == WINDOW_PAINT) {
        EditorLayerRenderSystem();
    }
}
