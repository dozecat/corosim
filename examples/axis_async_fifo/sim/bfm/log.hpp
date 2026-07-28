/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        log.hpp
 * @brief       Common logging utility
 ******************************************************************************/

#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>
#include <sstream>

/**
 * @class Log
 * @brief A simple logging utility class with color support and memory visualization.
 */
class Log {
public:
    bool quiet = false;

    /**
     * @brief Logs an informational message.
     *
     * @param args The message components to log.
     */
    template<typename... Args>
    void info(const Args&... args) {
        if (quiet) return;
        std::stringstream ss;
        format_msg(ss, args...);
        std::string msg = ss.str();
        if (msg.find("success") != std::string::npos || msg.find("successful") != std::string::npos) {
            std::cout << "\033[1;32m" << "[INFO] " << msg << "\033[0m" << std::endl;
        } else {
            std::cout << "[INFO] " << msg << std::endl;
        }
    }

    /**
     * @brief Logs a warning message.
     *
     * @param args The message components to log.
     */
    template<typename... Args>
    void warning(const Args&... args) {
        if (quiet) return;
        std::stringstream ss;
        format_msg(ss, args...);
        std::cout << "\033[1;33m" << "[WARN] " << ss.str() << "\033[0m" << std::endl;
    }

    /**
     * @brief Logs an error message.
     *
     * @param args The message components to log.
     */
    template<typename... Args>
    void error(const Args&... args) {
        std::stringstream ss;
        format_msg(ss, args...);
        std::cerr << "\033[1;31m" << "[ERR ] " << ss.str() << "\033[0m" << std::endl;
    }

    /**
     * @brief Logs a debug message.
     *
     * @param args The message components to log.
     */
    template<typename... Args>
    void debug(const Args&... args) {
        if (quiet) return;
        std::stringstream ss;
        format_msg(ss, args...);
        std::cout << "\033[1;36m" << "[DEBG] " << ss.str() << "\033[0m" << std::endl;
    }

    /**
     * @brief Display memory content in a formatted hex view.
     *
     * Example output:
     * base_addr + 0x00000 - 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
     *
     * @param data           The data to display.
     * @param base_addr      Starting address to display (visual only).
     * @param addr_width     Width of the address in bits (default: 32).
     * @param bytes_per_line Number of bytes per line (default: 16).
     */
    void hexdump(const std::vector<uint8_t>& data, const uint64_t base_addr, const size_t addr_width = 32, const size_t bytes_per_line = 16) {
        if (quiet) return;
        size_t size = data.size();

        // Determine the actual number of hex digits needed for the address.
        // If addr_width is provided, use it to calculate width (bits / 4).
        // If addr_width is 0 or less, fallback to a sensible default or dynamic width.
        // Here we ensure at least 8 hex digits (32-bit address) are shown for consistency if addr_width is 32.
        // If addresses exceed 32-bit range, one might want to use 64-bit (16 hex digits).
        // Let's use std::max to ensure we don't print fewer digits than necessary for larger addresses in the loop,
        // but to enforce consistency based on user-provided width, we'll stick to addr_width/4.
        // Note: addr_width is in bits. Hex digits = bits / 4.
        // Example: 32 bits -> 8 hex digits. 64 bits -> 16 hex digits.

        int hex_width = (addr_width + 3) / 4;

        for (size_t i = 0; i < size; i += bytes_per_line) {
             // Calculate current address
            uint64_t current_addr = base_addr + i;

            // Print address with specified width and hex format
            std::cout << "0x" << std::hex << std::setw(hex_width) << std::setfill('0') << current_addr << " - ";

            // Print bytes_per_line bytes
            for (size_t j = 0; j < bytes_per_line; ++j) {
                if (i + j < size) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0')
                              << static_cast<int>(data[i + j]) << " ";
                }
            }
            std::cout << std::dec << std::endl; // New line and reset to decimal
        }
    }

private:
    // Helper for formatting messages
    void format_msg(std::stringstream&) {}

    template<typename T, typename... Args>
    void format_msg(std::stringstream& ss, const T& first, const Args&... args) {
        ss << first;
        format_msg(ss, args...);
    }
};
#endif // LOG_HPP
