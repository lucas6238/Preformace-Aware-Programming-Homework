@echo off

subst w: c:\Projects 
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
set path=w:\PAP\bin;%PATH%
set path=C:\nasm-2.16.01-win32\nasm-2.16.01;%PATH%
w:
cd PAP\code