# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "H:/esp/esp-idf/components/bootloader/subproject"
  "H:/esp32_ads_1115/esp32_ads1115/build/bootloader"
  "H:/esp32_ads_1115/esp32_ads1115/build/bootloader-prefix"
  "H:/esp32_ads_1115/esp32_ads1115/build/bootloader-prefix/tmp"
  "H:/esp32_ads_1115/esp32_ads1115/build/bootloader-prefix/src/bootloader-stamp"
  "H:/esp32_ads_1115/esp32_ads1115/build/bootloader-prefix/src"
  "H:/esp32_ads_1115/esp32_ads1115/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "H:/esp32_ads_1115/esp32_ads1115/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "H:/esp32_ads_1115/esp32_ads1115/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
