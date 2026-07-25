// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vasync_fifo.h for the primary calling header

#ifndef VERILATED_VASYNC_FIFO___024ROOT_H_
#define VERILATED_VASYNC_FIFO___024ROOT_H_  // guard

#include "verilated.h"


class Vasync_fifo__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vasync_fifo___024root final {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(rst,0,0);
    VL_IN8(wr_clk,0,0);
    VL_IN8(rd_clk,0,0);
    VL_IN8(wr_en,0,0);
    VL_IN8(wr_data,7,0);
    VL_OUT8(wr_full,0,0);
    VL_OUT8(wr_overflow,0,0);
    VL_OUT8(wr_level,4,0);
    VL_IN8(rd_en,0,0);
    VL_OUT8(rd_data,7,0);
    VL_OUT8(rd_empty,0,0);
    VL_OUT8(rd_underflow,0,0);
    VL_OUT8(rd_level,4,0);
    CData/*4:0*/ async_fifo__DOT__gray2bin__Vstatic__tmp;
    CData/*4:0*/ async_fifo__DOT__wr_ptr;
    CData/*4:0*/ async_fifo__DOT__wr_gray;
    CData/*4:0*/ async_fifo__DOT__wr_gray_ms;
    CData/*4:0*/ async_fifo__DOT__wr_gray_rd;
    CData/*4:0*/ async_fifo__DOT__rd_ptr;
    CData/*4:0*/ async_fifo__DOT__rd_gray;
    CData/*4:0*/ async_fifo__DOT__rd_gray_ms;
    CData/*4:0*/ async_fifo__DOT__rd_gray_wr;
    CData/*4:0*/ async_fifo__DOT__rd_ptr_bin;
    CData/*4:0*/ async_fifo__DOT__wr_ptr_bin;
    CData/*4:0*/ __Vdly__async_fifo__DOT__wr_ptr;
    CData/*4:0*/ __Vdly__async_fifo__DOT__rd_ptr;
    CData/*7:0*/ __VdlyVal__async_fifo__DOT__mem__v0;
    CData/*3:0*/ __VdlyDim0__async_fifo__DOT__mem__v0;
    CData/*0:0*/ __VdlySet__async_fifo__DOT__mem__v0;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rst__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__wr_clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rd_clk__0;
    IData/*31:0*/ async_fifo__DOT__gray2bin__Vstatic__i;
    IData/*31:0*/ async_fifo__DOT__i;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<CData/*7:0*/, 16> async_fifo__DOT__mem;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlUnpacked<CData/*0:0*/, 4> __Vm_traceActivity;

    // INTERNAL VARIABLES
    Vasync_fifo__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vasync_fifo___024root(Vasync_fifo__Syms* symsp, const char* namep);
    ~Vasync_fifo___024root();
    VL_UNCOPYABLE(Vasync_fifo___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
