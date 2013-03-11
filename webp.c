#include <Python.h>
#include <webp/encode.h>
#include <webp/decode.h>

PyObject* WebPEncodeRGB_wrapper(PyObject* self, PyObject* args)
{
    PyStringObject *rgb_string;
    int width;
    int height;
    int stride;
    float quality_factor;

    if (!PyArg_ParseTuple(args, "Siiif", &rgb_string, &width, &height, &stride, &quality_factor)) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    
    uint8_t *rgb;
    Py_ssize_t size;
    PyString_AsStringAndSize((struct PyObject *) rgb_string, &rgb, &size);

    if (stride * height > size) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    uint8_t *output;
    size_t ret_size = WebPEncodeRGB(rgb, width, height, stride, quality_factor, &output);
    if (ret_size > 0) {
        PyObject *ret = PyString_FromStringAndSize(output, ret_size);
        free(output);
        return ret;
    }
    Py_INCREF(Py_None);
    return Py_None;

}

PyObject* WebPEncodeRGBA_wrapper(PyObject* self, PyObject* args)
{
    PyStringObject *rgba_string;
    int width;
    int height;
    int stride;
    float quality_factor;

    if (!PyArg_ParseTuple(args, "Siiif", &rgba_string, &width, &height, &stride, &quality_factor)) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    
    uint8_t *rgba;
    Py_ssize_t size;
    PyString_AsStringAndSize((struct PyObject *) rgba_string, &rgba, &size);

    if (stride * height > size) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    uint8_t *output;
    size_t ret_size = WebPEncodeRGBA(rgba, width, height, stride, quality_factor, &output);
    if (ret_size > 0) {
        PyObject *ret = PyString_FromStringAndSize(output, ret_size);
        free(output);
        return ret;
    }
    Py_INCREF(Py_None);
    return Py_None;

}

PyObject* WebPDecodeRGB_wrapper(PyObject* self, PyObject* args)
{
    PyStringObject *webp_string;
    float quality_factor;

    if (!PyArg_ParseTuple(args, "S", &webp_string)) {
        Py_INCREF(Py_None);
        return Py_None;
    }    
    uint8_t *webp;
    Py_ssize_t size;
    PyString_AsStringAndSize((struct PyObject *) webp_string, &webp, &size);

    int width;
    int height;
    uint8_t *output = WebPDecodeRGB(webp, size, &width, &height);

    PyObject *ret = PyString_FromStringAndSize(output, width * height * 3);
    free(output);
    return Py_BuildValue("Sii", ret, width, height);
}

static PyMethodDef webpMethods[] =
{
    {"WebPEncodeRGB", WebPEncodeRGB_wrapper, METH_VARARGS, "WebPEncodeRGB"},
    {"WebPEncodeRGBA", WebPEncodeRGBA_wrapper, METH_VARARGS, "WebPEncodeRGBA"},
    {"WebPDecodeRGB", WebPDecodeRGB_wrapper, METH_VARARGS, "WebPEncodeRGB"},
    {NULL, NULL}
};

void init_webp()
{
    PyObject* m;
    m = Py_InitModule("_webp", webpMethods);
}
