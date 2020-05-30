call build-realease.bat
if not exist "FINAL_BUILD" mkdir "FINAL_BUILD"
xcopy resources FINAL_BUILD\resources  /E/H/C/I/Y
xcopy *.dll FINAL_BUILD /Y
xcopy x64\Release\Benis3D.exe FINAL_BUILD /Y 
powershell Compress-Archive -Force FINAL_BUILD\* FINAL_BUILD/FINAL_BUILD.zip