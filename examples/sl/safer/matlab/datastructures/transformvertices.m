function verticeData = transformVertices(verticeData,T)

for i=1:rows(verticeData)
   verticeData(i,1:3) = transform3D(T,verticeData(i,1:3));
end;
