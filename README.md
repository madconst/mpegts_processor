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

Typical output
--------------
'''
$ ./mpegts_analyzer -i /mnt/hdd32/home/cos/programming/c++/mpegts_processor/media_content/football.ts
Got sync
.........................................
Statistics:
Packets total:  54466
Bytes total:    10240000
Bytes lost:     24
Sync errors:    0
Time elapsed:   2.04417 sec

TS packets stats by PID:
PID:     0,    42 packets processed
PID:    20,    11 packets processed
PID:    21, 33713 packets processed,    42 packets lost
PID:    24,  1109 packets processed,    29 packets lost
PID:    30,    11 packets processed
PID:    31, 11234 packets processed,    58 packets lost
PID:    34,   741 packets processed,     4 packets lost
PID:    40,    11 packets processed
PID:    41,  4213 packets processed,    10 packets lost
PID:  1001,    24 packets processed
PID:  1002,    27 packets processed
PID:  1003,    27 packets processed
PID:  1ffb,    44 packets processed
PID:  1fff,  3259 packets processed,  3259 packets lost

PES packets stats by PID:
PID:     0,    41 packets processed,       7503 bytes
PID:    20,    10 packets processed,       1830 bytes
PID:    21,   242 packets processed,    5873885 bytes,    10 packets lost,     259837 bytes lost
PID:    24,    25 packets processed,     191325 bytes
PID:    30,    10 packets processed,       1830 bytes
PID:    31,   119 packets processed,    1950087 bytes,     5 packets lost,      81277 bytes lost
PID:    34,    25 packets processed,     127675 bytes,     1 packets lost,       5107 bytes lost
PID:    40,    10 packets processed,       1830 bytes
PID:    41,   120 packets processed,     719418 bytes,     9 packets lost,      32819 bytes lost
PID:  1001,    23 packets processed,       4209 bytes
PID:  1002,    25 packets processed,       4575 bytes,     1 packets lost,        183 bytes lost
PID:  1003,    26 packets processed,       4758 bytes
PID:  1ffb,    41 packets processed,       7503 bytes,     2 packets lost,        366 bytes lost

Program table:
table id: 0, version: 1
  Program #2, PID: 0x20
    table id: 2, version: 0,  transport stream id: 2
    PID: 0x21, stream type: 0x2, PCR
    PID: 0x24, stream type: 0x81
  Program #3, PID: 0x30
    table id: 2, version: 0,  transport stream id: 3
    PID: 0x31, stream type: 0x2, PCR
    PID: 0x34, stream type: 0x81
  Program #4, PID: 0x40
    table id: 2, version: 0,  transport stream id: 4
    PID: 0x41, stream type: 0x2, PCR
'''
