pushd build
cl -Od -Z7 -nologo -TC  ../code/main.c
popd
echo done
