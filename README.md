# Wavy
Bad Apple, but it's made of sine waves. Based on the Javascript code by Low Byte Productions (https://www.youtube.com/watch?v=5vzMgNAzEuA).

Frames were chopped using this command: ```.\ffmpeg -i ".\badapple.mp4" .\frames\frame%04d.png```

There are some spots where this could be improved. This code is loading 2 image every frame at real time (1 to display via SDL_image, 1 to get the gray value with stb_image). 
A better way to improve would be either load the proper video file, or at least buffer every loading call.
Also, manually loading every image file means that calculating the frame rate is hard, so some video editing is required.

On a more personal note. After listing out all of the possible improvements, it astounds me how fast computers are that this program is running at 'real-time'.

## Demo



https://github.com/trunghung03/Wavy/assets/61304518/7b21c2da-847c-45c4-a830-4857951bf054



## Credits
Very inspired by Low Byte Productions' video: https://www.youtube.com/watch?v=5vzMgNAzEuA

Made using SDL2 for the display, SDL2_image for rendering PNGs, stb_image for getting the gray scale value from each image.
