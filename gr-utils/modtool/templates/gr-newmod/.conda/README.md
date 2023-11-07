# gr-howto conda recipe

This recipe is for creating a package that can be installed into a [conda](https://docs.conda.io/en/latest/) environment. See the [Conda GNU Radio Guide](https://wiki.gnuradio.org/index.php/CondaInstall) for more information on using GNU Radio with conda.

Packages for GNU Radio and some out-of-tree (OOT) modules are available through the [`conda-forge` channel](https://conda-forge.org/). If this OOT module is already available (search "gnuradio" on [anaconda.org](https://anaconda.org)), it is preferable to use that existing package rather than this recipe.

#### Users

- [Building the package](#building-the-package)

#### Developers

- [Modifying the recipe](#modifying-the-recipe)
- [Continuous integration](#continuous-integration)


## Building the package

(See the [Conda GNU Radio Guide](https://wiki.gnuradio.org/index.php/CondaInstall) if you are unfamiliar with how to use conda.)

1. Make sure that have `conda-build` and `conda-forge-pinning` installed and updated in your base environment:

        conda activate base
        conda install -n base conda-build conda-forge-pinning
        conda upgrade -n base conda-build conda-forge-pinning

    **Windows users only**: you will also need to have Microsoft's Visual C++ build tools installed. Usually you can do this by installing the [Community edition of Visual Studio](https://visualstudio.microsoft.com/free-developer-offers/) and then selecting a MSVC C++ x64/x86 build tools component under the list of "Individual Components". As of this writing, you will specifically need MSVC v142, i.e. the "MSVC v142 - VS 2019 C++ x64/x86 build tools" component. If the build fails to find the version of MSVC it is looking for, try installing other (newer) versions.

2. Download the source code for this OOT module (which includes this recipe). Typically, this is done by using `git` and cloning the module's repository:

        git clone <repository_url>
        cd <repository_name>

3. Run `conda-build` on the recipe to create the package:

    (Linux and macOS)

        conda build .conda/recipe/ -m ${CONDA_PREFIX}/conda_build_config.yaml

    (Windows)

        conda build .conda\recipe\ -m %CONDA_PREFIX%\conda_build_config.yaml

    If you plan on using this package within an existing environment which uses a specific version of Python, specify the version of Python using the `--python` flag. You must use a version string that matches one of the strings listed under `python` in the `${CONDA_PREFIX}/conda_build_config.yaml` file, e.g:

    (Linux and macOS)

        conda build .conda/recipe/ -m ${CONDA_PREFIX}/conda_build_config.yaml --python="3.9.* *_cpython"

    (Windows)

        conda build .conda\recipe\ -m %CONDA_PREFIX%\conda_build_config.yaml --python="3.9.* *_cpython"

    If you encounter errors, consult with the OOT module maintainer or the maintainers of the [gnuradio feedstock](https://github.com/conda-forge/gnuradio-feedstock). It is possible that the recipe will need to be updated.

4. Install the package into an existing environment

        conda install --use-local -n <environment_name> gnuradio-howto

    or create a new environment that includes the package:

        conda create -n test_env gnuradio-howto


## Modifying the recipe

This recipe is derived from a template, and so it is best to check it and make any necessary modifications. Likely changes include:

- Populating metadata near the bottom of the `recipe/meta.yaml` file
- Adding "host" (build-time) and "run" (run-time) dependencies specific to your module in `recipe/meta.yaml`
- Adding special configuration flags or steps are necessary to carry out the build to the build scripts (`recipe/build.sh` for Linux/macOS and `recipe/bld.bat` for Windows)

Specifying the versions of GNU Radio that your OOT is compatible with is one of the most important modifications. Following the instructions below, the module will be built against the conda-forge "pinned" version of GNU Radio, which is usually the latest version.

- To override the pinned version of GNU Radio (e.g. for a branch that builds against an older version), specify the `gnuradio_core` key as instructed in `recipe/conda_build_config.yaml`.
- If the module is compatible with multiple major versions of GNU Radio, and you want to build against multiple of them, you can also add extra versions to `recipe/conda_build_config.yaml` to expand the default build matrix.

See the [conda-build documentation](https://docs.conda.io/projects/conda-build/en/latest/index.html) for details on how to write a conda recipe.


## Continuous integration

Only a few steps are needed to use this recipe to build and test this OOT module using CI services. It can also be used to upload packages to [anaconda.org](https://anaconda.org) for others to download and use.

1. Make sure that have `conda-smithy` installed in your base conda environment:

        conda activate base
        conda install -n base conda-smithy
        conda upgrade -n base conda-smithy

2. Make any changes to the recipe and `conda-forge.yml` that are necessary. For example, if you plan on uploading packages to your own [anaconda.org](https://anaconda.org) channel, specify the channel name and label as the `channel_targets` key in `recipe/conda_build_config.yaml`. Commit the changes to your repository:

        git commit -a

3. "Re-render" the CI scripts by running conda-smithy from the root of your repository:

        conda-smithy rerender --feedstock_config .conda/conda-forge.yml -c auto

    This will create a commit that adds or updates the CI scripts that have been configured with `conda-forge.yml`. If you want to minimize extraneous files, you can remove some of the newly-created files that are not necessary outside of a typical conda-forge feedstock:

        git rm -f .github/workflows/automerge.yml .github/workflows/webservices.yml .circleci/config.yml
        git commit --amend -s

    When the CI is executed (on a pull request or commit), it will run one job per configuration file in `.ci_support` to build packages for various platforms, Python versions, and optionally `gnuradio` versions (by adding to `gnuradio_extra_pin` in `recipe/conda_build_config.yaml`).

    **You should repeat this step whenever the recipe is updated or when changes to the conda-forge infrastructure require all CI scripts to be updated.**

    Since the newly created files will be rewritten whenever conda-smithy is run, you should not edit any of the automatically-generated files in e.g. `.ci_support`, `.scripts`, or `.github/workflows/conda-build.yml`.

4. (optional) If you want to enable uploads of the packages to [anaconda.org](https://anaconda.org) whenever the CI is run from a commit on the branch specified in `conda-forge.yml`, you need to set an Anaconda Cloud API token to the `BINSTAR_TOKEN` environment variable. To generate a token, follow the instructions [here](https://docs.anaconda.com/anacondaorg/user-guide/tasks/work-with-accounts/#creating-access-tokens). To populate the `BINSTAR_TOKEN` environment variable for CI jobs, add the token as a secret by following, for example, the [Github docs](https://docs.github.com/en/actions/reference/encrypted-secrets).
