from distutils.core import setup, Extension

setup(name = 'WebPImagePlugin',
      version = '0.1',
      description = 'File decoder an encoder for Python Imaging Library',
      url = 'https://github.com/EverythingMe/PIL-WebP',
      ext_modules = [Extension('_webp_wrapper', 
                               sources = ['_webp_wrapper.c'],
                               libraries = ['webp'])]
)
