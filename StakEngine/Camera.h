#pragma once

#include "StakMath.h"

/**
 * @brief Camera projection type enum
 */
enum projection_type
{
    PROJ_TYPE_ORTHOGRAPHIC, /**< Orthographic projection */
    PROJ_TYPE_PERSPECTIVE /**< Perspective projection */
};

/**
 * @brief Struct for a camera to "peek" into a scene
 *        To be used by the editor only 
 *        Could also be called the editor camera
 */
struct scene_view_camera
{
    mat4 View; /**< View matrix */
    mat4 Projection; /**< Projection matrix */
    mat4 ViewProj; /**< Projection * View */
    projection_type Type; /**< Projection type */
    vec3 Pos; /**< Camera position */
    vec3 Orientation; /**< Camera orientation */
    f32 AspectRatio; /**< Camera aspect ratio */
    f32 OrthoNearPlane; /**< Near plane for orthographic projection */
    f32 OrthoFarPlane; /**< Far plane for orthographic projection */
    f32 OrthoSize; /**< Size of orthographic projection (World coordinates) */
};

/**
 * @brief Set the camera viewport size
 *        Camera will be modified
 * 
 * @param Camera Camera to update
 * @param Width Width of viewport
 * @param Height Height of viewport
 */
void SetCameraViewportSize(scene_view_camera *Camera, i32 Width, i32 Height);

/**
 * @brief Recalculate projection matrix 
 *        Camera will be modified
 * 
 * @param Camera Camera to update
 */
void RecalculateCameraProjection(scene_view_camera *Camera);

/**
 * @brief Recalculate view matrix
 *        Camera will be modified
 * 
 * @param Camera Camera to update
 */
void RecalculateCameraView(scene_view_camera *Camera);

/**
 * @brief Recalculate ViewProj matrix
 *        Camera will be modified
 * 
 * @param Camera Camera to update
 */
void RecalculateCameraViewProj(scene_view_camera *Camera);

/**
 * @brief Set the default ortho camera settings
 *        Camera will be modified
 *        View, Proj, and ViewProj will be recalculated
 * 
 * @param Camera Camera to update
 */
void SetDefaultOrthoCameraSettings(scene_view_camera *Camera);
