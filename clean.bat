CALL "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars32.bat"

"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/MSBuild.exe" Benis3D.sln -target:Clean
del x64\Debug\*.exe
del x64\Debug\*.obj
del x64\Release\*.obj
del x64\Release\*.exe
