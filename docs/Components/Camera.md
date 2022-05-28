\page ComponentCamera Camera

A %Camera component adds a camera through which the scene is rendered.

| Property | Description |
|----------|-------------|
| Orthographic | Whether the camera should use an orthographic projection. |
| Field of view | The field of view in degrees (only when perspective) |
| Size | Size of the view volume (only when orthographic). |
| Z Near | Near field. |
| Z Far | Far field. |
| Viewport | Where on the screen the camera should be rendered. |
| Layers | Which layers the camera should show. |

\section Filters

Various filters can be applied to the camera.

## Bloom

Causes bright areas of the image to bleed.

| Property | Description |
|----------|-------------|
| Intensity | How bright the effect is. |
| Threshold | How bright colors need to be to bleed. |
| Scatter | How blurry the effect is. |

## Gamma correction

| Property | Description |
|----------|-------------|
| Gamma | Gamma correction value |

## FXAA

Cheap anti-aliasing.

## Dither

Applies a low-amplitude noise to prevent color banding.
