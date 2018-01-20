project('fastusbaspv2', 'cpp')

compiler_obj = meson.get_compiler('cpp')

size = find_program('arm-none-eabi-size')
objcopy = find_program('arm-none-eabi-objcopy')
stinfo = find_program('st-info')
stflash = find_program('st-flash')

#define libopencm3 path here
libocm3Path = '/home/ami/repos/libopencm3'

#define cpu types here
cputype = 'cortex-m3'
cpudefine = 'STM32F1'
libocm3cputype = 'opencm3_stm32f1'

#Add your source files here
srcfiles = ['src/main.cpp']


libocm3IncPath = libocm3Path + '/include'
libocm3LibPath = libocm3Path + '/lib'

# check for libopencm3 library
libocm3lib_dep = compiler_obj.find_library(libocm3cputype, required: true, dirs: libocm3LibPath)
if libocm3lib_dep.found() == true
   message('Found libopencm3 library at ' + libocm3Path)
else
   error('unable to find libopencm3 library')
endif

libocm3_incdir = include_directories(libocm3IncPath)


myfoolib_path= '../lib'
stm32f1cargs = ['-Os',
                '-ggdb',
                '-mcpu=' + cputype,
                '-mthumb',
                '-msoft-float',
                '-std=gnu++14',
                '-Wall',
                '-ffunction-sections',
                '-fdata-sections',
                '-D' + cpudefine,
                '-fdata-sections',
                '-fno-exceptions',
                '-I' + myfoolib_path,
#'-I' + libocm3IncPath
                ]

stm32f1linkargs = ['-Os',
                   '-ggdb',
                   '-D' + cpudefine,
                   '-fdata-sections',
                   '-mcpu=' + cputype,
                   '-mthumb',
                   '-msoft-float',
                   '-lc',
                   '-flto',
#'-T../libopencm3_f1.ld',
                   '-T' + meson.current_source_dir() + '/libopencm3.ld',
                   '--specs=nosys.specs',
                   '-nostartfiles',
                   '-Wl,--gc-sections',
                   '-L' + libocm3LibPath,
#               '-l' + libocm3cputype,
                  ]

subdir('lib')

p = ['fastusbaspv2', srcfiles, stm32f1cargs, stm32f1linkargs]

exe = executable(p[0], p[1],
                 cpp_args: p[2],
                 link_args: p[3],
                 include_directories : libocm3_incdir,
                 dependencies: [libocm3lib_dep],
                 link_with: [foolib],
                 build_by_default: true)

run_target('hex', command: [objcopy, ['-Obinary', exe.full_path(),
           exe.full_path() + '.hex']], depends: exe)
run_target('size', command: [size, exe.full_path(), '-B'], depends: exe)

# STlink programmer related commands
run_target('probe', command: [stinfo, '--probe'])
run_target('upload', command: [stflash, 'write', exe.full_path() + '.hex', '0x08000000'], depends: exe)

if meson.is_cross_build()
   message('cross compiling for ' + cputype)
   message('     \n                  ninja - generates elf file.
                  ninja hex - generates hex file.
                  ninja upload - upload hex file to stm32 via stlink programmer.
                  ninja probe - probe stlink programmer.
                  (C) Amitesh Singh ')
endif

