// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vasync_fifo.h for the primary calling header

#include "Vasync_fifo__pch.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

void Vasync_fifo___024root___eval_triggers__act(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_triggers__act\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((((IData)(vlSelfRef.rd_clk) 
                                                       & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rd_clk__0))) 
                                                      << 2U) 
                                                     | ((((IData)(vlSelfRef.wr_clk) 
                                                          & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__wr_clk__0))) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.rst) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rst__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
    vlSelfRef.__Vtrigprevexpr___TOP__wr_clk__0 = vlSelfRef.wr_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rd_clk__0 = vlSelfRef.rd_clk;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vasync_fifo___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
}

bool Vasync_fifo___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vasync_fifo___024root___nba_sequent__TOP__0(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_sequent__TOP__0\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__0__Vfuncout;
    __Vfunc_async_fifo__DOT__gray2bin__0__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__0__gray;
    __Vfunc_async_fifo__DOT__gray2bin__0__gray = 0;
    // Body
    vlSelfRef.__VdlySet__async_fifo__DOT__mem__v0 = 0U;
    if (((IData)(vlSelfRef.wr_en) & (~ (IData)(vlSelfRef.wr_full)))) {
        vlSelfRef.__VdlyVal__async_fifo__DOT__mem__v0 
            = vlSelfRef.wr_data;
        vlSelfRef.__VdlyDim0__async_fifo__DOT__mem__v0 
            = (0x0000000fU & (IData)(vlSelfRef.async_fifo__DOT__wr_ptr));
        vlSelfRef.__VdlySet__async_fifo__DOT__mem__v0 = 1U;
    }
    vlSelfRef.async_fifo__DOT__rd_gray_wr = vlSelfRef.async_fifo__DOT__rd_gray_ms;
    __Vfunc_async_fifo__DOT__gray2bin__0__gray = vlSelfRef.async_fifo__DOT__rd_gray_wr;
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = __Vfunc_async_fifo__DOT__gray2bin__0__gray;
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp), 1U)));
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp), 2U)));
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp), 4U)));
    __Vfunc_async_fifo__DOT__gray2bin__0__Vfuncout 
        = vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp;
    vlSelfRef.async_fifo__DOT__rd_ptr_bin = __Vfunc_async_fifo__DOT__gray2bin__0__Vfuncout;
    vlSelfRef.async_fifo__DOT__rd_gray_ms = vlSelfRef.async_fifo__DOT__rd_gray;
}

void Vasync_fifo___024root___nba_sequent__TOP__1(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_sequent__TOP__1\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__async_fifo__DOT__wr_ptr = vlSelfRef.async_fifo__DOT__wr_ptr;
    vlSelfRef.wr_overflow = ((~ (IData)(vlSelfRef.rst)) 
                             & ((IData)(vlSelfRef.wr_en) 
                                & (IData)(vlSelfRef.wr_full)));
}

void Vasync_fifo___024root___nba_sequent__TOP__2(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_sequent__TOP__2\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo__DOT__bin2gray__3__Vfuncout;
    __Vfunc_async_fifo__DOT__bin2gray__3__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo__DOT__bin2gray__3__bin;
    __Vfunc_async_fifo__DOT__bin2gray__3__bin = 0;
    // Body
    vlSelfRef.__Vdly__async_fifo__DOT__rd_ptr = vlSelfRef.async_fifo__DOT__rd_ptr;
    vlSelfRef.rd_underflow = ((~ (IData)(vlSelfRef.rst)) 
                              & ((IData)(vlSelfRef.rd_en) 
                                 & (IData)(vlSelfRef.rd_empty)));
    if (vlSelfRef.rst) {
        vlSelfRef.__Vdly__async_fifo__DOT__rd_ptr = 0U;
        vlSelfRef.async_fifo__DOT__rd_gray = 0U;
    } else if (((IData)(vlSelfRef.rd_en) & (~ (IData)(vlSelfRef.rd_empty)))) {
        vlSelfRef.__Vdly__async_fifo__DOT__rd_ptr = 
            (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo__DOT__rd_ptr)));
        __Vfunc_async_fifo__DOT__bin2gray__3__bin = 
            (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo__DOT__rd_ptr)));
        __Vfunc_async_fifo__DOT__bin2gray__3__Vfuncout 
            = (0x0000001fU & (VL_SHIFTR_III(5,5,32, (IData)(__Vfunc_async_fifo__DOT__bin2gray__3__bin), 1U) 
                              ^ (IData)(__Vfunc_async_fifo__DOT__bin2gray__3__bin)));
        vlSelfRef.async_fifo__DOT__rd_gray = __Vfunc_async_fifo__DOT__bin2gray__3__Vfuncout;
    }
}

