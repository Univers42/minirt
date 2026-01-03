- aspect-ratio:
- pixel_spacing: the distance between two adjacents pixels
- square pixel: pixel spacing are of the same distance horizontally and vertically
- aspect-ratio: it's an ideal ratio, which we approximate as best as possibly with the integer-based ratio of image proportions, we use the calcularated image aspect ratio to determine our final viewport width
- eye point: camera center in 3D space  which all scene rays will originate.
- focal length: refer to the distance between the vector from teh camera ceenter pointed to be one unit.
- right-handed coordinates:  where we chose our coordinates reference to be y-axis to go up, the x-axis ot the right, the negative z-axis to the viewing direction
- lerp: a lerp is a linear interpolation between two interval (etg. 0 to 1). 
A lerp is alwasy fo the form:
	blendedValue = (1 - a) * startValue + a * endValue
	Where `a` going from zero to one
-hit point: means the exact point where something touched, interesects, or makes contact with something else. When a line, ray, or curve hits another line, curve, or shape the hit point is the point of intersection.
A ray hits the circle -> the hit point is where the ray meets the circle.
- quandary: 
- Surface:
- hittable: emphaszie the member function that unites them.
- aliasing/ jaggies: Those terms refers to the step stepping that is commonly found in graphic computation, when a camera takes a picture, there is no jaggies along the picture because the edge pixels are a blend of some foreground and background. In harvard they have made this as example how to blur for isntace it's teh same method, the color have to be average around a periphre of a pixel.
- point sampling : `https://www.researchgate.net/publication/244986797_A_Pixel_Is_Not_A_Little_Square_A_Pixel_Is_Not_A_Little_Square_A_Pixel_Is_Not_A_Little_Square`

- bbox : Boundary box