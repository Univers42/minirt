#!/bin/sh
# Fetch the public-domain / CC texture set used by the showcase scenes.
# All sources are Wikimedia Commons (PD or CC); fetched via Special:FilePath
# (which redirects to the real upload URL) and resized to texture-friendly
# dimensions with ImageMagick. Re-run from this directory: sh fetch_textures.sh
set -e
UA="Mozilla/5.0 (X11; Linux x86_64)"
dl() { # commons-filename  outname  maxdim
	curl -sL -A "$UA" "https://commons.wikimedia.org/wiki/Special:FilePath/$1" -o "/tmp/_tex_$2"
	convert "/tmp/_tex_$2" -resize "${3}x${3}>" -strip "$2.png"
	identify -format "  %f %wx%h\n" "$2.png"
	rm -f "/tmp/_tex_$2"
}
dl "The_Blue_Marble_%28remastered%29.jpg"   earth     2048   # NASA, public domain
dl "FullMoon2010.jpg"                        moon      2048   # CC BY-SA Gregory Revera
dl "Solarsystemscope_texture_8k_mars.jpg"    mars      2048   # CC BY 4.0 Solar System Scope
dl "Solarsystemscope_texture_8k_jupiter.jpg" jupiter   2048   # CC BY 4.0 Solar System Scope
dl "Red_brick_wall_texture.JPG"              brick     1600   # CC BY-SA
dl "ESO_-_Milky_Way.jpg"                     starfield 4096   # ESO, CC BY 4.0
dl "Sunflower_sky_backdrop.jpg"              sky       2048   # CC BY-SA
echo "done."
