#!/usr/bin/env bash
#
# Utility to setup git-clang-format as a pre-commit hook, with the format style
# defined in format (i.e., Google sytle). The script does the following:

# - Copies git-clang-format into the ${HOME}/bin directory. The directoy is
#   created if it does not exist.
# - Configures git with the clang-format configurations. You can find more
#   details about this in README.md, or in the code below.
# - Creats a symbolic link under .git/hooks to pre-commit, which runs the clang
#   format utility before commiting something.
#
# Note setup.sh has to be run afresh when a repo is cloned or moved.

base_path=$(git rev-parse --show-toplevel)
format_path=${base_path}/tools/format

# Configure git
git config clangFormat.binary clang-format
git config clangFormat.style file
git config --path clangFormat.stylePath ${format_path}

# Put git-clang-format in ${HOME}/bin
mkdir -p ${HOME}/bin
if [ ! -f ${HOME}/bin/git-clang-format ]; then
    cp ${format_path}/git-clang-format ${HOME}/bin
    chmod +x ${HOME}/bin/git-clang-format
fi

# Create link to pre-commit
hooks_path=${base_path}/.git/hooks
if [ -f ${hooks_path}/pre-commit ]; then
    echo "Error: pre-commit already exists. You need to modify your pre-commit"
    echo "       script manually to integrate the clang format pre-commit. One"
    echo "       approach is to create a subdirectory, e.g., hooks/pre, to put"
    echo "       all pre-commit scripts in there, and to have pre-commit simply"
    echo "       execute the scripts in there one by one."
    exit 1
fi
chmod +x ${format_path}/pre-commit
ln -s ${format_path}/pre-commit ${hooks_path}

if [ ! ":${PATH}:" == *":${HOME}/bin:"* ]; then
    echo "You need to add ${HOME}/bin to your PATH:"
    echo '    export PATH=${HOME}/bin:${PATH}'
fi
