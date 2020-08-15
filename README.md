# List Files in a Directory

![](https://github.com/WestleyR/list-files/workflows/list-files%20CI/badge.svg)

`lf` is a **simple**, and **fast** file/directory lister with color
hightlighting, file permision and file links!

## Install

There are several ways to install `lf`, you can:

 - [Use a package manger (gpack)](#install-via-gpack)
 - [Clone the repo and build from source](#install-from-source)
 - [Install via brew](#install-via-brew)

### Install via gpack

First, download and install [gpack](https://github.com/WestleyR/gpack). Then
you can install by:

```
$ gpack install WestleyR/list-files
```

Or update by:

```
$ gpack update && gpack upgrade
```

### Install from source

First, download the source code:

Using git:

```
$ git clone https://github.com/WestleyR/list-files.git
```

Or downloading the tarball:

```
# From master:
$ wget https://github.com/WestleyR/list-files/archive/master.tar.gz

# Or from a release:
$ wget https://github.com/WestleyR/list-files/archive/v1.6.2.tar.gz

# Then untar the file
$ tar -xzf master.tar.gz && mv list-files-master list-files
```

Then checkout, and compile the source code:

```
$ cd list-files/

# Checkout to a stable version *only if you downloaded with git* (optional)
$ git checkout v1.6.2

$ make
$ make test # optional, it only takes a second
$ sudo make install
```

You can also install to a specified prefix; like your home dir:
_(default: `usr/local`)_

```
$ make install PREFIX=${HOME}/.local
```

_**NOTE:** Check for the latest release on the
[release page](https://github.com/WestleyR/list-files/releases)_

### Install via brew

First, you will need to add my tap:

```
$ brew tap WestleyR/core
```

Then, you can install like normal:

```
$ brew install list-files
```

## Example:

```bash
$ ./lf 
-rwx r-xr-x  westleyk  westleyk  22.7 KB  lf
-rw- r--r--  westleyk  westleyk  1.7 KB   tests-funcs
-rw- r--r--  westleyk  westleyk  1.8 KB   Makefile
drwx r-xr-x  westleyk  westleyk  4.0 KB   src
-rw- r--r--  westleyk  westleyk  2.4 KB   checksum.ssum
drwx r-xr-x  westleyk  westleyk  4.0 KB   deps
drwx r-xr-x  westleyk  westleyk  4.0 KB   tests
-rw- r--r--  westleyk  westleyk  1.1 KB   run-tests
-rw- r--r--  westleyk  westleyk  601 B    clib.json
-rw- r--r--  westleyk  westleyk  2.0 KB   README.md
-rw- r--r--  westleyk  westleyk  1.6 KB   LICENSE
```

<br>

## list-files-utils

As of v1.4.1, list-files also offers a `list-files-utils` functions. You can
use them in your own project by copying all but `main-list-files.c` from `src`
to your own `src` or `deps` directory. Or if you are using `clib`, add this to
your `clib.json` or `package.json`:

```json
"dependencies": {
  "WestleyR/list-files": "v1.6.2"
}
```

_**NOTE:** be sure to check the latest release._

## License

This project if licensed under the terms of The Clear BSD License. See the
[LICENSE file](https://github.com/WestleyR/list-files/blob/master/LICENSE) for more details.

<br>

