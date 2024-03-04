@echo off

rem description: "Bootstrap Gobo Eiffel Compiler in $GOBO/bin"
rem copyright: "Copyright (c) 2016-2024, Eric Bezault and others"
rem license: "MIT License"


rem "usage: bootstrap.bat [-v|-s] [<c_compiler>]"


if .%1. == .-v. goto verbose
if .%1. == .-s. goto silent
goto no_verbose

:no_verbose
	set VERBOSE=
	set CC=%1
	set EIF=ge
	goto do_it

:verbose
	set VERBOSE=-v
	set CC=%2
	set EIF=ge
	goto do_it

:silent
	set VERBOSE=-s
	set CC=%2
	set EIF=ge
	goto do_it

:do_it
	if not .%VERBOSE%. == .-s. echo Executing bootstrap.bat...
	if ".%GOBO%." == ".." goto gobo
	goto windows

:gobo
	echo Environment variable GOBO must be set
	set GOBO=%~dp0\..\..\..
	echo Set $GOBO to "%GOBO%"
	goto windows

:windows
	set MV=rename
	set RM=del
	set BIN_DIR=%GOBO%\bin
	set BOOTSTRAP_DIR=%GOBO%\tool\gec\bootstrap
	set OBJ=.obj
	set EXE=.exe
	goto c_compiler

:c_compiler

set BIN_DIR=%GOBO%\bin
set BOOTSTRAP_DIR=%GOBO%\tool\gec\bootstrap
cd %BIN_DIR%

if .%CC%. == .. set CC=zig
if .%CC%. == .-help. goto usage
if .%CC%. == .--help. goto usage
if .%CC%. == .-h. goto usage
if .%CC%. == .-?. goto usage
if .%CC%. == ./h. goto usage
if .%CC%. == ./?. goto usage
if .%CC%. == .-v. goto usage
if .%CC%. == .-s. goto usage
if .%EIF%. == .. goto usage

if .%CC%. == .msc. goto msc
if .%CC%. == .cl. goto msc
if .%CC%. == .lcc-win32. goto lcc-win32
if .%CC%. == .lcc-win64. goto lcc-win64
if .%CC%. == .lcc. goto lcc-win64
if .%CC%. == .bcc. goto bcc32
if .%CC%. == .bcc32. goto bcc32
if .%CC%. == .gcc. goto gcc
if .%CC%. == .mingw. goto mingw
if .%CC%. == .clang. goto clang
if .%CC%. == .zig. goto zig
if .%CC%. == .cc. goto cc
if .%CC%. == .icc. goto icc
if .%CC%. == .tcc. goto tcc
if .%CC%. == .no_c. goto install
echo Unknown C compiler: %CC%
goto exit

:msc
	set CC=cl
	set LD=link
	set CFLAGS=-O2 -nologo -wd4049
	set LFLAGS=-nologo -subsystem:console
	set LFLAG_OUT=-out:
	set LLIBS=
	echo msc > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:bcc32
	set CC=bcc32
	set LD=bcc32
	set CFLAGS=-5 -q -w-8004 -w-8008 -w-8057 -w-8065 -w-8066 -w-8070 -O2
	set LFLAGS=-5 -q 
	set LFLAG_OUT=-e
	set LLIBS=
	echo bcc > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:lcc-win32
	set CC=lcc
	rem set CFLAGS=-O   -- Problem when gec is compiled with the -O option.
	set CFLAGS=
	set LD=lcclnk
	set LFLAGS=-s -subsystem Console
	set LFLAG_OUT=-o 
	set LLIBS=
	echo lcc-win32 > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:lcc-win64
	set CC=lcc64
	rem set CFLAGS=-O   -- Problem when gec is compiled with the -O option.
	set CFLAGS=
	set LD=lcclnk64
	set LFLAGS=-s -subsystem Console
	set LFLAG_OUT=-o 
	set LLIBS=
	echo lcc-win64 > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:gcc
	set CC=gcc
	set LD=gcc
	set CFLAGS=-O2
	set LFLAGS=
	set LFLAG_OUT=-o 
	set LLIBS=-lm -pthread
	set OBJ=.o
	echo gcc > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:mingw
	set CC=gcc
	set LD=gcc
	set CFLAGS=-O2
	set LFLAGS=
	set LFLAG_OUT=-o 
	set LLIBS=-lm -pthread
	set OBJ=.o
	echo mingw > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:clang
	set CC=clang
	set LD=clang
	set CFLAGS=-pthread -Wno-unused-value -Wno-deprecated-declarations -fno-strict-aliasing -Os -DNDEBUG
	set LFLAGS=-pthread -fno-strict-aliasing -Os -DNDEBUG
	set LFLAG_OUT=-o 
	set LLIBS=
	set OBJ=.obj
	echo clang > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:zig
	if not ".%ZIG%." == ".." goto zig_defined
	set ZIG=%GOBO%\tool\gec\backend\c\zig
	if exist "%ZIG%" goto zig_in_gobo
	set ZIG=zig
	goto zig_defined
