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

