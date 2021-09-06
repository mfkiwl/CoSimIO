# Build options for the _CoSimIO_

The CoSimIO uses [CMake](https://cmake.org/) as build system. The following options are available for configuration:

| name | default| description |
|---|---|---|
| CMAKE_BUILD_TYPE | Release | Build type with which the _CoSimIO_ is built. Use `Release` for production and `Debug` to enable more runtime checks (at the cost of being slower).  |
| BUILD_TESTING | OFF | Build the tests. Useful for developing and initial installation to check if everything works as expected. |
| CO_SIM_IO_ENABLE_MPI | OFF | Enable MPI support for distributed environments. Requires an installation of [MPI](https://www.mpi-forum.org/). |
| CO_SIM_IO_BUILD_C | OFF | Build the C-interface |
| CO_SIM_IO_BUILD_PYTHON | OFF | Build the Python-interface |
| CO_SIM_IO_STRICT_COMPILER | OFF | Enable more warnings in the compiler, useful for development. |

---

The configuration that is used is also printed when compiling, e.g.
~~~
Building the CoSimIO with the following configuration:
    CMAKE_BUILD_TYPE:          DEBUG
    CO_SIM_IO_ENABLE_MPI:      ON
    BUILD_TESTING:             ON
    CO_SIM_IO_BUILD_C:         ON
    CO_SIM_IO_BUILD_PYTHON:    ON
    CO_SIM_IO_STRICT_COMPILER: ON
~~~