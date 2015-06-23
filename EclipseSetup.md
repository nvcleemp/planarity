# Introduction #

This page tells you how to set up Eclipse to work on this project.  Other environments can be used if desired, in which case analogous setup processes should be used.

These instructions are for **Eclipse 4.3 (Kepler)** and the corresponding Subversive plugin for SVN support. Many earlier versions of Eclipse have also been extensively used with this project.

# Details #

  1. **Install Eclipse Development Environment**
    1. For Windows, get the MinGW C/C++ compiler (MinGW version 5.1.3 or higher) and install to eclipse/cpp-mingw (or a similar location outside of project space)
    1. For Windows, also get the GDB debugger version 6.6 or higher (gdb-6.6.zip) and unzip into the MinGW/bin directory
    1. For Windows, ensure the MinGW/bin directory is on the path
    1. Obtain the Eclipse IDE for Java EE (preferred) or the IDE for C/C++ developers from http://www.eclipse.org/downloads/
    1. Create a directory eclipse/zipfiles and put the download there
    1. Extract to eclipse/43
    1. Eclipse executable will be in the directory eclipse/43/eclipse
    1. If the preferred Java EE environment was installed, then add the C++ developer tools to it by using Help | Install New Software | All Available Sites | Programming Languages, then Add the C++ Development Tools plugin and optionally the API/Library Help plugin.
    1. Launch eclipse and create a 'Planarity' workspace directory within directory eclipse/workspaces-cpp
      * You're obviously free to adopt any directory structure you want, but an organization like this allows your source code to be independent of eclipse installations, e.g. upgrading from eclipse/42-Juno did not require moving the workspaces-cpp folder.
  1. **Install Subversive SVN plugins**
    1. In Eclipse, choose menu Help | Install New Software
    1. Must install Subversive plugin
      * In update site picker "Work with:", choose All Available Sites
      * Open Collaboration Tools
      * Choose the checkbox for Subversive SVN Team Provider
      * Optionally add the new Revision Graph plugin
      * Finish
    1. The installation will restart Eclipse.
    1. On restart, the SVN team provider will launch an installer to get an SVN Kit (this may only happen during environment setup below)
      * Choose the latest SVN Kit (currently 1.8.0)
  1. **Environment set up**
    1. Window | Preferences
    1. Go to General | Editors | Text Editors
      * Set "Displayed Tab width: 4" and check "Insert spaces for tabs"
    1. Go to Team | Ignore Resources, click Add Pattern... and add ".metadata" to the svn:ignore patterns
    1. Go to Team | SVN | Properties Configuration
    1. On the Automatic Properties tab, click Add button to add each of the following
```
	    Filename template   Properties
	    *.pdf               svn:mime-type=application/pdf
	    *.png               svn:mime-type=image/png
	    *.gif               svn:mime-type=image/gif
	    *.jpg               svn:mime-type=image/jpeg
            *.tif               svn:mime-type=image/tiff
            *.h                 svn:eol-style=native
            *.c                 svn:eol-style=native
            *.cpp               svn:eol-style=native
            *.java              svn:eol-style=native
```