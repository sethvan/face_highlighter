# face_highlighter
WebGL app for selecting and highlighting faces of models in STL format

It is meant to be integrated into a friend's tool. It will be used to select and save faces of 3D models of STL file format. I originally did this in OpenGL and then using google I redid it in WebGL as friend needs it for a browser based tool.  

Instead of doing everything in JavaScript though I learned enough emscripten to have a C++ class take the picked VertexID passed to it and do the work for calculating which faces/vertices have been previously selected or saved and which to display for the picked faces based upon how much relative tolerance to allow between the difference in normals of adjacent triangles. The C++ class uses glm and helper structs/functions.  

That same C++ class also uses assimp to load the file which is passed to it as an array from a FileReader in JavaScript. I may change this though as assimp in emscripten does not seem to like binary stl files that are roughly a million triangles or more ( in my code anyways ), and I may checkout how threejs loads them.  

Still evolving this while getting the bugs out. The video begins demoing viewer and then demos the picking.  

https://github.com/sethvan/face_highlighter/assets/78233173/333ea123-0c86-4fda-abe4-444340e2eb13




