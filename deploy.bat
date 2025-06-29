@echo off

set offline_build_dir=.\offline-build
set github_page_dir=.\keaganbell.github.io

del /q /f "%github_page_dir%\content\*" >nul 2>&1
del /q /f "%github_page_dir%\resources\*" >nul 2>&1
del /q /f "%github_page_dir%\*.js" >nul 2>&1
del /q /f "%github_page_dir%\*.wasm" >nul 2>&1
del /q /f "%github_page_dir%\*.html" >nul 2>&1

xcopy "%offline_build_dir%\*" "%github_page_dir%\*" /s /e /y /i

pushd %github_page_dir%
git add --all
git commit -m "message: %~1"
git push
popd
