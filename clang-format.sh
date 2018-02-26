#!/bin/bash

# This script reformats source files using the clang-format utility.
# Set the list of source directories on the "for" line below.
#
# The file .clang-format in this directory specifies the formatting parameters.
#
# Files are changed in-place, so make sure you don't have anything open in an
# editor, and you may want to commit before formatting in case of awryness.
#
# Note that clang-format is not included with OS X or Xcode; you must
# install it yourself.  There are multiple ways to do this:
#
# - If you use Xcode, install the ClangFormat-Xcode plugin. See instructions at
#   <https://github.com/travisjeffery/ClangFormat-Xcode/>.
#   After installation, the executable can be found at
#   $HOME/Library/Application Support/Alcatraz/Plug-ins/ClangFormat/bin/clang-format.
#
# - Download an LLVM release from <http://llvm.org/releases/download.html>.
#   For OS X, use the pre-built binaries for "Darwin".
#
# - Build the LLVM tools from source. See the documentation at <http://llvm.org>.

# Change this if your clang-format executable is somewhere else
if [ `uname -m` == 'x86_64' ];
then
	CLANG_FORMAT="$PROGRAMFILES (x86)\LLVM\bin\clang-format"
else
	CLANG_FORMAT="$PROGRAMFILES\LLVM\bin\clang-format"
fi

for DIRECTORY in goldsrc
do
    echo "Formatting code under $DIRECTORY/"
    find "$DIRECTORY" \( -name '*.h' -or -name '*.hpp' -or -name '*.c' -or -name '*.cpp' \) -print0 | xargs -0 "$CLANG_FORMAT" -i -style=file
done