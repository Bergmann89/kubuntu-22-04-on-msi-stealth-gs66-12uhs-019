/*
 *  EC-Access-Tool Copyright(C) 2021, Shubham Paul under GPLv3
 *
 *	This program is free software : you can redistribute it and /or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

#pragma once
#include <iomanip>
#include "ArgumentParser.hpp"
#include "ec.hpp"

int main(int argc, char** argv) {
    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    const auto& parsed_commands = arg_parser.getParsedCommandsRef();
    auto driver_type = arg_parser.getDriverType();

    EmbeddedController ec = EmbeddedController(EC_SC, EC_DATA, LITTLE_ENDIAN, 5, 100, driver_type);
    if (!ec.driverLoaded) {
        std::cerr << std::endl
            << "Error: Couldn't load Driver" << std::endl
            << std::endl;
    }
    else if (!ec.driverFileExist) {
        std::cerr << std::endl
            << "Error: Driver File Not Found" << std::endl
            << "Please make sure that the file 'RwDrv.sys' or 'WinRing0x64.sys' is in the same directorty as EC-Access-Tool.exe" << std::endl
            << std::endl;
    }
    else if (arg_parser.dump()) {
        BYTE ignored[255];
        memset(&ignored[0], 0, sizeof(ignored));

        if (arg_parser.filter())
        {
            ignored[0x68] = true; // CPU Temp (in °C)
            ignored[0x80] = true; // GPU Temp (in °C)
            ignored[0xD7] = true; // Battery Threshold (128 + [0..100])
            ignored[0x31] = true; // Battery Charging Status (enum: 1 not charging, 3 charging, 5 discharging, 9 fully charged, 13 fully charged no power)
            ignored[0xD3] = true; // Keyboard Backlight (enum: 0 off, 1 low, 2 mid, 3 high)
            // ignored[0x2C] = true; // Keyboard Backlight Mode (enum: 0 always on, 8 auto turn off)
            ignored[0xC9] = true; // FAN 1 (470000 / value)
            ignored[0xCB] = true; // FAN 2 (470000 / value)
            ignored[0xCD] = true; // FAN 3 (470000 / value)
            ignored[0x98] = true; // Cooler Boost (bit 7)
            ignored[0xD4] = true; // Fan Mode (enum: 0x0D auto, 0x1D silent, 0x4D basic ?, 0x8D advanced)
            ignored[0xD2] = true; // Shift Mode (enum: 0xC2 super-batery, 0xC1 silent/balanced, 0xC4 power)
            ignored[0xEB] = true; // Super Battery Mode (enum: 0x00 off, 0x0F on)
            ignored[0xE8] = true; // Switch Fn-Win-key (bit 4)
            ignored[0xBF] = true; // USB Power Chare (bit 5)
            ignored[0x2E] = true; // Web Cam (bit 1)

            ignored[0x9E] = true; // Unknown Sensor
            ignored[0x4A] = true; // Unknown Sensor
            ignored[0x4C] = true; // Unknown Sensor
            ignored[0xF4] = true; // Unknown Sensor
            ignored[0xFD] = true; // Unknown Sensor

            // Fan 1 Temp Start 0x82 (in °C)
            // Fan 1 Speed Start 0x71 (in %)
            for (int x = 0x6A; x <= 0x77; ++x)
                ignored[x] = true;

            // Fan 2 Temp Start 0x82 (in °C)
            // Fan 2 Speed Start 0x89 (in %)
            for (int x = 0x82; x <= 0x8F; ++x)
                ignored[x] = true;
        }

        std::cout << "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << std::endl;
        for (int y = 0; y <= 0x0F; ++y) {
            std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (INT)(y << 4) << " ";

            for (int x = 0; x <= 0x0F; ++x) {
                BYTE value;
                BYTE addr = x + (y << 4);

                if (ignored[addr]) {
                    std::cout << "   ";
                } else if (ec.readByte(addr, value) == TRUE) {
                    std::cout << " " << std::setfill('0') << std::setw(2) << std::right << std::hex << (INT)value;
                }
                else {
                    std::cout << " XX";
                }
            }
            std::cout << std::endl;
        }
    }
    else {
        BYTE read_value = 0x00;
        BYTE last_read_value = 0x00;

        for (const auto& command : parsed_commands) {
            char operation = std::get<0>(command);
            if (operation == 'w') {
                //std::cout << "write: " << (int)std::get<1>(command) << " " << (int)std::get<2>(command) << std::endl; //Debug prints
                uint8_t write_address = std::get<1>(command);
                uint8_t write_value = std::get<2>(command);
                ec.writeByte(write_address, write_value);
            }
            else if (operation == 'r') {
                //std::cout << "read: " << (int)std::get<1>(command) << std::endl; //Debug prints
                uint8_t read_address = std::get<1>(command);
                if (ec.readByte(read_address, read_value) == TRUE) {// Read value of register 
                    std::cout << " 0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (INT)read_value;
                    last_read_value = read_value;
                }
                else {
                    std::cout << " 0xXX";
                }
            }
        }
        std::cout << std::endl;
        ec.close(); // Close the resources
    }

    return 0;
}

