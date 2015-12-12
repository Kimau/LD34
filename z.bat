cls
echo off
cd Source
for %%v in (*.cpp) do clang-format -i -style=Google "%%v"
for %%v in (*.h) do clang-format -i -style=Google "%%v"
echo "--------------- Formatted folders -----------------------------"
cd ..
echo on