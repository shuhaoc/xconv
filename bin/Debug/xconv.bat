@echo off
for /r %1 %%f in (*.cpp *.h) do chkconv %%f && echo %%f
@echo on