/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        axis_master.hpp
 * @brief       AXI4-Stream VIP (Verification IP)
 * @see         https://github.com/dozecat/vaxivip
 *
 * @details     This module implements the VIP for AXI4-Stream protocol verification.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2025/12/30  Initial release (Separated from axis.hpp)
 ******************************************************************************/

#ifndef AXIS_MASTER_HPP
#define AXIS_MASTER_HPP

#include "axis_ptr.hpp"
#include "log.hpp"
#include <cstring>
#include <queue>

/// @brief AXI4-Stream Master BFM
template <
    uint32_t DATA_WIDTH = 64,
    uint32_t ID_WIDTH = 8,
    uint32_t DEST_WIDTH = 1,
    uint32_t USER_WIDTH = 1
>
class axis_master {
public:
    Log log;
    std::vector<uint8_t> tx_buf;            ///< Current transaction buffer
    size_t tx_buf_idx;                      ///< Current index in tx_buf
    std::queue<std::vector<uint8_t>> tx_queue;///< Queue of pending transactions
    std::queue<uint32_t> tx_id_queue;         ///< Queue of pending transaction IDs
    std::queue<uint32_t> tx_dest_queue;       ///< Queue of pending transaction DESTs
    std::queue<uint32_t> tx_user_queue;       ///< Queue of pending transaction USERs
    std::queue<bool> tx_sof_queue;            ///< Queue of pending SOF flags
    axis_master_ptr<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH> port;       ///< Interface signal pointers
    int byte_width;                         ///< Data width in bytes
    uint32_t tx_id;                         ///< Current transaction ID
    uint32_t tx_dest;                       ///< Current transaction DEST
    uint32_t tx_user;                       ///< Current transaction USER
    bool tx_sof;                            ///< Current transaction SOF flag

    // Registered Input Signals
    bool tready_i;

    /// @brief Constructor
    axis_master(axis_master_ptr<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH> port):port(port) {
        byte_width = DATA_WIDTH/8;
        tx_buf_idx = 0;
        tready_i = false;
        tx_id = 0;
        tx_dest = 0;
        tx_user = 0;
        tx_sof = false;
    }

    /// @brief Destructor
    ~axis_master() = default;

    /// @brief Send data through AXI Stream
    /// @param id Transaction ID
    /// @param dest Transaction Destination
    /// @param user Transaction User Data
    /// @param sof Assert TUSER[0] at Start of Frame
    void send(const std::vector<uint8_t>& data, uint32_t id = 0, uint32_t dest = 0, uint32_t user = 0, bool sof = false) {
        tx_queue.push(data);
        tx_id_queue.push(id);
        tx_dest_queue.push(dest);
        tx_user_queue.push(user);
        tx_sof_queue.push(sof);
    }

    /// @brief Get the current TREADY state
    bool get_tready() {
        return tready_i;
    }

    /// @brief Cycle tick
    void update_input() {
        tready_i = *(port.tready);
    }

    void update_output() {
        if (tready_i) {
            *(port.tvalid) = false;
            // start new transaction
            if (tx_buf.empty() && !tx_queue.empty()) {
                tx_buf = tx_queue.front();
                tx_queue.pop();

                if (!tx_id_queue.empty()) {
                    tx_id = tx_id_queue.front();
                    tx_id_queue.pop();
                }

                if (!tx_dest_queue.empty()) {
                    tx_dest = tx_dest_queue.front();
                    tx_dest_queue.pop();
                }

                if (!tx_user_queue.empty()) {
                    tx_user = tx_user_queue.front();
                    tx_user_queue.pop();
                }

                if (!tx_sof_queue.empty()) {
                    tx_sof = tx_sof_queue.front();
                    tx_sof_queue.pop();
                } else {
                    tx_sof = false;
                }

                tx_buf_idx = 0;
            }
            if (!tx_buf.empty()) {
                // bool is_start = (tx_buf_idx == 0);
                int byte_pos = 0;
                *(port.tkeep) = 0;
                *(port.tid) = tx_id;
                *(port.tdest) = tx_dest;
                // handle SOF on TUSER[0]
                if (tx_buf_idx == 0 && tx_sof) {
                     *(port.tuser) = tx_user | 0x1;
                } else {
                     *(port.tuser) = tx_user;
                }

                while (tx_buf_idx < tx_buf.size() && byte_pos < byte_width) {
                    *(port.tkeep) = *(port.tkeep) | ((uint64_t)1 << byte_pos);
                    ((char*)port.tdata)[byte_pos] = tx_buf[tx_buf_idx];
                    tx_buf_idx++;
                    byte_pos++;
                }

                bool last = (tx_buf_idx >= tx_buf.size());
                *(port.tlast) = last;
                *(port.tvalid) = true;

                if (last) {
                    log.info("[AXIS-MST] SEND success !");
                    log.info("SIZE:", std::dec, tx_buf.size());
                    log.hexdump(tx_buf, 0);
                    tx_buf.clear();
                    tx_buf_idx = 0;
                }
            }
        }
    }
};

#endif
