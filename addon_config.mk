meta:
	ADDON_NAME = ofxFBX
	ADDON_DESCRIPTION = Addon for importing fbx scenes using the autodesk fbx sdk
	ADDON_AUTHOR = OF Team
	ADDON_TAGS = "3d models"
	ADDON_URL = http://github.com/arturoc/ofxFBX

common:
	# dependencies with other addons, a list of them separated by spaces 
	# or use += in several lines
	ADDON_DEPENDENCIES =
	
	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	ADDON_INCLUDES =
	
	# any special flag that should be passed to the compiler when using this
	# addon
	ADDON_CFLAGS =
	
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	ADDON_LDFLAGS = 
	
	# linux only, any library that should be included in the project using
	# pkg-config
	ADDON_PKG_CONFIG_LIBRARIES =
	
	# osx/iOS only, any framework that should be included in the project
	ADDON_FRAMEWORKS =
	
	# source files, these will be usually parsed from the file system looking
	# in the src folders in libs and the root of the addon. if your addon needs
	# to include files in different places or a different set of files per platform
	# they can be specified here
	ADDON_SOURCES =
	
	# some addons need resources to be copied to the bin/data folder of the project
	# specify here any files that need to be copied, you can use wildcards like * and ?
	ADDON_DATA = 
	
	# when parsing the file system looking for libraries exclude this for all or
	# a specific platform
	ADDON_LIBS_EXCLUDE = 
	
linux64:
	ADDON_LIBS = libs/fbxsdk/lib/linux64/libfbxsdk-2013.2-static.a
	ADDON_LDFLAGS = -ldl
	
linux:
	ADDON_LIBS = libs/fbxsdk/lib/linux/libfbxsdk-2013.2-static.a
	ADDON_LDFLAGS = -ldl
	
win_cb:
	
linuxarmv6l:
	
	
linuxarmv7l:

android/armeabi:	
	
android/armeabi-v7a:	
