import Image
import ImageFile
import StringIO
import _webp

def _accept(prefix):
    return prefix[:4] == "RIFF" and prefix[8:12] == "WEBP"

class WebPImageFile(ImageFile.ImageFile):

    format = "WEBP"
    format_description = "WebP image"

    def _open(self):
        raw = self.fp.read()
        fwidth, fheight, has_alpha = _webp.WebPGetFeatures(raw)                

        if has_alpha:
            self.mode = "RGBA"
            data, width, height = _webp.WebPDecodeRGBA(raw)
        else :
            self.mode = "RGB"        
            data, width, height = _webp.WebPDecodeRGB(raw)

        assert (fwidth == width) and (fheight == height)
        self.size = width, height
        self.fp = StringIO.StringIO(data)
        self.tile = [("raw", (0, 0) + self.size, 0, self.mode)]

def _save(im, fp, filename):

    quality = im.encoderinfo.get("quality", 80)
    if im.mode == "RGB":    
    	data = _webp.WebPEncodeRGB(im.tostring(), im.size[0], im.size[1], im.size[0] * 3, float(quality))
    elif im.mode == "RGBA":
	data = _webp.WebPEncodeRGBA(im.tostring(), im.size[0], im.size[1], im.size[0] * 4, float(quality))
    else:
        raise IOError("cannot write mode %s as WEBP" % im.mode)
    fp.write(data)

Image.register_open("WEBP", WebPImageFile, _accept)
Image.register_save("WEBP", _save)

Image.register_extension("WEBP", ".webp")
Image.register_mime("WEBP", "image/webp")
