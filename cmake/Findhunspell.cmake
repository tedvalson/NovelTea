# - Try to find Hunspell
# Once done this will define
#
#  HUNSPELL_FOUND - System has Hunspell
#  HUNSPELL_INCLUDE_DIR - The Hunspell include directory
#  HUNSPELL_LIBRARY - The libraries needed to use Hunspell
#  HUNSPELL_DEFINITIONS - Compiler switches required for using Hunspell

find_path(
	HUNSPELL_INCLUDE_DIR
	NAMES
	hunspell.hxx
	PATH_SUFFIXES
	include/hunspell
	include
	hunspell
)
find_library(
	HUNSPELL_LIBRARY
	NAMES
	hunspell-1.7
	hunspell
)

# Handle the QUIETLY and REQUIRED arguments and set HUNSPELL_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HUNSPELL DEFAULT_MSG
	HUNSPELL_INCLUDE_DIR HUNSPELL_LIBRARY)

mark_as_advanced(HUNSPELL_INCLUDE_DIR HUNSPELL_LIBRARY)
