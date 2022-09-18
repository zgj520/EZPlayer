set DEPOT_TOOLS_UPDATE=0
set PATH=D:\\depot_tools;%PATH%

cd angle
python scripts/bootstrap.py
gclient sync

set DEPOT_TOOLS_WIN_TOOLCHAIN=0
gn gen out/Release
gn args out/Release --list --args="target_cpu=\"x64\" is_clang=false is_debug=false treat_warnings_as_errors=false"
autoninja -C out/Release