# Ray casting
libs: glew, glfw, and glm

## Steps:
- Draw a box with back face by using face culling
- Store this back face as texture by using frame buffer

This texture is used to calculate the exit points. Hence you can also get the direction by minus entry point.

![backface]()

![frontface]()

- Load trasfer texture (color map) and volumn texture. 
- Calculate accumulated opacity and color. 
