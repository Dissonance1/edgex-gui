from PIL import Image
try:
    img = Image.open('src/resources/honeycomblogo.png')
    print(f"SIZE: {img.size}")
except Exception as e:
    print(e)
