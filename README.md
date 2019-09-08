# List File(s) And Directory(s)

`lf` is a **simple**, and **fast** file/directory lister with color hightlighting, file permision and file links!

<br>

## Install:

This is how to install `lf` from source:

```
cd ~/github-repos  # optional

git clone https://github.com/WestleyR/list-files.git
cd lf/

# checkout to a stable version (optional)
git checkout v1.0.1

make
make test # optional, it only takes a second
sudo make install
```

## Example:

```bash
$ ./lf 
-rwx r-xr-x  westleyk  westleyk  18.4 KB       lf
-rw- r--r--  westleyk  westleyk  1.5 KB        tests-funcs
-rw- r--r--  westleyk  westleyk  1.5 KB        Makefile
drwx r-xr-x  westleyk  westleyk  4.0 KB        src
drwx r-xr-x  westleyk  westleyk  4.0 KB        tests
drwx r-xr-x  westleyk  westleyk  4.0 KB        pre-compiled
-rw- r--r--  westleyk  westleyk  859 B         run-tests
-rw- r--r--  westleyk  westleyk  745 B         README.md
-rw- r--r--  westleyk  westleyk  1.6 KB        LICENSE
```

<br>
