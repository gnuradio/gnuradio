@echo off
setlocal enableextensions enabledelayedexpansion

rem INPUTS (environment variables that need to be set before calling this script):
rem
rem CI (azure/github_actions/UNSET)
rem CI_RUN_ID (unique identifier for the CI job run)
rem CONDA_BLD_PATH (path to the conda-bld directory)
rem CONFIG (build matrix configuration string)
rem CONFIG_SHORT (uniquely-shortened configuration string)
rem FEEDSTOCK_NAME
rem Optional:
rem ARTIFACT_STAGING_DIR (use working directory if unset)
rem BLD_ARTIFACT_PREFIX (prefix for the conda build artifact name, skip if unset)
rem ENV_ARTIFACT_PREFIX (prefix for the conda build environments artifact name, skip if unset)
rem WRK_ARTIFACT_PREFIX (prefix for the conda work directory artifact name, skip if unset)

rem OUTPUTS
rem
rem BLD_ARTIFACT_NAME
rem BLD_ARTIFACT_PATH
rem ENV_ARTIFACT_NAME
rem ENV_ARTIFACT_PATH
rem WRK_ARTIFACT_NAME
rem WRK_ARTIFACT_PATH
call "%MINIFORGE_HOME%\Scripts\activate.bat"
if !errorlevel! neq 0 exit /b !errorlevel!

rem Check that the conda-build directory exists
if not exist %CONDA_BLD_PATH% (
    echo conda-build directory does not exist
    exit 1
)

if not defined ARTIFACT_STAGING_DIR (
    rem Set staging dir to the working dir
    set ARTIFACT_STAGING_DIR=%cd%
)

rem Set a unique ID for the artifact(s), specialized for this particular job run
set ARTIFACT_UNIQUE_ID=%CI_RUN_ID%_%CONFIG%
if not "%ARTIFACT_UNIQUE_ID%" == "%ARTIFACT_UNIQUE_ID:~0,80%" (
    set ARTIFACT_UNIQUE_ID=%CI_RUN_ID%_%CONFIG_SHORT%
)

rem --use-compress-prog hangs on Azure, but we can use bsdtar unconditionally.
set "ZSTD=--zstd --options=zstd:compression-level=12,zstd:threads=0"

rem Mirror the logic from create_conda_build_artifacts.sh.tmpl.
rem Note that the paths to tar must use forward slashes.

cd "%CONDA_BLD_PATH%"
if errorlevel 1 exit 1

set ARTIFACT_PATHS=
set ENVIRONMENT_PATHS=
set "EXCLUDE_FROM_WORK=--exclude=.git --exclude=./pkg_cache --exclude=./src_cache"

for /d %%a in (linux-* osx-* win-* broken noarch) do (
    if exist %%a (
        set "ARTIFACT_PATHS=!ARTIFACT_PATHS! %%a"
        set "EXCLUDE_FROM_WORK=!EXCLUDE_FROM_WORK! --exclude=%%a"
    )
)
for %%a in (channeldata.json index.html) do (
    if exist %%a (
        set "ARTIFACT_PATHS=!ARTIFACT_PATHS! %%a"
        set "EXCLUDE_FROM_WORK=!EXCLUDE_FROM_WORK! --exclude=%%a"
    )
)
for /d %%a in (. bld test) do (
    if exist %%a (
        pushd %%a
        if errorlevel 1 exit 1

        for /d %%b in (*_*) do (
            if exist %%a\%%b\pip_cache (
                set "EXCLUDE_FROM_WORK=!EXCLUDE_FROM_WORK! --exclude=%%a/%%b/pip_cache"
            )

            pushd %%b
            if errorlevel 1 exit 1

            for /d %%c in (*_env* *_prefix_moved_*) do (
                set "ENVIRONMENT_PATHS=!ENVIRONMENT_PATHS! %%a/%%b/%%c"
                set "EXCLUDE_FROM_WORK=!EXCLUDE_FROM_WORK! --exclude=%%a/%%b/%%c"
            )

            popd
            if errorlevel 1 exit 1
        )

        popd
        if errorlevel 1 exit 1
    )
)

