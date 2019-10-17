Contains procedures that interact with the OpenPower nest chipset.

## To Build
```
To build this package, do the following steps:

    1. ./bootstrap.sh
    2. ./configure ${CONFIGURE_FLAGS}
    3. make

To build with phal feature:
    1. ./bootstrap.sh
    2. i)  To build for p9 target host
             ./configure ${CONFIGURE_FLAGS} --enable-phal CHIPS="p9 openfsi"
       ii) To build for p10 target host
             ./configure ${CONFIGURE_FLAGS} --enable-phal CHIPS="p10 openfsi"
    3. make

    Note: At present pHAL is supporting p9 and p10 target processor.

To clean the repository run `./bootstrap.sh clean`.
```
