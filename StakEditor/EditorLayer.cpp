#include "EditorLayer.h"

void *
EditorLayerInit() {
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
    Data->Color = { 1, 1, 0, 1 };

    return (void *)Data;
}

// TODO: Do something about this
void
EditorLayerShutdown(void **Data) {
    editor_data **EData = (editor_data **)Data;
    if (*EData) {
        DestroyTexture2D(&(*EData)->Tex1);
        DestroyTexture2D(&(*EData)->Tex2);
        DestroyTexture2D(&(*EData)->Tex3);

        free(*EData);

        EData = NULL;
    }
}

void
EditorLayerUpdate(void *Data) {
    editor_data *EData = (editor_data *)Data;
    EData->QuadRotation += 1.0f;
    if (GetKeyDown(K_W)) {
        LogTrace("Forward");
        EData->Cam.Pos.y += 0.005f;
    }
    if (GetKeyDown(K_A)) {
        LogTrace("Left");
        EData->Cam.Pos.x -= 0.005f;
    }
    if (GetKeyDown(K_S)) {
        LogTrace("Backwards");
        EData->Cam.Pos.y -= 0.005f;
    }
    if (GetKeyDown(K_D)) {
        LogTrace("Right");
        EData->Cam.Pos.x += 0.005f;
    }

    RecalculateCameraView(&EData->Cam);
    RecalculateCameraProjection(&EData->Cam);
    RecalculateCameraViewProj(&EData->Cam);
}

void
EditorLayerRenderSystem(editor_data *Data) {
    RenderCommandSetClearColor(1, 0, 1, 1);
    RenderCommandClear();

    Renderer2DBeginScene(&Data->Cam);
    {
        Renderer2DDrawCircle(&Data->Vec1, &Data->Vec2, Data->QuadRotation * (PI / 180.0f), &Data->Tex3, false);
        Renderer2DDrawQuad(&Data->Vec3, &Data->Vec2, &Data->Color);
        Renderer2DDrawQuad(&Data->Vec2, &Data->Vec2, &Data->Tex1, false);
    }
    Renderer2DEndScene();

    RenderCommandSwapBuffers();
}

void
EditorLayerOnEvent(void *Data, const event *Event) {
    editor_data *EData = (editor_data *)Data;

    if (Event->Type == WINDOW_RESIZED) {
        SetCameraViewportSize(&EData->Cam, Event->WinResizeEvent.Width, Event->WinResizeEvent.Height);
        RecalculateCameraProjection(&EData->Cam);
        RecalculateCameraViewProj(&EData->Cam);
        return;
    }
    if (Event->Type == WINDOW_PAINT) {
        EditorLayerRenderSystem(EData);
    }
}
