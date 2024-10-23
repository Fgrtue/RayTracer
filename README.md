# Raytracer

With this repository, you can build quite an effective version of raytracer that I created.

### Features

Here I briefly described all the features that were implemented in the project. I descend from more important to my taste to less important

#### 1. Phong reflection model

Computes the impact:
	1) Light rays
		- Diffusive and specular component
	2) "Eye tracing"
		- Recursively computes reflection and refraction
   
#### 2. Acceleration structures

  - The bottleneck of performance is the computation of the intersection of rays with objects
  - Clustering objects in boxes and computing the intersection with those boxes allows to logarithmically decrease one parameter of complexity
  
#### 3. Camera transformation

  - Transforms the coordinates of the viewer depending on the origin and the direction of view
 
#### 4. Tone mapping
   
  - Uses *Gamma correction* to approximate the colors with a high range of difference

#### 5. `.OBJ` input
   
  - Takes `.obj` file as an input

#### 6. `.PNG` output
   
  - Creates a `.png` file as a result

#### 7. Antialiasing
    
  - Performs point sampling when casting the rays
  
#### 8. Logging
    
  - Makes a log of results to compare performance
  - Computes the duration, amount of ray casting, intersection, etc.
#### 9. **CMake**
    
  - Has `Debug` and `Release` versions
  - Both support non `BVH` and `BVH` modes

## How to build?

1. You will need to have libpng on your machine downloaded in order to build the project:
  
  - For Linux use `sudo apt-get install libpng-dev`

  - For Windows use `./vcpkg install libpng`

2. Once you have the library you can cmake with the following command:

- For Debug: `cmake -DCMAKE_BUILD_TYPE=Debug -H. -Bbuild/Debug`
- For Release: `cmake -DCMAKE_BUILD_TYPE=Release -H. -Bbuild/Release`

3. The release works way faster than debug, so `cd build/Release`, then do `make` and `cd bin/`.

4. At this moment you have reached the binary folder, where you have the binary for raytracer.

## How to run?

The binary takes as input up to two arguments: an absolute path to a `.obj` file that you want to render, and an optional argument `-BVH`. `BVH` flag is used to increase the speed of raytracer, using box volumes for objects. 




