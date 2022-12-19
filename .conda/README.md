# gnuradio conda recipe

This recipe is for creating a package that can be installed into a [conda](https://docs.conda.io/en/latest/) environment. See the [Conda GNU Radio Guide](https://wiki.gnuradio.org/index.php/CondaInstall) for more information on using GNU Radio with conda.

Packages for GNU Radio releases and some out-of-tree (OOT) modules are available through the [`conda-forge` channel](https://conda-forge.org/). If you only need a released version of GNU Radio, it is preferable to use that existing package rather than this recipe.

#### Users

- [Building the package](#building-the-package)

#### Developers

- [Updating the recipe and continuous integration](#updating-the-recipe-and-continuous-integration)
- [Automatically uploading packages from CI](#automatically-uploading-packages-from-ci)


## Building the package

(See the [Conda GNU Radio Guide](https://wiki.gnuradio.org/index.php/CondaInstall) if you are unfamiliar with how to use conda.)

1. Make sure that have `conda-build` and `conda-forge-pinning` installed and updated in your base environment:

        conda activate base
        conda install -n base conda-build conda-forge-pinning
        conda upgrade -n base conda-build conda-forge-pinning

    **Windows users only**: you will also need to have Microsoft's Visual C++ build tools installed. Usually you can do this by installing the [Community edition of Visual Studio](https://visualstudio.microsoft.com/free-developer-offers/) and then selecting a MSVC C++ x64/x86 build tools component under the list of "Individual Components". As of this writing, you will specifically need MSVC v141, i.e. the "MSVC v141 - VS2017 C++ x64/x86 build tools (v14.16)" component. If the build fails to find the version of MSVC it is looking for, try installing other (newer) versions.

2. Download the source code for GNU Radio (which includes this recipe). Typically, this is done by using `git` and cloning the gnuradio repository:

        git clone https://github.com/gnuradio/gnuradio
        cd gnuradio

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

    If you encounter errors, consult with the GNU Radio maintainers or the maintainers of the [gnuradio feedstock](https://github.com/conda-forge/gnuradio-feedstock). It is possible that the recipe will need to be updated.

4. Install the package into an existing environment

        conda install --use-local -n <environment_name> gnuradio

    or create a new environment that includes the package:

        conda create -n test_env gnuradio


## Updating the recipe and continuous integration

If it is necessary to update this recipe and/or the generated continuous integration scripts, these are the steps to follow. You can also use these instructions to build and upload your own gnuradio conda packages to [anaconda.org](https://anaconda.org).

1. Make sure that have `conda-smithy` installed in your base conda environment:

        conda activate base
        conda install -n base conda-smithy
        conda upgrade -n base conda-smithy

2. Make any changes to the recipe and `conda-forge.yml` that are necessary. For example, this could be adding a new dependency in `recipe/meta.yaml` or updating one of the build scripts. If you plan on uploading packages to your own [anaconda.org](https://anaconda.org) channel, this could be specifying the channel name and label as the `channel_targets` key in `recipe/conda_build_config.yaml`. Commit the changes to your repository:

        git commit -a

3. "Re-render" the CI scripts by running conda-smithy from the root of your repository:

        conda-smithy rerender --feedstock_config .conda/conda-forge.yml -c auto

    This will create a commit that adds or updates the CI scripts that have been configured with `conda-forge.yml`. If you want to minimize extraneous files, you can remove some of the newly-created files that are not necessary outside of a typical conda-forge feedstock:

        git rm -f .github/workflows/automerge.yml .github/workflows/webservices.yml .circleci/config.yml
        git commit --amend -s

    When the CI is executed (on a pull request or commit), it will run one job per configuration file in `.ci_support` to build packages for various platforms and Python versions.

    **You should repeat this step whenever the recipe is updated or when changes to the conda-forge infrastructure require all CI scripts to be updated.**

    Since the newly created files will be rewritten whenever conda-smithy is run, you should not edit any of the automatically-generated files in e.g. `.ci_support`, `.scripts`, or `.github/workflows/conda-build.yml`.


## Automatically uploading packages from CI

If you want to enable uploads of the packages to [anaconda.org](https://anaconda.org) whenever the CI is run from a commit on the branch specified in `conda-forge.yml`, you need to set an Anaconda Cloud API token to the `BINSTAR_TOKEN` environment variable. To generate a token, follow the instructions [here](https://docs.anaconda.com/anacondaorg/user-guide/tasks/work-with-accounts/#creating-access-tokens). To populate the `BINSTAR_TOKEN` environment variable for CI jobs, add the token as a secret by following, for example, the [Github docs](https://docs.github.com/en/actions/reference/encrypted-secrets).
