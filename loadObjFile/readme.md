## Loading 3D object file 

This project is used to load 3D object file. 

#### Updating (11/13/2019): Add lighting
    For adding light for 3D object, we need normal vector of faces, which is a vector perpendicular to the surface at a given point, as shown below:
    
![Result](https://github.com/KokoFan16/openGL/blob/master/loadObjFile/normal.png = 200x300) 

    Here is the result:
![Result](https://github.com/KokoFan16/openGL/blob/master/loadObjFile/footWithLighting.png) 

    Here is some example about the object file format:

    v 135 105 7.73 
    v 134.325 105 8 
    v 135 104.5 8 
    v 135.587 105 8 
    v 135 105.794 8
    v 136 105.95 8
    v 136 106 7.99057
    ...
    f 1 2 3 
    f 1 3 4 
    f 5 2 1 
    f 6 7 8 
    f 4 5 1 
    f 7 6 9 
    f 10 11 12
    f 10 12 13 
    f 14 8 7 
    f 7 9 14
    f 15 11 10 
    ...

    It includes two header files: `shader.h` and `camera.h`.  <br> 
    * `shader.h` is used to load and compile shader. And also, it provides several set functions. <br> 
    * `camera.h` is used to control the position of camera by keyboard and mouse. <br> 

    Two simple shaders: `shader.vs` and `shader.fs`.<br> 
    * `shader.vs` is the vertice shader.<br> 
    * `shader.fs` is the fragment shader.<br> 

    The entry of this project is `main.cpp`. <br> 

    Here is the running result:<br> 
![Result](https://github.com/KokoFan16/openGL/blob/master/loadObjFile/foot.png)




