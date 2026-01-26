@echo off
pushd bin
for /d %%d in (*.*) do (
   for %%a in (%%d\*) do (
      move %%a %cd%
   )
)
popd
