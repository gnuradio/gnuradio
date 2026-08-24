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

rem OUTPUTS
rem
rem BLD_ARTIFACT_NAME
rem BLD_ARTIFACT_PATH
rem ENV_ARTIFACT_NAME
rem ENV_ARTIFACT_PATH

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

rem Make the build artifact zip
if defined BLD_ARTIFACT_PREFIX (
    set BLD_ARTIFACT_NAME=%BLD_ARTIFACT_PREFIX%_%ARTIFACT_UNIQUE_ID%
    echo BLD_ARTIFACT_NAME: !BLD_ARTIFACT_NAME!

    set "BLD_ARTIFACT_PATH=%ARTIFACT_STAGING_DIR%\%FEEDSTOCK_NAME%_%BLD_ARTIFACT_PREFIX%_%ARCHIVE_UNIQUE_ID%.zip"
    7z a "!BLD_ARTIFACT_PATH!" "%CONDA_BLD_PATH%" -xr^^!.git/ -xr^^!_*_env*/ -xr^^!*_cache/ -bb
    if errorlevel 1 exit 1
    echo BLD_ARTIFACT_PATH: !BLD_ARTIFACT_PATH!

    if "%CI%" == "azure" (
        echo ##vso[task.setVariable variable=BLD_ARTIFACT_NAME]!BLD_ARTIFACT_NAME!
        echo ##vso[task.setVariable variable=BLD_ARTIFACT_PATH]!BLD_ARTIFACT_PATH!
    )
    if "%CI%" == "github_actions" (
        echo BLD_ARTIFACT_NAME=!BLD_ARTIFACT_NAME!>> !GITHUB_OUTPUT!
        echo BLD_ARTIFACT_PATH=!BLD_ARTIFACT_PATH!>> !GITHUB_OUTPUT!
    )
)

rem Make the environments artifact zip
if defined ENV_ARTIFACT_PREFIX (
    set ENV_ARTIFACT_NAME=!ENV_ARTIFACT_PREFIX!_%ARTIFACT_UNIQUE_ID%
    echo ENV_ARTIFACT_NAME: !ENV_ARTIFACT_NAME!

    set "ENV_ARTIFACT_PATH=%ARTIFACT_STAGING_DIR%\%FEEDSTOCK_NAME%_%ENV_ARTIFACT_PREFIX%_%ARCHIVE_UNIQUE_ID%.zip"
    7z a "!ENV_ARTIFACT_PATH!" -r "%CONDA_BLD_PATH%"/_*_env*/ -bb
    if errorlevel 1 exit 1
    echo ENV_ARTIFACT_PATH: !ENV_ARTIFACT_PATH!

    if "%CI%" == "azure" (
        echo ##vso[task.setVariable variable=ENV_ARTIFACT_NAME]!ENV_ARTIFACT_NAME!
        echo ##vso[task.setVariable variable=ENV_ARTIFACT_PATH]!ENV_ARTIFACT_PATH!
    )
    if "%CI%" == "github_actions" (
        echo ENV_ARTIFACT_NAME=!ENV_ARTIFACT_NAME!>> !GITHUB_OUTPUT!
        echo ENV_ARTIFACT_PATH=!ENV_ARTIFACT_PATH!>> !GITHUB_OUTPUT!
    )
)
