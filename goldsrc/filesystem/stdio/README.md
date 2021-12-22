# File system module (Stdio)

## Overview

This is the stdio (Standard I/O) implementation of the filesystem module. In order to implement it, the original Quake fs code (including .pak support) was ripped off the engine and moved over here.

The reason behind the fs code being separated into its own module is that in order to make the engine work with Steam Valve needed to implement the fs functionality differently so they wrote a filesystem interface and implemented it as two modules. One of them - using the standard i/o funcs and another one using the Steam APIs.

The code here still requires some tweaks and cleaning and most likely eventually will be rewritten to use the C++17's std::filesystem library.