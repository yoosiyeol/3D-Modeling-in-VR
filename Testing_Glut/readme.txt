
1.How to set intel realsense sdk properties

Create new project with empty project visual c++
Create cpp files with named ¡®main¡¯
Go to Project properties
Go to C/C++ -> Additional Include Directories, put these entry.
$(RSSDK_DIR)/include
$(RSSDK_DIR)/sample/common/include
Go to Linker -> General -> Additional Library Directories, put these entry .
$(RSSDK_DIR)/lib/$(PlatformName)
$(RSSDK_DIR)/sample/common/lib/$(PlatformName)/$(PlatformToolset)
Go to Linker -> Input ->Additional Dependencies, put these entry.
libpxc_d.lib
libpxcutils_d.lib
Chose the x64 platform at Project Properties.
Go to next section of this tutorial at below.

2.How to set OpenCV 3.10
o to Project properties
Go to C/C++ -> Additional Include Directories, put these entry.
$(OPENCV310_DIR)/opencv/build/include
Go to Linker -> General -> Additional Library Directories, put these entry .
$(OPENCV310_DIR)/opencv/build/$(PlatformName)/$(PlatformToolset)
Go to Linker -> Input ->Additional Dependencies, put these entry.
opencv_world310.lib
opencv_world310d.lib

3.OpenGL set
Case glew-2.0.0 
1. copy glew-2.0.0-win32\glew-2.0.0\bin\Release\x64\glew32.dll -> (projetc name)\x64\Debug
2. create folder (projetc name)\(projetc name)\library, Copy glew-2.0.0-win32\glew-2.0.0\lib\Release\x64\ glew32.lib glews32.lib
3. copy dir glew-2.0.0-win32\glew-2.0.0\include -> (projetc name)\(projetc name)

Case freeglut
1. copy freeglut-MSVC-3.0.0-2.mp\freeglut\bin\x64 -> (projetc name)\x64\Debug
2. copy freeglut-MSVC-3.0.0-2.mp\freeglut\lib\x64\freeglut.lib -> (projetc name)\(projetc name)\library
3. copy dir freeglut-MSVC-3.0.0-2.mp\freeglut\include  -> (projetc name)\(projetc name)\


Visual

properties -> Linker -> generate -> addtioanl lib dir -> .\library
-----------------------------------------------------------------------------
How to use
you Open 5 fingers -> Rotate Polygon


When you want to just move your hand location, grab your hand
