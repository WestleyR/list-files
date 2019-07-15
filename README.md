# List File(s) And Directory(s)

`lf` is a **simple**, and **fast** file/directory lister with color hightlighting, and prints file permision!

<br>

## Install:

This is how to install `lf` from source:

```
cd /tmp

git clone https://github.com/WestleyR/list-files
cd lf/
make
sudo make install
```

*And yes, lf can be compiled with `tcc` (tiny c compiler) edit in `Makefile`.*

## Example:

```bash
$ ./lf 
-rwx r-xr-x  westleyk  westleyk  17.7 kB       lf
-rw- r--r--  westleyk  westleyk  693 B         Makefile
drwx r-xr-x  westleyk  westleyk  4.0 kB        src
drwx r-xr-x  westleyk  westleyk  4.0 kB        pre-compiled
-rw- r--r--  westleyk  westleyk  762 B         README.md
-rw- r--r--  westleyk  westleyk  1.6 kB        LICENSE
```

<br>
