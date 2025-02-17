from setuptools import setup, Extension

# Define the extension module
LinuxVolume = Extension('LinuxVolume',
                 sources=['LinuxVolume.c'],
                 libraries=['asound'],  # Link against the ALSA library
                 include_dirs=['/usr/include'],  # Include directory for ALSA headers
                 library_dirs=['/usr/lib'],  # Library directory for ALSA
                 )


# Run the setup
setup(
    name='LinuxVolume',
    version='1.0',
    description='Library used in the ERPC project to change the volume in linux',
    ext_modules=[LinuxVolume],
)
