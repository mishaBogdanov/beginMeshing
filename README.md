# beginMeshing

- by Mikhail Bogdanov

![image](https://github.com/user-attachments/assets/c1923c9e-942b-43f0-b46a-39f693979e03)

This is my first attempt at making a 2D mesher. You are able to receive results like above by passing the coordinates of nodes, followed by indicies of nodes to connect by edge in order. (examples can be found in models folder with .delouney extension)

## using the mesher:

![image](https://github.com/user-attachments/assets/26d77cc9-b323-4fea-beaa-bbf86574ee49)

You just have to pass the path of the model data file, and 3 parameters which I will explain later.

## heart of the application:

![image](https://github.com/user-attachments/assets/74338259-f3ef-4bc8-bd9b-f913ad64a685)

## Populate points:

- simply reads the data from the file and populates the necessary data structures.

## Add Extra Points:

- This function exists to add additional points to the model which can then be connected together to triangulate the mesh.
- goals of these points:
  - Add small triangles near the edges of the model.
  - Add bigger triangles near the center of the model. 
- implementation:
  - To add many smaller triangles near the edges of the model, I iterate over every edge and take turns adding points to each side of the edge.
  - The distance between the added points depends on the "pointDist" parameter
  - if the edge is long enough, there can be multiple "levels" of added edge nodes.
  - ![image](https://github.com/user-attachments/assets/fd58f0ea-4c2f-43fe-b774-2c4720f7a55a)
  - The next step is to add points to the inside of the model.
  - This is achieved by taking every "jmpForFillPts" th edge node and adding points between it and every other edge node in a straight line
  - The distance between these new interior nodes is detemined by "pointDist" * "sclForFillPts"
  - ![image](https://github.com/user-attachments/assets/cbe0f749-798a-4dee-bc36-a7982a2d0b21)
  - above is an example of "jmpForFillPts" = 1 since every edge node is connected to every other edge node.
  - The time complexity of this algorithm is O(numPts<sup>2</sup> * scaleModel / (jmpForFillPts<sup>2</sup> * pointDist))



