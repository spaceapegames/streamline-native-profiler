# Introduction
This Android native plugin enables the user to capture more detaild information from their Unity game using Arm's DS-5 Streamline profiler. Profiler tags (Profiler.BeginSample/EndSample) will appear in your trace, this includes both Unity and user submitted samples.

Using the Uniy IUnityProfilerCallbacks interface, profile tags are forwarde onto Arm's DS-5 Streamline toolchain enabling more verbose output and information from your captures.

# Usage
1) Copy the contents of Assets directory to your project (it will be easier if you retain the path, then the plugin importer will set CPU architecture for the .so's correctly)
2) Attach the Assets/Scripts/StreamlinePlugin.cs script to a game object of your choice in your first scene to make sure the plugin is loaded as early as possible

# Building
1) Open a terminal and navigate to your NDK install eg
```
cd /Users/MyName/Library/Android/sdk/ndk-bundle/
```
2) Build the project by calling the ndk-build program and providing it the path of the repo src folder eg
```
./ndk-build -C /Users/MyName/repos/shared/streamline-native-profiler/src
```
3) Copy the libraries in src/libs/ to Assets/Plugins/Android/libs
