mkdir ..\stb_imv
copy release\stb_imv.exe ..\stb_imv\imv.exe
copy readme_binary.txt ..\stb_imv\readme.txt
cd ..
del stb_imv-version.zip
zip -r stb_imv-version.zip stb_imv
