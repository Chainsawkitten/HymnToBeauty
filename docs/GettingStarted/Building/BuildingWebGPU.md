\page BuildingWebGPU Building the WebGPU backend

Hymn to Beauty has an optional WebGPU backend. However, the goal is to support any WebGPU implementation, rather than mandating a specific one. Therefore, a WebGPU implementation is not included as part of the repository, so building with the WebGPU backend requires some additional steps.

First, make sure you've followed all of the build instructions for the platform you're building for. Then follow the additional instructions for your chosen WebGPU backend.

## Backends
 - \subpage Dawn
 - \subpage wgpu