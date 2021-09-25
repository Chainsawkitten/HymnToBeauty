\page Tests

Hymn to Beauty uses unit tests to test its low-level renderer. This is to catch regressions and to verify that it behaves the same across all platforms and backends. Note that these tests require a GPU, and as such are not run as part of the CI.

To run the tests, simply run `./Tests`. To get a list of options, run `./Tests --help`.

## Example output

```
=========================
Video
=========================
DrawTriangle: Success
DrawVertexTriangle: Success
DrawTexturedTriangle: Success
DrawQuad: Success
DrawTriangles: Success
DrawPushTriangles: Success
DrawStorageTriangle: Success
InvertColors: Success
DrawMipmappedTriangle: Success
DepthPrePass: Success
DrawLines: Success
Attachmentless: Success
ConservativeRasterization: Success
MultipleFrames: Success
ComputeSetBuffer: Success
ComputeVertexBuffer: Success
ComputeMultipleBuffers: Success


Results:
   Success: 17 / 17
   Failure: 0 / 17
   Not run: 0 / 17
```
