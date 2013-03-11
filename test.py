
import WebPImagePlugin
import Image
img = Image.open('/tmp/96.png')
img.save(open('96.webp', 'w+'))
