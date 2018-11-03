#!/home/m/Projects/python-fun/venv/bin/python
from PIL import Image


imgx = 256 #Image dimensions
imgy = 256
imgz = 10
image = Image.new("HSV", (imgx + imgz, imgy*2))


for x in range(imgx):
    for y in range(imgy):
        col = (x, y, 255) 
        image.putpixel((x, y), col)

    for y in range(imgy):
        col = (x, 255, 255-y) 
        image.putpixel((x, imgy + y), col)


for x in range(imgx,imgx+imgz):
    for y in range(imgy*2):
        col = (0, 0, 255-(y>>1)) 
        image.putpixel((x, y), col)

image.convert("RGB").save("hsv.png", "PNG")

import base64

with open("hsv.png", "rb") as image_file:
    encoded_string = base64.b64encode(image_file.read()).decode()
    print('data:image/png;base64,{}'.format(encoded_string))