# Ray tracer
as its core, a ray tracer sens rays through pixels and computes the color seen in
the direction of those rays.the involved steps are
	1. Calculate the ray from the "eye" through the pixel.
	2. Determine which object the ray intersect
	3. Compute a color for the closes intersection point.

# Chose the ratio
I’ve often gotten into trouble using square images for debugging because I transpose 𝑥
 and 𝑦
 too often, so we’ll use a non-square image. A square image has a 1∶1 aspect ratio, because its width is the same as its height. Since we want a non-square image, we'll choose 16∶9 because it's so common. A 16∶9 aspect ratio means that the ratio of image width to image height is 16∶9. Put another way, given an image with a 16∶9 aspect ratio,

width/height=16/9=1.7778
For a practical example, an image 800 pixels wide by 400 pixels high has a 2∶1 aspect ratio.

