#pragma once

#include "StakMath.h"

enum projection_type
{
    PROJ_TYPE_ORTHOGRAPHIC,
    PROJ_TYPE_PERSPECTIVE
};

struct scene_view_camera
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProj;
    projection_type Type;
    vec3 Pos;
    vec3 Orientation;
    f32 AspectRatio;
    f32 OrthoNearPlane;
    f32 OrthoFarPlane;
    f32 OrthoSize;
};

void SetCameraViewportSize(scene_view_camera *Camera, i32 Width, i32 Height);
void RecalculateCameraProjection(scene_view_camera *Camera);
void RecalculateCameraView(scene_view_camera *Camera);
void RecalculateCameraViewProj(scene_view_camera *Camera);
void SetDefaultOrthoCameraSettings(scene_view_camera *Camera);
