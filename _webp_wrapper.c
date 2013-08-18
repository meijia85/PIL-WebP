#include <Python.h>
#include <webp/encode.h>
#include <webp/decode.h>
#include <stdbool.h>

typedef size_t (*WebPEncodeFunc)(const uint8_t*, int, int, int, float, uint8_t**);
typedef uint8_t* (*WebPDecodeFunc)(const uint8_t*, size_t, int*, int *);

PyObject* encode_wrapper(WebPEncodeFunc encode_func, PyObject* self, PyObject* args)
{
    PyStringObject *data_string;
    int width;
    int height;
    int stride;
    float quality_factor;

    if (!PyArg_ParseTuple(args, "Siiif", &data_string, &width, &height, &stride, &quality_factor)) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    
    uint8_t *data;
    Py_ssize_t size;
    PyString_AsStringAndSize((struct PyObject *) data_string, &data, &size);

    if (stride * height > size) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    uint8_t *output;
    size_t ret_size = encode_func(data, width, height, stride, quality_factor, &output);
    if (ret_size > 0) {
        PyObject *ret = PyString_FromStringAndSize(output, ret_size);
        free(output);
        return ret;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* WebPEncodeRGB_wrapper(PyObject* self, PyObject* args) 
{
    return encode_wrapper(&WebPEncodeRGB, self, args);
}

PyObject* WebPEncodeRGBA_wrapper(PyObject* self, PyObject* args)
{
    return encode_wrapper(&WebPEncodeRGBA, self, args);
}

PyObject* decode_wrapper(WebPDecodeFunc decode_func, bool has_alpha, PyObject* self, PyObject* args)
{
    PyStringObject *webp_string;

    if (!PyArg_ParseTuple(args, "S", &webp_string)) {
        Py_INCREF(Py_None);
        return Py_None;
    }    
    uint8_t *webp;
    Py_ssize_t size;
    PyString_AsStringAndSize((struct PyObject *) webp_string, &webp, &size);  

    int width;
    int height;

    uint8_t *output = decode_func(webp, size, &width, &height);

    PyObject *ret = PyString_FromStringAndSize(output, width * height * (3+has_alpha));
    free(output);
    return Py_BuildValue("Sii", ret, width, height);
}

PyObject* WebPDecodeRGB_wrapper(PyObject* self, PyObject* args) 
{
    return decode_wrapper(&WebPDecodeRGB, false, self, args);
}

PyObject* WebPDecodeRGBA_wrapper(PyObject* self, PyObject* args) 
{
    return decode_wrapper(&WebPDecodeRGBA, true, self, args);
}

PyObject* WebPGetFeatures_wrapper(PyObject* self, PyObject* args)
{
    PyStringObject *webp_string;

    if (!PyArg_ParseTuple(args, "S", &webp_string)) {
        Py_INCREF(Py_None);
        return Py_None;
    }    
    uint8_t *webp;
    Py_ssize_t size;
    PyString_AsStringAndSize((struct PyObject *) webp_string, &webp, &size);  

    WebPBitstreamFeatures features;   
    VP8StatusCode err = WebPGetFeatures(webp, size, &features);

    if (VP8_STATUS_OK != err) {
        return PyErr_Format(PyExc_RuntimeError, "WebPGetFeatures returned error: %d", err);
    }

    return Py_BuildValue("iii", features.width, features.height, features.has_alpha);
}

static PyMethodDef webpMethods[] =
{
    {"WebPEncodeRGB", WebPEncodeRGB_wrapper, METH_VARARGS, 
        "Wraps WebPEncodeRGB\nArguments: data, width, height, stride, quality_factor\nReturns encoded image as Python String"},
    {"WebPEncodeRGBA", WebPEncodeRGBA_wrapper, METH_VARARGS,
        "Wraps WebPEncodeRGBA\nArguments: data, width, height, stride, quality_factor\nReturns encoded image as Python String"},
    {"WebPDecodeRGB", WebPDecodeRGB_wrapper, METH_VARARGS, 
        "Wraps WebPDecodeRGB\nReturns a (raw_data, width, height) tuple, where raw_data is the image samples in RGB order"},
    {"WebPDecodeRGBA", WebPDecodeRGBA_wrapper, METH_VARARGS,
        "Wraps WebPDecodeRGBA\nReturns a (raw_data, width, height) tuple, where raw_data is the image samples in RGBA order"},
    {"WebPGetFeatures", WebPGetFeatures_wrapper, METH_VARARGS,
        "Wraps WebPGetFeatures\nReturns a (width, height, has_alpha) tuple"},
    {NULL, NULL}
};

void init_webp_wrapper()
{
    PyObject* m;
    m = Py_InitModule("_webp_wrapper", webpMethods);
}
