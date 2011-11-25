#
# Find the native GPHOTO2 includes and library
#

FIND_PACKAGE(PkgConfig)
pkg_check_modules(GPHOTO2 libgphoto2)
