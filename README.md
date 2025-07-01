# beginMeshing

- by Mikhail Bogdanov

![image](https://github.com/user-attachments/assets/c1923c9e-942b-43f0-b46a-39f693979e03)

This is my first attempt at making a 2D mesher. You are able to receive results like above by passing the coordinates of nodes, followed by indicies of nodes to connect by edge in order. (examples can be found in models folder with .delaunay extension)

## using the mesher:

![image](https://github.com/user-attachments/assets/38d96989-83a0-463b-8c8f-9b670dbf6689)


You just have to pass the path of the model data file, and 3 parameters which I will explain later.

## heart of the application:

![image](https://github.com/user-attachments/assets/9500012f-dc92-4245-ad87-cf60aa4ad5cf)

## Populate points:

- simply reads the data from the file and populates the necessary data structures.

## Add Extra Points:

- This function exists to add additional points to the model which can then be connected together to triangulate the mesh.
- goals of these points:
  - Add small triangles near the edges of the model.
  - Add bigger triangles near the center of the model. 
- theory:
  - To add many smaller triangles near the edges of the model, I iterate over every edge and take turns adding points to each side of the edge.
  - The distance between the added points depends on the "pointDist" parameter
  - if the edge is long enough, there can be multiple "levels" of added edge nodes.
  - ![image](https://github.com/user-attachments/assets/fd58f0ea-4c2f-43fe-b774-2c4720f7a55a)
  - The next step is to add points to the inside of the model.
  - This is achieved by taking every "jmpForFillPts" th edge node and adding points between it and every other edge node in a straight line
  - The distance between these new interior nodes is detemined by "pointDist" * "sclForFillPts"
  - ![image](https://github.com/user-attachments/assets/cbe0f749-798a-4dee-bc36-a7982a2d0b21)
  - above is an example of "jmpForFillPts" = 1 since every edge node is connected to every other edge node.
  - The time complexity of this algorithm is O(numPts<sup>2</sup> * scaleModel / (jmpForFillPts<sup>2</sup> * pointDist)) since every point has to interact with every "jmpForFillPts" th point, and add a number of points that's related to the physical scale of the model.
 
## Create 2D Unconstrained:
- Runs the delaunay algorithm on all points.
- At this point the model looks something like:
- <img src="https://github.com/user-attachments/assets/601c067a-7341-4a1a-8c8d-f862fe8950fa" width="300">
- the actual edges are still not present in the model, however we can see a vague outline of the shape where the smaller triangles are.

## Force constraints:
- At this point the shape is ready to have edges inserted.
- For every edge I calculate any triangle that intersects said edge with 3 kinds of intersections:
  - if the edge intersects a point of a triangle I do nothing
  - if the edge splits the triangle in 2, I split the triangle into 3 smaller triangles:
  - <img src = "https://github.com/user-attachments/assets/d8cecf3a-3d16-45eb-bdba-44d8fe2d4cbd" width = "300">
  - if the edge passes through an edge of a triangle I count the triangle edge as part of the model edge.
  - At this point the model looks something like:
  - <img src = "https://github.com/user-attachments/assets/5899f265-07e3-43a2-887e-d877cd6f4e4a" width = "300">
  - As can be seen above we now have accurate edges splitting all triangles

## Delete Outside Triangles:
- I now loop through every triangle, and shoot a ray directly up from the triangle.
- If this ray intersects an even number of edges, it means the triangle is outside the model and can be deleted. Else I keep the triangle. I then send the model to the graphics card and display it on screen.

## gallery:
![image](https://github.com/user-attachments/assets/99a6163b-bdcd-4151-8d96-cd82d25134f0)
![image](https://github.com/user-attachments/assets/1f1be743-a394-41ed-a583-94e53bea79f0)
![image](https://github.com/user-attachments/assets/d062132b-a3c8-4cd9-a835-cfc4657d7a0b)