void Vasync_fifo___024root___nba_sequent__TOP__3(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_sequent__TOP__3\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__1__Vfuncout;
    __Vfunc_async_fifo__DOT__gray2bin__1__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__1__gray;
    __Vfunc_async_fifo__DOT__gray2bin__1__gray = 0;
    // Body
    if (((IData)(vlSelfRef.rd_en) & (~ (IData)(vlSelfRef.rd_empty)))) {
        vlSelfRef.rd_data = vlSelfRef.async_fifo__DOT__mem
            [(0x0000000fU & (IData)(vlSelfRef.async_fifo__DOT__rd_ptr))];
    }
    vlSelfRef.async_fifo__DOT__wr_gray_rd = vlSelfRef.async_fifo__DOT__wr_gray_ms;
    __Vfunc_async_fifo__DOT__gray2bin__1__gray = vlSelfRef.async_fifo__DOT__wr_gray_rd;
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = __Vfunc_async_fifo__DOT__gray2bin__1__gray;
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp), 1U)));
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp), 2U)));
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp 
        = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp) 
                          ^ VL_SHIFTR_III(5,5,32, (IData)(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp), 4U)));
    __Vfunc_async_fifo__DOT__gray2bin__1__Vfuncout 
        = vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp;
    vlSelfRef.async_fifo__DOT__wr_ptr_bin = __Vfunc_async_fifo__DOT__gray2bin__1__Vfuncout;
    vlSelfRef.async_fifo__DOT__wr_gray_ms = vlSelfRef.async_fifo__DOT__wr_gray;
}

void Vasync_fifo___024root___nba_sequent__TOP__4(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_sequent__TOP__4\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.__VdlySet__async_fifo__DOT__mem__v0) {
        vlSelfRef.async_fifo__DOT__mem[vlSelfRef.__VdlyDim0__async_fifo__DOT__mem__v0] 
            = vlSelfRef.__VdlyVal__async_fifo__DOT__mem__v0;
    }
}

void Vasync_fifo___024root___nba_sequent__TOP__5(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_sequent__TOP__5\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.async_fifo__DOT__rd_ptr = vlSelfRef.__Vdly__async_fifo__DOT__rd_ptr;
}

void Vasync_fifo___024root___nba_sequent__TOP__6(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_sequent__TOP__6\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo__DOT__bin2gray__2__Vfuncout;
    __Vfunc_async_fifo__DOT__bin2gray__2__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo__DOT__bin2gray__2__bin;
    __Vfunc_async_fifo__DOT__bin2gray__2__bin = 0;
    // Body
    if (vlSelfRef.rst) {
        vlSelfRef.__Vdly__async_fifo__DOT__wr_ptr = 0U;
        vlSelfRef.async_fifo__DOT__wr_gray = 0U;
    } else if (((IData)(vlSelfRef.wr_en) & (~ (IData)(vlSelfRef.wr_full)))) {
        vlSelfRef.__Vdly__async_fifo__DOT__wr_ptr = 
            (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo__DOT__wr_ptr)));
        __Vfunc_async_fifo__DOT__bin2gray__2__bin = 
            (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.async_fifo__DOT__wr_ptr)));
        __Vfunc_async_fifo__DOT__bin2gray__2__Vfuncout 
            = (0x0000001fU & (VL_SHIFTR_III(5,5,32, (IData)(__Vfunc_async_fifo__DOT__bin2gray__2__bin), 1U) 
                              ^ (IData)(__Vfunc_async_fifo__DOT__bin2gray__2__bin)));
        vlSelfRef.async_fifo__DOT__wr_gray = __Vfunc_async_fifo__DOT__bin2gray__2__Vfuncout;
    }
    vlSelfRef.async_fifo__DOT__wr_ptr = vlSelfRef.__Vdly__async_fifo__DOT__wr_ptr;
}

