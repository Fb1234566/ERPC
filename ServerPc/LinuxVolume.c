#include <Python.h>
#ifdef __linux__
#include <alsa/asoundlib.h>
#endif

static PyObject* LinuxVolume_changeVolume(PyObject* self, PyObject* args) {
  //function used to change the system volume
    long volume;
    if (!PyArg_ParseTuple(args, "l", &volume)) {
        return NULL;
    }
#ifdef __linux__
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
#endif
    Py_RETURN_NONE;
}

// Method definitions
static PyMethodDef LinuxVolumeMethods[] = {
    {"changeVolume", LinuxVolume_changeVolume, METH_VARARGS, "Set the volume to the specified value"},
    {NULL, NULL, 0, NULL} // Sentinel
};

// Module definition
static struct PyModuleDef LinuxVolume = {
    PyModuleDef_HEAD_INIT,
    "LinuxVolume", // Module name
    NULL,       // Module documentation, may be NULL
    -1,         // Size of per-interpreter state of the module, or -1 if the module keeps state in global variables
    LinuxVolumeMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_LinuxVolume(void) {
    return PyModule_Create(&LinuxVolume);
}
