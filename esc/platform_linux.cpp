//#include <../platform_specific_components/linux/libuavcan/include/uavcan_linux/uavcan_linux.hpp>
#include "/home/james/Documents/platform_specific_components/linux/libuavcan/include/uavcan_linux/uavcan_linux.hpp"
#include <string>

uavcan::ISystemClock& getSystemClock()
{
    static uavcan_linux::SystemClock clock;
    return clock;
}

uavcan::ICanDriver& getCanDriver(std::string interface)
{
    static uavcan_linux::SocketCanDriver driver(dynamic_cast<const uavcan_linux::SystemClock&>(getSystemClock()));
    if (driver.getNumIfaces() == 0)     // Will be executed once
    {
        if (driver.addIface(interface) < 0)
        {
            throw std::runtime_error("Failed to add iface");
        }
    }
    return driver;
}
