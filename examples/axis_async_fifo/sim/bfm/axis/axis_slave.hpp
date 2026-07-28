/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        axis_slave.hpp
 * @brief       AXI4-Stream Slave VIP
 * @see         https://github.com/dozecat/vaxivip
 *
 * @details     This module implements the Slave VIP for AXI4-Stream protocol verification.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2025/12/30  Initial release (Separated from axis.hpp)
 ******************************************************************************/

#ifndef AXIS_SLAVE_HPP
#define AXIS_SLAVE_HPP

#include "axis_ptr.hpp"
#include "log.hpp"
#include <cstring>
#include <queue>

/// @brief AXI4-Stream Slave BFM
template <
    uint32_t DATA_WIDTH = 64,
    uint32_t ID_WIDTH = 8,
    uint32_t DEST_WIDTH = 1,
    uint32_t USER_WIDTH = 1
>
class axis_slave {
public:
    Log log;
    axis_slave_ptr<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH> port;        ///< Interface signal pointers

    /// @brief Constructor
    axis_slave(axis_slave_ptr<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH> port):port(port) {
        tready_o = true; // always tready
        *(port.tready) = tready_o;
        tvalid_i = false;
        tkeep_i = 0;
        tlast_i = false;
        tuser_i = 0;
    }

    /// @brief Destructor
    ~axis_slave() = default;

    /// @brief Check if the receive queue is empty
    bool empty() {
        return rx_queue.empty();
    }

    /// @brief Receive data from the internal queue
    /// @return Number of bytes actually read, or -1 if empty
    ssize_t recv(std::vector<uint8_t>& dst_buf) {
        if (rx_queue.empty()) {
            return -1;
        } else {
            dst_buf = rx_queue.front();
            rx_queue.pop();
            return dst_buf.size();
        }
    }

    void set_tready(bool tready) {
        tready_o = tready;
    }

    /// @brief Cycle tick
    void update_input() {
        tready_i = *(port.tready);
        tvalid_i = *(port.tvalid);
        tkeep_i = *(port.tkeep);
        tlast_i = *(port.tlast);
        tuser_i = *(port.tuser);

        tdata_i.clear();
        for (int i=0; i<DATA_WIDTH/8; i++) {
             tdata_i.push_back(((char*)port.tdata)[i]);
        }
    }

    void update_output() {
        if (tvalid_i && tready_i) {
            for (int i=0;i<DATA_WIDTH/8;i++) {
                if ((tkeep_i & ((uint64_t)1 << i)) != 0) {
                    recv_buf.push_back(tdata_i[i]);
                }
            }
            if (tlast_i) {
                rx_queue.push(recv_buf);
                log.info("[AXIS-SLV] RECV success !");
                log.info("SIZE:", std::dec, recv_buf.size());
                log.hexdump(recv_buf, 0);
                recv_buf.clear();
            }
        }
        *(port.tready) = tready_o;
    }

private:
    std::vector<uint8_t> recv_buf;
    std::queue<std::vector<uint8_t>> rx_queue;
    bool tready_i;
    bool tready_o;
    bool tvalid_i;
    uint64_t tkeep_i;
    bool tlast_i;
    uint32_t tuser_i;
    std::vector<uint8_t> tdata_i;
};

#endif
