인텔리얼센스 sdk 설정법

Create new project with empty project visual c++
Create cpp files with named ‘main’
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

opencv 3.10 종속절

opencv_world310d.lib
opencv_world310.lib