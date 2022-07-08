# Multifile Specification

CubeEngine uses a custom file format, multifile, to store most of its assets, excluding models currently due to technical reasons. The engine automatically makes a multifile of the whole assets folder when building, but here is the specification for the multifile file format and how to make one.

To make a multifile, call: 
`multifile <output file name> <list of files>`

In the list of files, folders ending in `/*` can be inputted, which puts all the files in that folder (non-recursively) into the multifile.
The .mf file extension is preferred for multifiles and CubeEngine by default makes a multifile named `assets.mf` that it uses.

File Layout:
----------

The first few bytes of a multifile is the of the header. If any of the first four bytes are set, than the file was created on a little endian computer and should be read as such. The next four bytes give the length of the file header. Then, the structure below is repeated for as many files exist in the multifile.

| Offset | Length | Use                                            |
|--------|--------|------------------------------------------------|
| 0      | 16     | The MD5 hash of the file.                      |
| 16     | 4      | A pointer in the file to the data of the file. |
| 20     | 4      | The length, in bytes, of the file.             |

After the header, the data of the files is laid out in sequential order to how they were inserted in the header. The multifile creator null-terminates them, so generally you should be able to use them like any other C string in C/C++.
