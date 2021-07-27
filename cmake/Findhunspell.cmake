# - Try to find HUNSPELL
# Once done this will define
#
#  HUNSPELL_FOUND - System has HUNSPELL
#  HUNSPELL_INCLUDE_DIR - The HUNSPELL include directory
#  HUNSPELL_LIBRARY - The libraries needed to use HUNSPELL
#  HUNSPELL_DEFINITIONS - Compiler switches required for using HUNSPELL

find_path(HUNSPELL_INCLUDE_DIR hunspell/hunspell.hxx)
find_library(HUNSPELL_LIBRARY NAMES hunspell)

# Handle the QUIETLY and REQUIRED arguments and set HUNSPELL_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HUNSPELL DEFAULT_MSG
	HUNSPELL_INCLUDE_DIR HUNSPELL_LIBRARY)

mark_as_advanced(HUNSPELL_INCLUDE_DIR HUNSPELL_LIBRARY)
