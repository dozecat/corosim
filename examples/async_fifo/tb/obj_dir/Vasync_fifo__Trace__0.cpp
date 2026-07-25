// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals

#include "verilated_vcd_c.h"
#include "Vasync_fifo__Syms.h"


void Vasync_fifo___024root__trace_chg_0_sub_0(Vasync_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vasync_fifo___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_chg_0\n"); );
    // Body
    Vasync_fifo___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vasync_fifo___024root*>(voidSelf);
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    Vasync_fifo___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vasync_fifo___024root__trace_chg_0_sub_0(Vasync_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_chg_0_sub_0\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[0U]))) {
        bufp->chgIData(oldp+0,(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__i),32);
        bufp->chgIData(oldp+1,(vlSelfRef.async_fifo__DOT__i),32);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[1U]))) {
        bufp->chgCData(oldp+2,(vlSelfRef.async_fifo__DOT__rd_gray_ms),5);
        bufp->chgCData(oldp+3,(vlSelfRef.async_fifo__DOT__rd_gray_wr),5);
        bufp->chgCData(oldp+4,(vlSelfRef.async_fifo__DOT__rd_ptr_bin),5);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[2U]))) {
        bufp->chgCData(oldp+5,(vlSelfRef.async_fifo__DOT__wr_gray_ms),5);
        bufp->chgCData(oldp+6,(vlSelfRef.async_fifo__DOT__wr_gray_rd),5);
        bufp->chgCData(oldp+7,(vlSelfRef.async_fifo__DOT__wr_ptr_bin),5);
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[3U]))) {
        bufp->chgCData(oldp+8,(vlSelfRef.async_fifo__DOT__mem[0]),8);
        bufp->chgCData(oldp+9,(vlSelfRef.async_fifo__DOT__mem[1]),8);
        bufp->chgCData(oldp+10,(vlSelfRef.async_fifo__DOT__mem[2]),8);
        bufp->chgCData(oldp+11,(vlSelfRef.async_fifo__DOT__mem[3]),8);
        bufp->chgCData(oldp+12,(vlSelfRef.async_fifo__DOT__mem[4]),8);
        bufp->chgCData(oldp+13,(vlSelfRef.async_fifo__DOT__mem[5]),8);
        bufp->chgCData(oldp+14,(vlSelfRef.async_fifo__DOT__mem[6]),8);
        bufp->chgCData(oldp+15,(vlSelfRef.async_fifo__DOT__mem[7]),8);
        bufp->chgCData(oldp+16,(vlSelfRef.async_fifo__DOT__mem[8]),8);
        bufp->chgCData(oldp+17,(vlSelfRef.async_fifo__DOT__mem[9]),8);
        bufp->chgCData(oldp+18,(vlSelfRef.async_fifo__DOT__mem[10]),8);
        bufp->chgCData(oldp+19,(vlSelfRef.async_fifo__DOT__mem[11]),8);
        bufp->chgCData(oldp+20,(vlSelfRef.async_fifo__DOT__mem[12]),8);
        bufp->chgCData(oldp+21,(vlSelfRef.async_fifo__DOT__mem[13]),8);
        bufp->chgCData(oldp+22,(vlSelfRef.async_fifo__DOT__mem[14]),8);
        bufp->chgCData(oldp+23,(vlSelfRef.async_fifo__DOT__mem[15]),8);
    }
    bufp->chgBit(oldp+24,(vlSelfRef.rst));
    bufp->chgBit(oldp+25,(vlSelfRef.wr_clk));
    bufp->chgBit(oldp+26,(vlSelfRef.wr_en));
    bufp->chgCData(oldp+27,(vlSelfRef.wr_data),8);
    bufp->chgBit(oldp+28,(vlSelfRef.wr_full));
    bufp->chgBit(oldp+29,(vlSelfRef.wr_overflow));
    bufp->chgCData(oldp+30,(vlSelfRef.wr_level),5);
    bufp->chgBit(oldp+31,(vlSelfRef.rd_clk));
    bufp->chgBit(oldp+32,(vlSelfRef.rd_en));
    bufp->chgCData(oldp+33,(vlSelfRef.rd_data),8);
    bufp->chgBit(oldp+34,(vlSelfRef.rd_empty));
    bufp->chgBit(oldp+35,(vlSelfRef.rd_underflow));
    bufp->chgCData(oldp+36,(vlSelfRef.rd_level),5);
    bufp->chgCData(oldp+37,(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp),5);
    bufp->chgCData(oldp+38,(vlSelfRef.async_fifo__DOT__wr_ptr),5);
    bufp->chgCData(oldp+39,(vlSelfRef.async_fifo__DOT__wr_gray),5);
    bufp->chgCData(oldp+40,(vlSelfRef.async_fifo__DOT__rd_ptr),5);
    bufp->chgCData(oldp+41,(vlSelfRef.async_fifo__DOT__rd_gray),5);
}

void Vasync_fifo___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_cleanup\n"); );
    // Body
    Vasync_fifo___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vasync_fifo___024root*>(voidSelf);
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[2U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[3U] = 0U;
}
