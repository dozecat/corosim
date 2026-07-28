/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        axis_ptr.hpp
 * @brief       AXI4-Stream Interface Definitions
 * @see         https://github.com/dozecat/vaxivip
 *
 * @details     This module implements the interface definitions for AXI4-Stream
 *              protocol verification.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2025/12/25  Initial release
 ******************************************************************************/

#ifndef AXIS_PTR_HPP
#define AXIS_PTR_HPP
#include "sig.hpp"
#include <cstdint>
#include <cstring>

/// @brief Check if all signal pointers are assigned and unique
template <typename T>
bool axis_check(const T& p) {
    void* ptrs[] = {
        (void*)p.tdata, (void*)p.tkeep, (void*)p.tstrb,
        (void*)p.tid,   (void*)p.tdest, (void*)p.tuser,
        (void*)p.tlast, (void*)p.tvalid,(void*)p.tready
    };

    for (int i = 0; i < 9; ++i) {
        if (ptrs[i] == NULL) return false;
        for (int j = i + 1; j < 9; ++j) {
            if (ptrs[i] == ptrs[j]) return false;
        }
    }
    return true;
}

/// @brief AXI4-Stream interface signals pointer structure
template <
    size_t DATA_WIDTH = 64,
    size_t ID_WIDTH = 8,
    size_t DEST_WIDTH = 1,
    size_t USER_WIDTH = 1
>
struct axis_ptr {
    sig_io(*tdata , DATA_WIDTH-1  , 0) = NULL;
    sig_io(*tkeep , DATA_WIDTH/8-1, 0) = NULL;
    sig_io(*tstrb , DATA_WIDTH/8-1, 0) = NULL;
    sig_io(*tid   , ID_WIDTH-1    , 0) = NULL;
    sig_io(*tdest , DEST_WIDTH-1  , 0) = NULL;
    sig_io(*tuser , USER_WIDTH-1  , 0) = NULL;
    sig_io(*tlast , 0             , 0) = NULL;
    sig_io(*tvalid, 0             , 0) = NULL;
    sig_io(*tready, 0             , 0) = NULL;

    /// @brief Check if all signal pointers are assigned
    bool check() {
        return axis_check(*this);
    }
};

template <
    size_t DATA_WIDTH = 64,
    size_t ID_WIDTH = 8,
    size_t DEST_WIDTH = 1,
    size_t USER_WIDTH = 1
>
struct axis_master_ptr {
    sig_out(*tdata , DATA_WIDTH-1  , 0) = NULL;
    sig_out(*tkeep , DATA_WIDTH/8-1, 0) = NULL;
    sig_out(*tstrb , DATA_WIDTH/8-1, 0) = NULL;
    sig_out(*tid   , ID_WIDTH-1    , 0) = NULL;
    sig_out(*tdest , DEST_WIDTH-1  , 0) = NULL;
    sig_out(*tuser , USER_WIDTH-1  , 0) = NULL;
    sig_out(*tlast , 0             , 0) = NULL;
    sig_out(*tvalid, 0             , 0) = NULL;
    sig_in (*tready, 0             , 0) = NULL;

    axis_master_ptr() = default;
    axis_master_ptr(const axis_ptr<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH>& p) {
        tdata  = p.tdata;
        tkeep  = p.tkeep;
        tstrb  = p.tstrb;
        tid    = p.tid;
        tdest  = p.tdest;
        tuser  = p.tuser;
        tlast  = p.tlast;
        tvalid = p.tvalid;
        tready = p.tready;
    }

    /// @brief Check if all signal pointers are assigned
    bool check() {
        return axis_check(*this);
    }
};

template <
    size_t DATA_WIDTH = 64,
    size_t ID_WIDTH = 8,
    size_t DEST_WIDTH = 1,
    size_t USER_WIDTH = 1
>
struct axis_slave_ptr {
    sig_in (*tdata , DATA_WIDTH-1  , 0) = NULL;
    sig_in (*tkeep , DATA_WIDTH/8-1, 0) = NULL;
    sig_in (*tstrb , DATA_WIDTH/8-1, 0) = NULL;
    sig_in (*tid   , ID_WIDTH-1    , 0) = NULL;
    sig_in (*tdest , DEST_WIDTH-1  , 0) = NULL;
    sig_in (*tuser , USER_WIDTH-1  , 0) = NULL;
    sig_in (*tlast , 0             , 0) = NULL;
    sig_in (*tvalid, 0             , 0) = NULL;
    sig_out(*tready, 0             , 0) = NULL;

    axis_slave_ptr() = default;
    axis_slave_ptr(const axis_ptr<DATA_WIDTH, ID_WIDTH, DEST_WIDTH, USER_WIDTH>& p) {
        tdata  = p.tdata;
        tkeep  = p.tkeep;
        tstrb  = p.tstrb;
        tid    = p.tid;
        tdest  = p.tdest;
        tuser  = p.tuser;
        tlast  = p.tlast;
        tvalid = p.tvalid;
        tready = p.tready;
    }

    /// @brief Check if all signal pointers are assigned
    bool check() {
        return axis_check(*this);
    }
};

#endif
