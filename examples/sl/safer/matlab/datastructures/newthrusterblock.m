function thrusterBlock = newThrusterBlock

thrusterBlock = newDrawingPatch;

thrusterBlock.label = 'thrusterblock';
thrusterBlock.vertices = [-1 -2 1; -1 2 1; 1 2 1; 1 -2 1; -1 -2 -1; -1 2 -1; 1 2 -1; 1 -2 -1];
thrusterBlock.faces = [1 2 3 4; 1 4 8 5; 1 2 6 5; 2 6 7 3; 3 7 8 4; 5 6 7 8];
thrusterBlock = setColor(thrusterBlock,[0.2706 0.5373 0.93734]);
