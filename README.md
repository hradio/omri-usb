# OMRI USB

This is an implementation of the [OpenMobileRadioInterface API](https://github.com/hradio/OMRI) 
project.
  
It compiles to an Android .aar library

Features:
*   Supports some hardware USB DAB receivers
*   Supports playback of ShoutCast streams
*   Support for EDI streams
*   Full C++ DAB stack for DAB/EDI decoding including
    * DAB Band III service scanner
    * DAB servicecomponent decoder
    * DAB+ servicecomponent decoder
    * Dynamic Label (Plus) (DLS / DL+) decoder
    * Enhanced Slideshow (SLS) decoder
*   Java/Android JNI layer for easy integration
*   RadioDns service scanner for IP services and metadata enrichment  

## Compile

You must have the Android SDK and NDK installed.  
Use the gradle wrapper command line or import the project into AndroidStudio

## TODO

* DAB Ensemble reconfiguration 