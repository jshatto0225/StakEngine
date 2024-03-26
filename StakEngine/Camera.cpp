#include "Camera.h"

#include "Log.h"
#include "StakMath.h"

void SetCameraViewportSize(scene_view_camera *Camera, i32 Width, i32 Height) {
  Camera->AspectRatio = (f32)Width / (f32)Height;
}

void RecalculateCameraProjection(scene_view_camera *Camera) {
  switch (Camera->Type) {
  case PROJ_TYPE_ORTHOGRAPHIC: {
    f32 Left = -Camera->OrthoSize * 0.5f * Camera->AspectRatio;
    f32 Right = Camera->OrthoSize * 0.5f * Camera->AspectRatio;
    f32 Bottom = -Camera->OrthoSize * 0.5f;
    f32 Top = Camera->OrthoSize * 0.5f;
    Camera->Projection = OrthographicProjection(Left,
                                                Right,
                                                Bottom,
                                                Top,
                                                Camera->OrthoNearPlane,
                                                Camera->OrthoFarPlane);
    break;
  }
  case PROJ_TYPE_PERSPECTIVE:
    LogCoreError("Perspective Projection Not Supported");
    break;
  }
}

void RecalculateCameraView(scene_view_camera *Camera) {
  quat Quaternion = DirectionVecToQuat(&Camera->Orientation);
  mat4 Rotation = QuatToMat4(&Quaternion);
  vec3 NegativePos = { -Camera->Pos.x, -Camera->Pos.y, -Camera->Pos.z };
  mat4 Translate = TranslationMatrix(&NegativePos);
  mat4 Transform = Matmul(&Translate, &Rotation);
  Camera->View = Inverse(&Transform);
}

void RecalculateCameraViewProj(scene_view_camera *Camera) {
  Camera->ViewProj = Matmul(&Camera->Projection, &Camera->View);
}

void SetDefaultOrthoCameraSettings(scene_view_camera *Camera) {
  *Camera = {};
  Camera->Pos = { 0, 0, 0 };
  Camera->Orientation = { 0, 0, 0 };
  Camera->AspectRatio = 1;
  Camera->OrthoNearPlane = -1;
  Camera->OrthoFarPlane = 1;
  Camera->OrthoSize = 1;
}
