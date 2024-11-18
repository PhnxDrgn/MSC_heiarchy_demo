# MSC heiarchy demo

## Building
Requires CMake to build.

To build, run `cmake -B build && cmake --build build`
This will use CMake to configure the build by creating the build directory then building it.

## How to Use
To use the demo, you need to create the MSC then you can create the BS on the port assigned by the MSC (MCS will output port number in terminal) then you can create the MN that uses the BS's port.

### Mobile Switching Center
from the root of the repo, run `./build/msc/msc <msc name>`

### Base Station
from the root of the repo, run `./build/bs/bs <bs name> <msc port>`

### Mobile Node
from the root of the repo, run `./build/mn/mn <mn name> <bs port>`

## Notes
If the MSC dies, the BS will detect it and shut itself down.

If the BS dies, the MN will detect it and shut itself down.