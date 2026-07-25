// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vasync_fifo.h for the primary calling header

#include "Vasync_fifo__pch.h"

VL_ATTR_COLD void Vasync_fifo___024root___eval_static(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_static\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
    vlSelfRef.__Vtrigprevexpr___TOP__wr_clk__0 = vlSelfRef.wr_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rd_clk__0 = vlSelfRef.rd_clk;
}

VL_ATTR_COLD void Vasync_fifo___024root___eval_initial__TOP(Vasync_fifo___024root* vlSelf);
VL_ATTR_COLD void Vasync_fifo___024root____Vm_traceActivitySetAll(Vasync_fifo___024root* vlSelf);

VL_ATTR_COLD void Vasync_fifo___024root___eval_initial(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_initial\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vasync_fifo___024root___eval_initial__TOP(vlSelf);
    Vasync_fifo___024root____Vm_traceActivitySetAll(vlSelf);
}

VL_ATTR_COLD void Vasync_fifo___024root___eval_initial__TOP(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_initial__TOP\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__i = 8U;
    vlSelfRef.async_fifo__DOT__mem[0U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[1U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[2U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[3U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[4U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[5U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[6U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[7U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[8U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[9U] = 0U;
    vlSelfRef.async_fifo__DOT__mem[0x0aU] = 0U;
    vlSelfRef.async_fifo__DOT__mem[0x0bU] = 0U;
    vlSelfRef.async_fifo__DOT__mem[0x0cU] = 0U;
    vlSelfRef.async_fifo__DOT__mem[0x0dU] = 0U;
    vlSelfRef.async_fifo__DOT__mem[0x0eU] = 0U;
    vlSelfRef.async_fifo__DOT__mem[0x0fU] = 0U;
    vlSelfRef.async_fifo__DOT__i = 0x00000010U;
}

VL_ATTR_COLD void Vasync_fifo___024root___eval_final(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_final\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vasync_fifo___024root___eval_phase__stl(Vasync_fifo___024root* vlSelf);

VL_ATTR_COLD void Vasync_fifo___024root___eval_settle(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_settle\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00000064U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vasync_fifo___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("/Users/my_mac/workspace/github/corosim/examples/async_fifo/rtl/async_fifo.v", 22, "", "Settle region did not converge after 100 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
    } while (Vasync_fifo___024root___eval_phase__stl(vlSelf));
}

VL_ATTR_COLD void Vasync_fifo___024root___eval_triggers__stl(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_triggers__stl\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered
                                      [0U]) | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
    vlSelfRef.__VstlFirstIteration = 0U;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vasync_fifo___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
}

VL_ATTR_COLD bool Vasync_fifo___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vasync_fifo___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vasync_fifo___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vasync_fifo___024root___stl_sequent__TOP__0(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___stl_sequent__TOP__0\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__0__Vfuncout;
    __Vfunc_async_fifo__DOT__gray2bin__0__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__0__gray;
    __Vfunc_async_fifo__DOT__gray2bin__0__gray = 0;
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__1__Vfuncout;
    __Vfunc_async_fifo__DOT__gray2bin__1__Vfuncout = 0;
    CData/*4:0*/ __Vfunc_async_fifo__DOT__gray2bin__1__gray;
    __Vfunc_async_fifo__DOT__gray2bin__1__gray = 0;
    // Body
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
    vlSelfRef.wr_full = (((1U & ((IData)(vlSelfRef.async_fifo__DOT__wr_ptr) 
                                 >> 4U)) != (1U & ((IData)(vlSelfRef.async_fifo__DOT__rd_ptr_bin) 
                                                   >> 4U))) 
                         & ((0x0000000fU & (IData)(vlSelfRef.async_fifo__DOT__wr_ptr)) 
                            == (0x0000000fU & (IData)(vlSelfRef.async_fifo__DOT__rd_ptr_bin))));
    vlSelfRef.wr_level = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__wr_ptr) 
                                         - (IData)(vlSelfRef.async_fifo__DOT__rd_ptr_bin)));
    vlSelfRef.rd_empty = ((IData)(vlSelfRef.async_fifo__DOT__rd_ptr) 
                          == (IData)(vlSelfRef.async_fifo__DOT__wr_ptr_bin));
    vlSelfRef.rd_level = (0x0000001fU & ((IData)(vlSelfRef.async_fifo__DOT__wr_ptr_bin) 
                                         - (IData)(vlSelfRef.async_fifo__DOT__rd_ptr)));
}

VL_ATTR_COLD void Vasync_fifo___024root___eval_stl(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_stl\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vasync_fifo___024root___stl_sequent__TOP__0(vlSelf);
        Vasync_fifo___024root____Vm_traceActivitySetAll(vlSelf);
    }
}

VL_ATTR_COLD bool Vasync_fifo___024root___eval_phase__stl(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___eval_phase__stl\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vasync_fifo___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = Vasync_fifo___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vasync_fifo___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vasync_fifo___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vasync_fifo___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vasync_fifo___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge rst)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(posedge wr_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(posedge rd_clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vasync_fifo___024root____Vm_traceActivitySetAll(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root____Vm_traceActivitySetAll\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vm_traceActivity[0U] = 1U;
    vlSelfRef.__Vm_traceActivity[1U] = 1U;
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
    vlSelfRef.__Vm_traceActivity[3U] = 1U;
}

VL_ATTR_COLD void Vasync_fifo___024root___ctor_var_reset(Vasync_fifo___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root___ctor_var_reset\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->rst = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18209466448985614591ull);
    vlSelf->wr_clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6739791619049129404ull);
    vlSelf->wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7710928637576349896ull);
    vlSelf->wr_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12812822527505751231ull);
    vlSelf->wr_full = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4538020038871708192ull);
    vlSelf->wr_overflow = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5712970403703992364ull);
    vlSelf->wr_level = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 17729978025146213340ull);
    vlSelf->rd_clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3164821756025340486ull);
    vlSelf->rd_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3814484142505630662ull);
    vlSelf->rd_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17824471296722538975ull);
    vlSelf->rd_empty = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13234839125597451765ull);
    vlSelf->rd_underflow = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10839655039479725950ull);
    vlSelf->rd_level = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 17946973666247161538ull);
    vlSelf->async_fifo__DOT__gray2bin__Vstatic__i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 2872976713883751241ull);
    vlSelf->async_fifo__DOT__gray2bin__Vstatic__tmp = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2377097604688308821ull);
    vlSelf->async_fifo__DOT__wr_ptr = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3479706912837817665ull);
    vlSelf->async_fifo__DOT__wr_gray = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12219325033682177764ull);
    vlSelf->async_fifo__DOT__wr_gray_ms = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7511306625124418215ull);
    vlSelf->async_fifo__DOT__wr_gray_rd = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13530313209802965933ull);
    vlSelf->async_fifo__DOT__rd_ptr = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13334962686257697316ull);
    vlSelf->async_fifo__DOT__rd_gray = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3630775870656441522ull);
    vlSelf->async_fifo__DOT__rd_gray_ms = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13584639673678834406ull);
    vlSelf->async_fifo__DOT__rd_gray_wr = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 1683669290941996526ull);
    vlSelf->async_fifo__DOT__rd_ptr_bin = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7918769524331024642ull);
    vlSelf->async_fifo__DOT__wr_ptr_bin = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 9531739937529857947ull);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->async_fifo__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14547223606984779107ull);
    }
    vlSelf->async_fifo__DOT__i = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 3290158743168906356ull);
    vlSelf->__Vdly__async_fifo__DOT__wr_ptr = 0;
    vlSelf->__Vdly__async_fifo__DOT__rd_ptr = 0;
    vlSelf->__VdlyVal__async_fifo__DOT__mem__v0 = 0;
    vlSelf->__VdlyDim0__async_fifo__DOT__mem__v0 = 0;
    vlSelf->__VdlySet__async_fifo__DOT__mem__v0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__rst__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__wr_clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__rd_clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
