MPEG TS processor
=================

An example demonstrating:

* processing MPEG-TS packets
* demultiplexing, PES assembling
* PAT and PMT analyzing
* errors reporting

Compiling
---------

    mkdir build
    cd build
    cmake ..
    make
    ./mpegts_analyzer -i media_content/football.ts

(use any .ts file as in example above)

Usage
-----

    Allowed options:
      -h [ --help ]           show help message
      -i [ --input-file ] arg set the input file name

