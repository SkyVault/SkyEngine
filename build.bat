CALL "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"

"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/MSBuild.exe" Benis3D.sln /p:DebugSymbols=true /p:DebugType=Z7 /p:useenv=true /p:Platform=x64 /t:Build