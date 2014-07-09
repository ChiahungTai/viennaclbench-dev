<h1>ViennaCL Benchmark Readme</h1>
This is the developer repository of ViennaCL Benchmark GUI.
<h2>About ViennaCL Benchmark</h2>
The ViennaCL Benchmark GUI is a fresh project aiming to create a cross-platform standalone benchmarking program for the ViennaCL library. It will utilize the capabilities of ViennaCL to perform hardware benchmarking and collection of users' test results.<br>
The project is being developed by <a href="http://zalomiga.ba/">Namik Karovic</a> as a part of 2014 Google Summer of Code.<br>
ViennaCL was developed under the aegis of the Institute for Microelectronics at the Vienna University of Technology.<br>
For any technical questions related to ViennaCL, please use <a href="mailto:viennacl-support%40lists.sourceforge.net">our
mailing list</a>.<br>

<h2>Requirements</h2>
-Works with Qt4 (tested with 4.8.5) and Qt5 (tested with 5.2.0 and 5.3.0)<br>
-Qt Creator with QMake; alternatively, a CMake build is now available<br>
-CMake 2.8.11 (Qt5) or 2.6 (Qt4)<br>
-ViennaCL 1.5.2<br>
-Boost 1.55<br>
-OpenCL SDK (tested with AMD APP SDK 2.9)
<h2>System Environment Variables Prerequisites</h2>
<h3>Qt</h3>
When building with CMake, you should have Qt's bin folder in your path and/or have QT_QMAKE_EXECUTABLE system environment variable defined to point to qmake.exe location of your local Qt bin folder.<br>
<h3>Boost</h3>
Your system path should contain the path to Boost (e.g. C:\boost\boost_1_55_0)
<h3>ViennaCL</h3>
Your system path should contain the path to ViennaCL (e.g. C:\ViennaCL-1.5.2)
<h3>OpenCL</h3>
If you wish to enable OpenCL usage in the benchmark, these steps should be followed:<br>
* Make sure you have defined the OPENCLROOT system environment variable. It should point to the root folder (containing the bin/, include/ and lib/ folders) of your OpenCL SDK (e.g. C:\AMDAPPSDK). This variable is used to automatically detect and enable OpenCL.<br>
* Make sure OpenCL.dll can be found in your system environment (AFAIK Windows' system32/ folder may contain a copy of OpenCL.dll; if this is not the case, add the bin/ folder of your OpenCL SDK to the system PATH - e.g. add the folder C:\AMDAPPSDK\bin to your PATH variable)<br>
<h2>Build Process</h2>
<h3>Qt Creator with the default qmake build system:</h3>
1. open ViennaCL_Benchmark.pro with Qt Creator<br>
2. when prompted to configure project, choose a Qt4 or Qt5 kit (tested with 4.8.5, 5.2.0 and 5.3.0) and click Configure Project<br>
3. in the case you have not (properly) set your environment variables, you may edit the ViennaCL_Benchmark.pro file and manually change the project's include and depend paths for ViennaCL and Boost<br>
BOOSTROOT = C:\boost\boost_1_55_0 ==> BOOSTROOT = yourPathToViennacl<br>
VIENNACLROOT = C:\Users\Namik\Documents\GitHub\viennacl-dev ==> VIENNACLROOT = yourPathToViennacl<br>
4. in order to run solver and sparse benchmarks, you must add a make install step to your build process: <br>
In QtCreator, under Projects>Build Steps>Make:>add to Make arguments: install<br>
Do this for both debug and release modes<br>
5. you can now run the project (Ctrl+R)<br>
<h3>CMake build system - with Qt Creator</h3>
<b>Default CMakeLists.txt now supports both Qt4 and Qt5, with OpenCL support. The build will default to Qt5, but if you want to build with Qt4 set USE_QT5 variable to OFF.</b><br>
1. Open CMakeLists.txt with CMake gui or Qt Creator<br>
2. Run CMake; At this point you may disable USE_QT5 variable in order to build with Qt4. In this case you must change the QT_QMAKE_EXECUTABLE variable to point to qmake.exe of your local Qt4 bin directory.<br>
3. Compile<br>
4. Manually copy testdata folder from project root folder into the root folder where the project was compiled(will be automated/removed soon)<br>
5. You may now run the program<br>
<h2>Important Notice</h2>
-When not using OpenCL: Blas3 benchmark is extremely slow. Takes about 5 minutes to complete on my machine. Don't run it if you can't wait; Solver benchmark crashes in Qt5 release builds without OpenCL.<br>
<h2>Troubleshooting</h2>
In case you get an error in QtGui/qopenglfunction.h : remove(or comment out) line 785 in that file (void (QOPENGLF_APIENTRYP MemoryBarrier)(GLbitfield barriers);); it is a known Qt bug (https://github.com/go-qml/qml/issues/56) and should be fixed in Qt 5.3 (recently tested with Qt 5.3 - the bug is now gone)<br>
<h2>Additional Info</h2>
I'm running a <a href="http://zalomiga.ba/blog">devblog</a> dedicated to this project and will try to update it as much as possible. Check it out for detailed reports and updates on the development process.