:zig_in_gobo
	set ZIG=%ZIG%\zig
:zig_defined
	set CC="%ZIG%" cc
	set LD="%ZIG%" cc
	set CFLAGS=-pthread -Wno-unused-value -Wno-deprecated-declarations -fno-sanitize=undefined -fno-strict-aliasing -fno-sanitize=cfi -Os -DNDEBUG
	set LFLAGS=-pthread -Os -DNDEBUG
	set LFLAG_OUT=-o 
	set LLIBS=
	set OBJ=.obj
	echo zig > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:cc
	set CC=cc
	set LD=cc
	set CFLAGS='-fast'
	set LFLAGS='-lm -pthread'
	set LFLAG_OUT=-o 
	set LLIBS=
	set OBJ=.o
	echo cc > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:icc
	set CC=icc
	set LD=icc
	set CFLAGS=-O2
	set LFLAGS=
	set LFLAG_OUT=-o 
	set LLIBS=
	set OBJ=.o
	echo icc > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:tcc
	set CC=tcc
	set LD=tcc
	set CFLAGS=-O2
	set LFLAGS=-lm
	set LFLAG_OUT=-o 
	set LLIBS=
	set OBJ=.o
	echo tcc > "%GOBO%\tool\gec\backend\c\config\default.cfg"
	goto c_compilation

:c_compilation
	if not .%VERBOSE%. == .-s. echo Compiling gec (bootstrap 0)...
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec8.c
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec7.c
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec6.c
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec5.c
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec4.c
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec3.c
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec2.c
	%CC% %CFLAGS% -c %BOOTSTRAP_DIR%\gec1.c
	%LD% %LFLAGS% %LFLAG_OUT%gec%EXE% gec1%OBJ% gec2%OBJ% gec3%OBJ% gec4%OBJ% gec5%OBJ% gec6%OBJ% gec7%OBJ% gec8%OBJ% %LLIBS%
	%RM% gec*%OBJ%
	if ".%CC%." == ".bcc32." %RM% gec.tds
	goto install

:install
	if .%EIF%. == .ge. goto ge
	echo Unknown Eiffel compiler: %EIF%
	goto exit

:ge
	cd "%BIN_DIR%"
	rem Compile gec twice to get a bootstrap effect.
	if not .%VERBOSE%. == .-s. echo Compiling gec (bootstrap 1)...
	%MV% "%BIN_DIR%\gec%EXE%" gec1%EXE%
	"%BIN_DIR%\gec1%EXE%" --finalize "%GOBO%\tool\gec\src\system.ecf"
	%RM% "%BIN_DIR%\gec1%EXE%"
	if not .%VERBOSE%. == .-s. echo Compiling gec (bootstrap 2)...
	%MV% "%BIN_DIR%\gec%EXE%" gec1%EXE%
	"%BIN_DIR%\gec1%EXE%" --finalize "%GOBO%\tool\gec\src\system.ecf"
	%RM% "%BIN_DIR%\gec1%EXE%"
	%RM% gec*.h
	%RM% gec*.c
	%RM% gec*%OBJ%
	rem Make sure 'gec.bat' exists to avoid getting some warning when removing it.
	echo "" > gec.bat
	%RM% gec*.bat
	rem Make sure 'gec.sh' exists to avoid getting some warning when removing it.
	echo "" > gec.sh
	%RM% gec*.sh
	rem Make sure 'gec.make' exists to avoid getting some warning when removing it.
	echo "" > gec.make
	%RM% gec*.make
	if ".%CC%." == ".bcc32." %RM% gec.tds
	goto exit

:usage
	echo usage: bootstrap.bat [-v^|-s] [^<c_compiler^>]
	echo    c_compiler:  zig ^| gcc ^| clang ^| msc ^| lcc-win32 ^| lcc-win64 ^| bcc ^| mingw ^| cc ^| icc ^| tcc ^| no_c
	echo    default c_compiler: zig
	goto exit

:exit
