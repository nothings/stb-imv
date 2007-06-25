@call version.bat
@mkdir ..\stb_imv
@copy vc6\release\stb_imv.exe ..\stb_imv\imv.exe
@copy readme_binary.txt ..\stb_imv\readme.txt
@cd ..
@del stb_imv-%VERSION%.zip
@zip -r stb_imv-%VERSION%.zip stb_imv
@del /q stb_imv\*
@rmdir stb_imv
@cd stb-imv