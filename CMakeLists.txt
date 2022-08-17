add_library(USBHost_STM32_core INTERFACE)
target_sources(USBHost_STM32_core INTERFACE
        USBHost/TARGET_STM/USBEndpoint_STM.cpp
        USBHost/TARGET_STM/USBHALHost_STM.cpp
        USBHost/USBDeviceConnected.cpp
        USBHost/USBEndpoint.cpp
        USBHost/USBHost.cpp
        )
target_include_directories(USBHost_STM32_core INTERFACE
        USBHost
        USBHost/TARGET_STM
        )

add_library(USBHost_STM32_hid INTERFACE)
target_sources(USBHost_STM32_hid INTERFACE
        USBHostHID/USBHostKeyboard.cpp
        USBHostHID/USBHostMouse.cpp
        )
target_include_directories(USBHost_STM32_hid INTERFACE
        USBHostHID
        )
add_dependencies(USBHost_STM32_hid USBHost_STM32_core)

add_library(USBHost_STM32_hub INTERFACE)
target_sources(USBHost_STM32_hub INTERFACE
        USBHostHub/USBHostHub.cpp
        )
target_include_directories(USBHost_STM32_hub INTERFACE
        USBHostHub
        )
add_dependencies(USBHost_STM32_hub USBHost_STM32_core)

add_library(USBHost_STM32_msd INTERFACE)
target_sources(USBHost_STM32_msd INTERFACE
        USBHostMSD/USBHostMSD.cpp
        )
target_include_directories(USBHost_STM32_msd INTERFACE
        USBHostMSD
        )
add_dependencies(USBHost_STM32_msd USBHost_STM32_core)

add_library(USBHost_STM32_printer INTERFACE)
target_sources(USBHost_STM32_printer INTERFACE
        USBHostPrinter/USBHostPrinter.cpp
        )
target_include_directories(USBHost_STM32_printer INTERFACE
        USBHostPrinter
        )
add_dependencies(USBHost_STM32_printer USBHost_STM32_core)

add_library(USBHost_STM32::Core ALIAS USBHost_STM32_core)
add_library(USBHost_STM32::HID ALIAS USBHost_STM32_hid)
add_library(USBHost_STM32::Hub ALIAS USBHost_STM32_hub)
add_library(USBHost_STM32::MSD ALIAS USBHost_STM32_msd)
add_library(USBHost_STM32::Printer ALIAS USBHost_STM32_printer)
