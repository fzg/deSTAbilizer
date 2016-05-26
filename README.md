# deSTAbilizer


license WTFPL

Checks, extracts, replaces parts from some networking devices' firmwares.

various licenses for code in ext :)
ext/squashfs-3.4 comes from the payton gpl sourcebase (fork of squashfs with LZMA)


TODO/ONGOING:
  1. recognize

     -> map module types / magic numbers to blob types
  2. compute

     -> figure out how different blob types are stored
  3. replace

     -> implement --dump -o [modules_dir]

     -> implement --create -i [modules_dir] -o [upgrade_img]
