# Multifile Specification

CubeEngine uses a custom file format, multifile, to store most of its assets, excluding models currently due to technical reasons. The engine automatically makes a multifile of the whole assets folder when building, but here is the specification for the multifile file format and how to make one.

To make a multifile, call: 
`multifile <output file name> <list of files>`

In the list of files, folders ending in `/*` can be inputted, which puts all the files in that folder (non-recursively) into the multifile.
The .mf file extension should be used for multifiles, and for CubeEngine, the assets file should be named exactly `assets.mf`.

File Layout & Structure:
----------

The start of a multifile contains the information for the hash, size, and location of the files inside the multifile. The first four bytes of the multifile is the length of all this data, in little endian format. Then, the structure below is repeated for as many files exist in the multifile.

| Offset | Length | Use                                                                     |
|--------|--------|-------------------------------------------------------------------------|
| 0      | 16     | The MD5 hash of the file.                                               |
| 16     | 4      | A pointer in the file to the data of the file, in little endian format. |
| 20     | 4      | The length, in bytes, of the file, in little endian format.             |

After the header, the data of the files is laid out in sequential order to how they were inserted in the header. They should all be null-terminated, to make working with them easier in C/C++.
