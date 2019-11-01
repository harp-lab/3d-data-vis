## Loading 3D object file 

This project is used to load 3D object file. 

Here is some example about the object file format:

v 135 105 7.73 <br> 
v 134.325 105 8 <br> 
v 135 104.5 8 <br> 
v 135.587 105 8 <br> 
v 135 105.794 8 <br> 
v 136 105.95 8 <br> 
v 136 106 7.99057<br> 
...<br> 
<br> 
f 1 2 3 <br> 
f 1 3 4 <br> 
f 5 2 1 <br> 
f 6 7 8 <br> 
f 4 5 1 <br> 
f 7 6 9 <br> 
f 10 11 12 <br> 
f 10 12 13 <br> 
f 14 8 7 <br> 
f 7 9 14 <br> 
f 15 11 10 <br> 
...<br> 
<br> 
It includes two header files: `shader.h` and `camera.h`.  <br> 
* `shader.h` is used to load and compile shader. And also, it provides several set functions. <br> 
* `camera.h` is used to control the position of camera by keyboard and mouse. <br> 

Two simple shaders: `shader.vs` and `shader.fs`.<br> 
* `shader.vs` is the vertice shader.<br> 
* `shader.fs` is the fragment shader.<br> 

The entry of this project is `main.cpp`. <br> 

Here is the running result:<br> 
![Result]()




