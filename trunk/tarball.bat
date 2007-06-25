@call version
@mkdir ..\stb_imv_src-%VERSION%
@mkdir ..\stb_imv_src-%VERSION%\vc6
@copy vc6\stb_imv.ds? ..\stb_imv_src-%VERSION%\vc6
@copy *.c ..\stb_imv_src-%VERSION%
@copy *.h ..\stb_imv_src-%VERSION%
@copy COPYING ..\stb_imv_src-%VERSION%
@copy *.txt ..\stb_imv_src-%VERSION%
@copy *.bat ..\stb_imv_src-%VERSION%
@cd ..
@del stb_imv_src-%VERSION%.zip
@zip -r stb_imv_src-%VERSION%.zip stb_imv_src-%VERSION%
@del /s /q stb_imv_src-%VERSION%\*
@rmdir stb_imv_src-%VERSION%\vc6
@rmdir stb_imv_src-%VERSION%
@cd stb-imv