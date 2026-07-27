#pragma once

#include <cstdint>

namespace corosim {

class SignalBase {
public:
    virtual ~SignalBase() = default;
    virtual void commit() = 0;
    virtual bool is_dirty() const = 0;
    virtual void clear_dirty() = 0;
    virtual bool had_posedge() const = 0;
    virtual bool had_negedge() const = 0;
    virtual bool has_changed() const = 0;
    virtual void clear_edge_flags() = 0;
};

} // namespace corosim
