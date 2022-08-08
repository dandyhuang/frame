# DataSketches Core C++ Library Component
This is the core C++ component of the DataSketches library.  It contains all of the key sketching algorithms that are in the Java component and can be accessed directly from user applications. 

This component is also a dependency of other components of the library that create adaptors for target systems, such as PostgreSQL.

Note that we have a parallel core component for Java implementations of the same sketch algorithms, 
[incubator-datasketches-java](https://github.com/apache/incubator-datasketches-java).

Please visit the main [DataSketches website](https://datasketches.apache.org) for more information. 

If you are interested in making contributions to this site please see our [Community](https://datasketches.apache.org/docs/Community/) page for how to contact us.

---

This code requires C++11. It was tested with GCC 4.8.5 (standard in RedHat at the time of this writing), GCC 8.2.0 and Apple LLVM version 10.0.1 (clang-1001.0.46.4)

This includes Python bindings. For the Python interface, see the README notes in [the python subdirectory](https://github.com/apache/incubator-datasketches-cpp/tree/master/python).

This library is header-only. The build process provided is only for building unit tests and the python library.

Building the unit tests requires cmake 3.12.0 or higher.

Installing the latest cmake on OSX: brew install cmake

Building and running unit tests using cmake for OSX and Linux:

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make
	$ make test

Building and running unit tests using cmake for Windows from the command line:

  $ mkdir build
	$ cd build
	$ cmake ..
	$ cd ..
	$ cmake --build build --config Release
	$ cmake --build build --config Release --target RUN_TESTS

----

Disclaimer: Apache DataSketches is an effort undergoing incubation at The Apache Software Foundation (ASF), sponsored by the Apache Incubator. Incubation is required of all newly accepted projects until a further review indicates that the infrastructure, communications, and decision making process have stabilized in a manner consistent with other successful ASF projects. While incubation status is not necessarily a reflection of the completeness or stability of the code, it does indicate that the project has yet to be fully endorsed by the ASF.