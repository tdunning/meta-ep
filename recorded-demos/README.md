# Recorded Optimization Runs

This directory contains a number of traces from optimizing various functions
with different starting conditions. These traces can be visualized using
some TCL programs provided here.

To run the visualization use
```bash
wish show.tcl
```

The available traces are for the following functions

## Bowl 
This is a symmetric 5-dimensional bowl with depth equal to the distance to 
the center squared. This is just about the simplest function to optimize that
there is. Of interest is the fact that the optimum mutation scale should be
about the same as the distance to the center of the bowl. The adaptive mutation
algorithm figures this out nicely.

## Valley 
This is a bowl as above, except that the coordinates are transformed so that
the bowl is now very skinny and the long axis of the bowl is not aligned with 
any of the coordinates. The starting point for these traces has low mutation 
rate and is not positioned on the axis of the bowl. You will see the population
fall towards the bowl and then re-orient the mutation direction to follow the
axis of the bowl.

The implications of this are that the recorded-step algorithm should converge
for functions like this with a scaling factor that is about O(d) where d is 
the dimensionality of the problem. Convergence should be fastest along 
eigenvectors with small eigenvalues and convergence along the last of the 
eigenvectors of the bowl should be about as fast as the symmetric bowl.

## Bohachevsky Function
The Bohachevsky function is a multi-modal function often used in the GA literature.
It is 2-dimensional with small perturbations laid onto a larger symmetric bowl shape.

## Spiral 
The spiral function has a very flat bowl with a narrow spiral valley cut into it. Near
the origin, the valley spirals more and more tightly.