rem Make the build artifact archive
if defined BLD_ARTIFACT_PREFIX (
    if not "%ARTIFACT_PATHS%" == "" (
        echo Creating build artifact archive ...
        set BLD_ARTIFACT_NAME=%BLD_ARTIFACT_PREFIX%_%ARTIFACT_UNIQUE_ID%

        set "BLD_ARTIFACT_PATH=%ARTIFACT_STAGING_DIR%\%FEEDSTOCK_NAME%_%BLD_ARTIFACT_PREFIX%_%ARCHIVE_UNIQUE_ID%.tar.zst"
        bsdtar -c -f "!BLD_ARTIFACT_PATH!" %ZSTD% %ARTIFACT_PATHS%
        if errorlevel 1 (
            if exist "!BLD_ARTIFACT_PATH!" (
                move "!BLD_ARTIFACT_PATH!" "!BLD_ARTIFACT_PATH:.tar.zst=-broken.tar.zst!"
                set "BLD_ARTIFACT_PATH=!BLD_ARTIFACT_PATH:.tar.zst=-broken.tar.zst!"
            )
        )

        if exist "!BLD_ARTIFACT_PATH!" (
            echo Archive created:
            dir "!BLD_ARTIFACT_PATH!"

            if "%CI%" == "azure" (
                echo ##vso[task.setVariable variable=BLD_ARTIFACT_NAME]!BLD_ARTIFACT_NAME!
                echo ##vso[task.setVariable variable=BLD_ARTIFACT_PATH]!BLD_ARTIFACT_PATH!
            )
            if "%CI%" == "github_actions" (
                echo BLD_ARTIFACT_NAME=!BLD_ARTIFACT_NAME!>> !GITHUB_OUTPUT!
                echo BLD_ARTIFACT_PATH=!BLD_ARTIFACT_PATH!>> !GITHUB_OUTPUT!
            )
        ) else (
            echo No archive created
        )
    ) else (
        echo Skipping work directory artifact archive because of no files
    )
) else (
    echo Skipping build artifact archive
)
echo.

rem Make the work directory artifact archive
if defined WRK_ARTIFACT_PREFIX (
    echo Creating work directory archive ...
    set WRK_ARTIFACT_NAME=!WRK_ARTIFACT_PREFIX!_%ARTIFACT_UNIQUE_ID%

    set "WRK_ARTIFACT_PATH=%ARTIFACT_STAGING_DIR%\%FEEDSTOCK_NAME%_%WRK_ARTIFACT_PREFIX%_%ARCHIVE_UNIQUE_ID%.tar.zst"
    bsdtar -c -f "!WRK_ARTIFACT_PATH!" %ZSTD% %EXCLUDE_FROM_WORK% .
    if errorlevel 1 (
        if exist "!WRK_ARTIFACT_PATH!" (
            move "!WRK_ARTIFACT_PATH!" "!WRK_ARTIFACT_PATH:.tar.zst=-broken.tar.zst!"
            set "WRK_ARTIFACT_PATH=!WRK_ARTIFACT_PATH:.tar.zst=-broken.tar.zst!"
        )
    )

    if exist "!WRK_ARTIFACT_PATH!" (
        echo Archive created:
        dir "!WRK_ARTIFACT_PATH!"

        if "%CI%" == "azure" (
            echo ##vso[task.setVariable variable=WRK_ARTIFACT_NAME]!WRK_ARTIFACT_NAME!
            echo ##vso[task.setVariable variable=WRK_ARTIFACT_PATH]!WRK_ARTIFACT_PATH!
        )
        if "%CI%" == "github_actions" (
            echo WRK_ARTIFACT_NAME=!WRK_ARTIFACT_NAME!>> !GITHUB_OUTPUT!
            echo WRK_ARTIFACT_PATH=!WRK_ARTIFACT_PATH!>> !GITHUB_OUTPUT!
        )
    ) else (
        echo No archive created
    )
) else (
    echo Skipping work directory artifact archive
)
echo.

rem Make the environment artifact archive
if defined ENV_ARTIFACT_PREFIX (
    if not "%ENVIRONMENT_PATHS%" == "" (
        echo Creating build environment artifact archive ...
        set ENV_ARTIFACT_NAME=!ENV_ARTIFACT_PREFIX!_%ARTIFACT_UNIQUE_ID%

        set "ENV_ARTIFACT_PATH=%ARTIFACT_STAGING_DIR%\%FEEDSTOCK_NAME%_%ENV_ARTIFACT_PREFIX%_%ARCHIVE_UNIQUE_ID%.tar.zst"
        bsdtar -c -f "!ENV_ARTIFACT_PATH!" %ZSTD% %ENVIRONMENT_PATHS%
        if errorlevel 1 (
            if exist "!ENV_ARTIFACT_PATH!" (
                move "!ENV_ARTIFACT_PATH!" "!ENV_ARTIFACT_PATH:.tar.zst=-broken.tar.zst!"
                set "ENV_ARTIFACT_PATH=!ENV_ARTIFACT_PATH:.tar.zst=-broken.tar.zst!"
            )
        )

        if exist "!ENV_ARTIFACT_PATH!" (
            echo Archive created:
            dir "!ENV_ARTIFACT_PATH!"

            if "%CI%" == "azure" (
                echo ##vso[task.setVariable variable=ENV_ARTIFACT_NAME]!ENV_ARTIFACT_NAME!
                echo ##vso[task.setVariable variable=ENV_ARTIFACT_PATH]!ENV_ARTIFACT_PATH!
            )
            if "%CI%" == "github_actions" (
                echo ENV_ARTIFACT_NAME=!ENV_ARTIFACT_NAME!>> !GITHUB_OUTPUT!
                echo ENV_ARTIFACT_PATH=!ENV_ARTIFACT_PATH!>> !GITHUB_OUTPUT!
            )
        ) else (
            echo No archive created
        )
    ) else (
        echo Skipping environment artifact archive because of no files
    )
) else (
    echo Skipping environment artifact archive
)
echo.
