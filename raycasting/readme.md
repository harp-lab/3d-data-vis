# Ray casting
libs: glew, glfw, and glm

## Steps:
- Draw a box with back face by using face culling
- Store this back face as texture by using frame buffer

This texture is used to calculate the exit points. Hence you can also get the direction by minus entry point.

![backface](https://github.com/KokoFan16/openGL/blob/master/raycasting/pics/backface.png)

![frontface](https://github.com/KokoFan16/openGL/blob/master/raycasting/pics/frontface.png)

- Load trasfer texture (color map) and volumn texture. 
- Calculate accumulated opacity and color. 

## Updata

Add Gui (imGui)

![demo](https://github.com/KokoFan16/openGL/blob/master/raycasting/pics/gui_demo.gif)

## Result
![frontface](https://github.com/KokoFan16/openGL/blob/master/raycasting/pics/molecule.png)
![frontface](https://github.com/KokoFan16/openGL/blob/master/raycasting/pics/head.png)
