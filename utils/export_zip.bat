cd ..
mkdir temp_zip
mkdir temp_zip\src
xcopy src temp_zip\src /E
copy doc\*.pdf temp_zip\
"C:\Program Files\7-Zip\7z.exe" a -aot ..\CbsdMixer-ZiniMatteo_533197.zip .\temp_zip\*
rmdir /S /Q temp_zip 
