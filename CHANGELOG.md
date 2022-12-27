# CHANGELOG

This is the changelog for list-files (`lf`) starting from the first release (v1.0.0).

_**NOTES:**_
 1. Versions containing "alpha", "a", "beta", "b", or "rc" are pre-releases, and
subject to change.


## v1.7.0.b2 - 2022-05-09

### CHANGED
 - Re-write internal code for memory leak-free performance.
 - Piping, or redirecting the output of `lf` will enable machine-readable output (`-1`, `-m`)
 - Use `scandir()` for output sorting.
 - Proper realloc if many paths are supplied
 - Use `catpath()` instead of fixed size arrays
 - Always check the max len of owner, group, and bytes when during listing files
 - Added `-o`, `--octal` flag to print the file permissions in octal


## v1.6.2 - Aug 14, 2020

### FIXED
 - Fixed main version; removed from alpha (opps!)


## v1.6.1 - Aug 14, 2020

### FIXED
 - Fixed malloc issue, which caused crashing when listing on a directory
 - Cleaned include libraries, removed duplicates, and unnecessary includes


## v1.6.0 - May 17, 2020

### ADDED
 - Print the last modified date on files/directories

### CHANGED
 - Fixed checksum files to not include object/binary files
 - Updated help menu; added Description
 - Updated gpack install section in README.md
 - Fixed source install section in README.md

### REMOVED
 - Outdated `pre-compiled` directory


## v1.5.3 - Mar 1, 2020

### CHANGES
 - Fixed alignment issue when `WITHOUT_ID=true`


## v1.5.2 - Dec 12, 2019

### CHANGES
 - Bumped version in clib.json and README.md

### REMOVED
 - Test 22, which relies on a root-owned file


## v1.5.1 - Dec 12, 2019

### CHANGES
 - Fixes a output format error if a file is owned by root (issue found on raspberry pi)


## v1.5.0 - Dec 12, 2019

### ADDED
 - `.tgz` as a zip format
 - Once-only header for include files

### CHANGES
 - Fixed panic when lf-ing a file with a unknown uid/gid (only prints the uid/gid)


## v1.4.1 - Nov 29, 2019

### CHANGES
 - Fixed name in `clib.json`


## v1.4.0 - Nov 29, 2019

### ADDED
 - Added `clib.json` to support list-files api

### CHANGES
 - Separated main code to smaller files


## v1.3.0 - Nov 28, 2019

### ADDED
 - Print a file red if it is a link, and the link is broken

### CHANGES
 - Small improvments to source code


## v1.2.2 - Nov 23, 2019

### CHANGES
 - Fixed issue where symlink prints unknown/random character after filename
 - Fixed malloc buffers


## v1.2.1 - Nov 23, 2019

### CHANGES
 - Improved mallocs
 - Improved buffers
 - Cleaned/remove extra variables


## v1.2.0 - Nov 16, 2019

### ADDED
 - `-C`, `--commit` to print the github commit

### CHANGES
 - Improved README
 - print the lf version and commit before running the tests


## v1.1.0 - Nov 5, 2019

### CHANGES
 - Fixes the PREFIX to not include the bin dir
 - More improvments to source code


## v1.0.2 - Nov 5, 2019

### CHANGES
 - Improvments to source code


## v1.0.1 - Sep 1, 2019

### ADDED
 - More tests

### CHANGES
 - Improves tests
 - Some improvments to source code


## v1.0.0 - Jul 14, 2019

Init release.

