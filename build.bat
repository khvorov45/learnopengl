pushd build
cl -Od -Z7 -nologo -TC ../code/main.c -link User32.lib Gdi32.lib Opengl32.lib
popd
echo done
