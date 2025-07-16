# beginMeshing

- by Mikhail Bogdanov

<img width="800" height="690" alt="image" src="https://github.com/user-attachments/assets/d318063b-1904-4800-91d6-c3606318b080" />


Readme updated July 15th:

This is my first attempt at making a 2D mesher. You are able to receive results like above by passing the coordinates of nodes, followed by indicies of nodes to connect by edge in order. (examples can be found in models folder with .delaunay extension)

## using the mesher:

![image](https://github.com/user-attachments/assets/38d96989-83a0-463b-8c8f-9b670dbf6689)


You just have to pass the path of the model data file, and 3 parameters which I will explain later.

## jul 15th update:

since first posting this repo, I made every algorithm much quicker and almost linear in time complexity.
## Update to generating extra nodes:

My goal for my mesh is to have smaller triangles on the edges, and larger triangles on the inside. My last method didn't quite achieve the results I was looking for, since drawing a line with points from one edge node to another frequently resulted in nodes being bunched up in the center, which is the opposite result to what I'm looking for. That's why for this algorithm I simply filled a grid of nodes at a specified distance apart from eachother, which then turns into a really nice grid inside the mesh, where almost every triangle is almost perfectly equalatiral. I then added nodes around my edges for finer triangles near the edges. This method barely achieves my goal of smaller triangles to the edges, however it creates a very nice mesh with equalatiral triangles. Another issue is it creates more points outside the geometry which we then waste time computing the delaunay triangulation for. The running time for this algorithm is linear to the number of points added

## Update to creating the 2D unconstrained delaunay triangulation:

Now when I first create the super triangle I keep track of which triangle every point is inside of, even if they're simply floating points (a point which is not part of any triangle) is inside of. I also keep track of which triangles still have points inside, and run a loop which goes something like while(there are still triangles with points inside) pick the first triangle with points inside, pick the first floating point in the triangle, and perform a breadth first search on the triangles around to see which ones have the point inside the circumcircle. I then delete those triangles and create new ones according to bowyer watson algorithm.

While this algorithm might be slower for super small models (~40 triangles) for large models with tens of thousands of triangles we perform significantly less circumcirle tests, rather than before where I checked every triangles circumcircle. I was surprised to run some tests and learn that this makes my delaunay triangulation run in what appears to be linear time!

<img width="700" height="620" alt="Create_Unconstraint_vs_n_points" src="https://github.com/user-attachments/assets/3102adab-6a7f-4a11-8153-9fd0b54ab33f" />

## Update to my constraint forcing algorithm:

In my old algorithm for every edge I checked to see if it intersects with every triangle, and if it did I sliced the triangle accordingly. This lead to a very bad run time since it relied not only on the length of the edge, but also on the total number of triangles in the model. My new algorithm starts at the point of origin, and recursively looks at the neighbors of the triangles it's in and walks towards the endpoint. it also doesn't create any new points, it instead checks for edge intersections, finds the 2 triangles that are attached to the edge, creates a quadrilatiral from those triangles, if the quadralatiral is convex, it flips the edge, else it skips the edge and comes back to it later when it's guaranteed to be convex. This algorithm is way faster, as it only looks at the triangles directly adjacent to the edge. This means the run time is linear with number of edges that have to be flipped:

<img width="700" height="620" alt="time_to_force_constraints_vs_Number_of_constrained_Edges_created" src="https://github.com/user-attachments/assets/6406cea7-1364-413d-bb83-5905e1895653" />

## Update to outside triangle deletion:

Before I used to shoot a ray straight up from each triangle, and see if it intersected with any edge. This required geometry and was not very efficient. I now select a triangle, and do a serch across its neighbors with the boundaries being constrained edges. If triangles touch the edge of the mesh (an edge of a triangle only has 1 triangle attached) then I delete all the triangles in the path. This method is more efficient since there's no geometric calculations required, however it's more error prone, since if my model isn't watertight all the triangles will get deleted. the running time of this algorithm is also linear with the number of triangles generated:

<img width="700" height="620" alt="Time_to_delete_outside_tri_vs_Total_Triangles" src="https://github.com/user-attachments/assets/d7d832d9-1357-4a29-bf1e-7814d68ce472" />

## new gallery:

<img width="793" height="471" alt="image" src="https://github.com/user-attachments/assets/5bfe172c-c056-48fe-89e9-3d79e345fa19" />

<img width="790" height="747" alt="image" src="https://github.com/user-attachments/assets/277ebbeb-fad7-46fb-9948-25e9e0c3d009" />

<img width="793" height="191" alt="image" src="https://github.com/user-attachments/assets/48734a42-ddba-4154-aae6-534cbc82c3ab" />






## heart of the application:

![image](https://github.com/user-attachments/assets/9500012f-dc92-4245-ad87-cf60aa4ad5cf)

## Populate points:

- simply reads the data from the file and populates the necessary data structures.

## old code:
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








