if not exist "FINAL_BUILD" mkdir "FINAL_BUILD"
xcopy resources FINAL_BUILD\resources  /E/H/C/I/Y
xcopy *.dll FINAL_BUILD /Y
xcopy Release\Benis3D.exe FINAL_BUILD /Y 
powershell Compress-Archive -Force FINAL_BUILD\* FINAL_BUILD/FINAL_BUILD.zip