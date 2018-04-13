# Parallel Computer Project

Parallel computer project for course [CS 15-618: Parallel Computer Programming
and Programming][1] at CMU, spring 2018.

## Environment
Configuring clang-format:
```{bash}
git config --global clangFormat.binary clang-format-3.7
git config --global clangFormat.style file
git config --global --path clangFormat.stylePath /PATH/TO/format_code
```

Configuring pre-commit hook:
```{bash}
cd /PATH/TO/format_code
chmod +x pre-commit
ln -s /PATH/TO/format_code/pre-commit /PATH/TO/YOUR/GIT/REPOSITORY/.git/hooks/
```

[1]: http://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/15418-s18/www/
