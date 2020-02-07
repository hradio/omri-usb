# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/thirdparty/fec)
add_subdirectory(src/main/cpp/thirdparty/fec)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             irtdab

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/platformspecific/android/native-lib.cpp
             src/main/cpp/fig_00_ext_05.cpp
             src/main/cpp/fig_00_ext_13.cpp
             src/main/cpp/fig_00_ext_21.cpp
             src/main/cpp/fig_01_ext_04.cpp
             src/main/cpp/fig_00_ext_00.cpp
             src/main/cpp/fig_00_ext_06.cpp
             src/main/cpp/fig_00_ext_14.cpp
             src/main/cpp/fig_00_ext_24.cpp
             src/main/cpp/fig_01_ext_05.cpp
             src/main/cpp/fig_00_ext_01.cpp
             src/main/cpp/fig_00_ext_07.cpp
             src/main/cpp/fig_00_ext_17.cpp
             src/main/cpp/fig_00_ext_25.cpp
             src/main/cpp/fig_01_ext_06.cpp
             src/main/cpp/fig_00_ext_02.cpp
             src/main/cpp/fig_00_ext_08.cpp
             src/main/cpp/fig_00_ext_18.cpp
             src/main/cpp/fig_00_ext_26.cpp
             src/main/cpp/fig_00_ext_03.cpp
             src/main/cpp/fig_00_ext_09.cpp
             src/main/cpp/fig_00_ext_19.cpp
             src/main/cpp/fig_01_ext_00.cpp
             src/main/cpp/fig_00_ext_04.cpp
             src/main/cpp/fig_00_ext_10.cpp
             src/main/cpp/fig_00_ext_20.cpp
             src/main/cpp/fig_01_ext_01.cpp
             src/main/cpp/dabservicecomponentmscstreamaudio.cpp
             src/main/cpp/motdecoder.cpp
             src/main/cpp/dynamiclabeldecoder.cpp
             src/main/cpp/slideshowdecoder.cpp
             src/main/cpp/mscdatagroupdecoder.cpp
             src/main/cpp/dabservicecomponentmscpacketdata.cpp
             src/main/cpp/dabuserapplicationdecoder.cpp
             src/main/cpp/dabmotuserapplicationdecoder.cpp
             src/main/cpp/dabservicecomponentdecoder.cpp
             src/main/cpp/dabuserapplication.cpp
             src/main/cpp/dabservicecomponent.cpp
             src/main/cpp/dabservice.cpp
             src/main/cpp/dabplusservicecomponentdecoder.cpp
             src/main/cpp/dabmpegservicecomponentdecoder.cpp
             src/main/cpp/dabservicecomponentmscstreamdata.cpp
             src/main/cpp/paddecoder.cpp
             src/main/cpp/dabensemble.cpp
             src/main/cpp/dabservicecomponentmscstream.cpp
             src/main/cpp/ficparser.cpp
             src/main/cpp/platformspecific/android/raontunerinput.cpp
             src/main/cpp/platformspecific/android/jusbdevice.cpp
             src/main/cpp/platformspecific/android/jtunerusbdevice.cpp
             src/main/cpp/platformspecific/android/jdabservice.cpp
             src/main/cpp/platformspecific/android/ediinput.cpp
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       irtdab

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       fec
                       )

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE MinSizeRel)
