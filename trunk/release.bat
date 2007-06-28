+@echo off
call version.bat

rem # Update ReleaseNotes on wiki

cd wiki
svn up
copy /b /y ..\notes_header.wiki+..\notes.txt ReleaseNotes.wiki
svn ci -m "release %VERSION%"
cd ..

rem # Make release directory

mkdir ..\stb_imv
copy vc6\release\stb_imv.exe ..\stb_imv\imv.exe
copy readme_binary.txt+notes.txt ..\stb_imv\readme.txt

rem # Make tarball directory

mkdir ..\stb_imv_src-%VERSION%
mkdir ..\stb_imv_src-%VERSION%\vc6
copy vc6\stb_imv.ds? ..\stb_imv_src-%VERSION%\vc6
copy *.* ..\stb_imv_src-%VERSION%

rem #

cd ..

rem # zip release directory

del stb_imv-%VERSION%.zip
zip -r stb_imv-%VERSION%.zip stb_imv
del /q stb_imv\*
rmdir stb_imv

rem # zip tarball directory

del stb_imv_src-%VERSION%.zip
zip -r stb_imv_src-%VERSION%.zip stb_imv_src-%VERSION%
del /s /q stb_imv_src-%VERSION%\*
rmdir stb_imv_src-%VERSION%\vc6
rmdir stb_imv_src-%VERSION%

rem # upload

call upload.bat

cd stb-imv
