// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vasync_fifo.h for the primary calling header

#include "Vasync_fifo__pch.h"

void Vasync_fifo___024root___ctor_var_reset(Vasync_fifo___024root* vlSelf);

Vasync_fifo___024root::Vasync_fifo___024root(Vasync_fifo__Syms* symsp, const char* namep)
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vasync_fifo___024root___ctor_var_reset(this);
}

void Vasync_fifo___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vasync_fifo___024root::~Vasync_fifo___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
