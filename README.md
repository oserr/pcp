# Parallel Computer Project

Parallel computer project for course [CS 15-618: Parallel Computer Programming
and Programming][1] at CMU, spring 2018.

## Environment

### cmake
If you don't already have one, then install a recent (i.e., >3) version of
cmake. You can get a tarball and instructions from [here][2]. Alternatively, you
can run ``tools/cmake/install.sh``, which will by default install cmake in
``${Home}/local``, but you can pass in a different prefix, e.g., ``./install.sh
${HOME}/opt``. After installing cmake, make sure the ``bin`` directory of
installation location is in your path, e.g.,
``export PATH=${HOME}/local/bin:${PATH}``.

### clang-format
Configuring clang-format:
```{bash}
git config --global clangFormat.binary clang-format
git config --global clangFormat.style file
base_path=$(git rev-parse --show-toplevel)
format_path=${base_path}/tools/format
git config --global --path clangFormat.stylePath ${format_path}
```
Note on the first we specify clang-format, however, this is simply the name of
the clang-format binary in your path. Therefore, if your clang-format binary is
clang-format-3.7, for example, then use that.

Configuring pre-commit hook:
```{bash}
base_path=$(git rev-parse --show-toplevel)
format_path=${base_path}/tools/format
chmod +x ${format_path}/pre-commit
ln -s ${format_path}/pre-commit ${base_path}/.git/hooks/
```

Alternatively, to avoid doing all of this manually, you can simply run
``tools/format/setup.sh``. It will install ``git-clang-format`` in
``${HOME}/bin``, and setup the configurations.

[1]: http://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/15418-s18/www/
[2]: https://cmake.org/download/
