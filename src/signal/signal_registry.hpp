/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        signal_registry.hpp
 * @brief       Tracks registered signals and commits pending NBA updates
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Owns the signal list and pending queue used by the scheduler
 *              each tick.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <functional>
#include <vector>
#include "signal_base.hpp"

namespace corosim {

class SignalRegistry {
public:
    void register_signal(SignalBase* s);
    void unregister_signal(SignalBase* s);
    void mark_pending(SignalBase* s);
    void mark_changed(SignalBase* s);

    void commit_all();
    bool any_changed() const { return !changed_signals_.empty(); }
    const std::vector<SignalBase*>& changed_signals() const { return changed_signals_; }
    void clear_edge_flags();

    std::vector<SignalBase*>& signals() { return signals_; }
    const std::vector<SignalBase*>& signals() const { return signals_; }

    std::function<void(SignalBase*)> on_signal_unregistered;

private:
    std::vector<SignalBase*> signals_;
    std::vector<SignalBase*> pending_signals_;
    std::vector<SignalBase*> changed_signals_;
};

} // namespace corosim
