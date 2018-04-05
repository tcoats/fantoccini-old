# Fantoccini

## MacOS build and run
- Unzip macos.zip
- `./macos.sh`

## Progress
- [x] GLFW Window
- [x] GLEW Setup
- [x] GLM Imported
- [x] Vertex arrays
- [x] Vertex shaders
- [x] Fragment shaders
- [x] MagicaVoxel loader
- [x] Fbx loader
- [x] Display tri
- [x] State directory for writing
- [x] Moddable directory / archive cascade for resources
- [x] PhysFS loaded resources
- [x] Input events
- [x] Perspective and camera
- [x] Instancing
- [x] Lighting
- [X] Shadows
- [ ] Camera movement (WASD + Mouse)
- [ ] Display MagicaVoxel model
- [ ] Display Fbx scene
- [ ] Replace MagicaVoxels with Fbx models

## Later
- [ ] Phyisics
- [ ] Implement cmake
- [ ] Audio
- [ ] Cmake / resource copying etc

## Camera
- https://r3dux.org/2012/12/a-c-camera-class-for-simple-opengl-fps-controls/
- https://github.com/senbon/gloom/wiki/Camera-class
- http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
- https://stackoverflow.com/questions/33761593/glfw-camera-and-mouse-control
- https://learnopengl.com/Getting-started/Camera

## OpenGL
- https://github.com/tomdalling/opengl-series

## Voxel Rendering
- https://medium.com/@fogleman/voxel-rendering-techniques-fa8d869457ca
- https://github.com/Zylann/godot_voxel
- https://github.com/Zylann/voxelgame
- https://0fps.net/2012/07/12/smooth-voxel-terrain-part-2/
- http://transvoxel.org

## Procedural Animation
- http://www.alanzucconi.com/2017/04/17/procedural-animations/
- http://kesen.realtimerendering.com/sig2016.html

## Improvement: Merge-Instancing
- https://www.slideshare.net/DevCentralAMD/vertex-shader-tricks-bill-bilodeau
- http://www.humus.name/Articles/Persson_GraphicsGemsForGames.pdf

## Improvement: Uniform Buffers or Texture Buffers
- https://github.com/progschj/OpenGL-Examples
- http://rastergrid.com/blog/2010/01/uniform-buffers-vs-texture-buffers/
- http://www.java-gaming.org/index.php?topic=28209.0

## Improvement: Shadows
- Automatic light frustum
- Light-space perspective Shadow Maps
- Cascaded shadow maps
- http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

## Improvement: SSAO
- http://thebookofshaders.com
- http://john-chapman-graphics.blogspot.co.nz/2013/01/ssao-tutorial.html
- http://ogldev.atspace.co.uk/www/tutorial46/tutorial46.html
- https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/5.advanced_lighting/9.ssao/ssao.cpp
