@echo off

set CommonCompilerFlags= -nologo -MT  -arch:AVX2 -Od -W4 -wd4100 -wd4189 -wd4201 -wd4309 -wd4702 -Gm- -GR- -EHa- -FC -Zi -Wv:18 -wd4244 -WX


REM pushd ..\resdump
REM del my.res
REM RC /r /nologo my.rc 

pushd ..\build

cl  -c -nologo  /Fdpap /EHsc   %CommonCompilerFlags% ..\code\win32_pap.cpp -Fmwin32_pap.map
link win32_pap.obj /SUBSYSTEM:CONSOLE -nologo -debug:full -incremental:no kernel32.lib user32.lib Gdi32.lib winmm.lib Ws2_32.lib w:/pap/resdump/my.res 



