add_library(stm32f1cpp
            pin.h pin.cpp
            rcc.h rcc.cpp
            delay.h delay.cpp
            serial.h serial.cpp
            fastserial.h
            usb/descriptor.h
            )
target_include_directories(stm32f1cpp PUBLIC .)
