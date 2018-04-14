# Parallel Computer Project

Parallel computer project for course [CS 15-618: Parallel Computer Programming
and Programming][1] at CMU, spring 2018.

## Environment
Configuring clang-format:
```{bash}
git config --global clangFormat.binary clang-format
git config --global clangFormat.style file
base_path=$(git rev-path --show-toplevel)
format_path=${base_path}/tools/format
git config --global --path clangFormat.stylePath ${format_path}/format
```
Note on the first we specify clang-format, however, this is simply the name of
the clang-format binary in your path. Therefore, if your clang-format binary is
clang-format-3.7, for example, then use that.

Configuring pre-commit hook:
```{bash}
base_path=$(git rev-path --show-toplevel)
format_path=${base_path}/tools/format
chmod +x ${format_path}/pre-commit
ln -s ${format_path}/pre-commit ${base_path}/.git/hooks/
```

[1]: http://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/15418-s18/www/
