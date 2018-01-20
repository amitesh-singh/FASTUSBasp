libsrcfiles = [ 'pin.h',
            'pin.cpp',
            'rcc.h',
            'rcc.cpp',
            'delay.h', 'delay.cpp',
            'serial.h', 'serial.cpp',
            'fastserial.h',
            'usb/descriptor.h']

mylib = ['foo', libsrcfiles, stm32f1cargs, stm32f1linkargs]

foolib = static_library(mylib[0],
                        mylib[1],
                        cpp_args: mylib[2],
                        link_args: mylib[3],
                        include_directories : libocm3_incdir,
                        dependencies: [libocm3lib_dep],
                        pic: false)
include_directories('.')
