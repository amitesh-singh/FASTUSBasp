add_executable_stm32f1(fastusbasp config.h tpi.h spi.h spi.c tpi.c usbdevice.h main.c)
target_include_directories(fastusbasp PUBLIC 
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/usart1
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/sleep)
target_link_libraries(fastusbasp 
                      -L${CMAKE_CURRENT_SOURCE_DIR}/../../lib/usart1 usart1
                      -L${CMAKE_CURRENT_SOURCE_DIR}/../../lib/sleep sleep)