void Vasync_fifo___024root___nba_comb__TOP__0(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_comb__TOP__0\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.rd_empty = ((IData)(vlSelfRef.async_fifo__DOT__rd_ptr) 
                          == (IData)(vlSelfRef.async_fifo__DOT__wr_ptr_bin));
    vlSelfRef.rd_level = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__wr_ptr_bin) 
                                         - (IData)(vlSelfRef.async_fifo__DOT__rd_ptr)));
}

void Vasync_fifo___024root___nba_comb__TOP__1(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___nba_comb__TOP__1\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.wr_full = (((1U & ((IData)(vlSelfRef.async_fifo__DOT__wr_ptr) 
                                 >> 4U)) != (1U & ((IData)(vlSelfRef.async_fifo__DOT__rd_ptr_bin) 
                                                   >> 4U))) 
                         & ((0x0000000fU & (IData)(vlSelfRef.async_fifo__DOT__wr_ptr)) 
                            == (0x0000000fU & (IData)(vlSelfRef.async_fifo__DOT__rd_ptr_bin))));
    vlSelfRef.wr_level = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__wr_ptr) 
                                         - (IData)(vlSelfRef.async_fifo__DOT__rd_ptr_bin)));
}

void Vasync_fifo___024root___eval_nba(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_nba\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_sequent__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[1U] = 1U;
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_sequent__TOP__1(vlSelf);
    }
    if ((5ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_sequent__TOP__2(vlSelf);
    }
    if ((4ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_sequent__TOP__3(vlSelf);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
    }
    if ((2ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_sequent__TOP__4(vlSelf);
        vlSelfRef.__Vm_traceActivity[3U] = 1U;
    }
    if ((5ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_sequent__TOP__5(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_sequent__TOP__6(vlSelf);
    }
    if ((5ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_comb__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vasync_fifo___024root___nba_comb__TOP__1(vlSelf);
    }
}

void Vasync_fifo___024root___trigger_orInto__act(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___trigger_orInto__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vasync_fifo___024root___eval_phase__act(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_phase__act\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vasync_fifo___024root___eval_triggers__act(vlSelf);
    Vasync_fifo___024root___trigger_orInto__act(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    return (0U);
}

void Vasync_fifo___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vasync_fifo___024root___eval_phase__nba(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_phase__nba\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vasync_fifo___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vasync_fifo___024root___eval_nba(vlSelf);
        Vasync_fifo___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vasync_fifo___024root___eval(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vasync_fifo___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("/Users/my_mac/workspace/github/corosim/examples/async_fifo/rtl/async_fifo.v", 22, "", "NBA region did not converge after 100 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00000064U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vasync_fifo___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                VL_FATAL_MT("/Users/my_mac/workspace/github/corosim/examples/async_fifo/rtl/async_fifo.v", 22, "", "Active region did not converge after 100 tries");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
        } while (Vasync_fifo___024root___eval_phase__act(vlSelf));
    } while (Vasync_fifo___024root___eval_phase__nba(vlSelf));
}

#ifdef VL_DEBUG
void Vasync_fifo___024root___eval_debug_assertions(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_debug_assertions\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.rst & 0xfeU)))) {
        Verilated::overWidthError("rst");
    }
    if (VL_UNLIKELY(((vlSelfRef.wr_clk & 0xfeU)))) {
        Verilated::overWidthError("wr_clk");
    }
    if (VL_UNLIKELY(((vlSelfRef.wr_en & 0xfeU)))) {
        Verilated::overWidthError("wr_en");
    }
    if (VL_UNLIKELY(((vlSelfRef.rd_clk & 0xfeU)))) {
        Verilated::overWidthError("rd_clk");
    }
    if (VL_UNLIKELY(((vlSelfRef.rd_en & 0xfeU)))) {
        Verilated::overWidthError("rd_en");
    }
}
#endif  // VL_DEBUG
