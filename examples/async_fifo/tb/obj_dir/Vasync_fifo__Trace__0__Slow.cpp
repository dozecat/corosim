// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals

#include "verilated_vcd_c.h"
#include "Vasync_fifo__Syms.h"


VL_ATTR_COLD void Vasync_fifo___024root__trace_init_sub__TOP__0(Vasync_fifo___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_init_sub__TOP__0\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const int c = vlSymsp->__Vm_baseCode;
    tracep->pushPrefix("$rootio", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+25,0,"rst",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+26,0,"wr_clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+27,0,"wr_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+28,0,"wr_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+29,0,"wr_full",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+30,0,"wr_overflow",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+31,0,"wr_level",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+32,0,"rd_clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+33,0,"rd_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+34,0,"rd_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+35,0,"rd_empty",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+36,0,"rd_underflow",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+37,0,"rd_level",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->popPrefix();
    tracep->pushPrefix("async_fifo", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+43,0,"WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+44,0,"DEPTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+45,0,"FWFT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declArray(c+47,0,"RAM_STYLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 87,0);
    tracep->declBit(c+25,0,"rst",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+26,0,"wr_clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+27,0,"wr_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+28,0,"wr_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+29,0,"wr_full",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+30,0,"wr_overflow",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+31,0,"wr_level",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+32,0,"rd_clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+33,0,"rd_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+34,0,"rd_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+35,0,"rd_empty",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+36,0,"rd_underflow",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+37,0,"rd_level",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+50,0,"AWID",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+51,0,"PTRW",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+51,0,"LVLW",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+52,0,"PTR_ZERO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+53,0,"PTR_ONE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+1,0,"gray2bin__Vstatic__i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->declBus(c+38,0,"gray2bin__Vstatic__tmp",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+39,0,"wr_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+40,0,"wr_gray",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+6,0,"wr_gray_ms",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+7,0,"wr_gray_rd",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+41,0,"rd_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+42,0,"rd_gray",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+3,0,"rd_gray_ms",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+4,0,"rd_gray_wr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+5,0,"rd_ptr_bin",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+8,0,"wr_ptr_bin",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+29,0,"wfull_val",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+35,0,"rempty_val",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+31,0,"wdiff",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+37,0,"rdiff",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->pushPrefix("mem", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declBus(c+9+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 7,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+2,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vasync_fifo___024root__trace_init_top(Vasync_fifo___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_init_top\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vasync_fifo___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vasync_fifo___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
VL_ATTR_COLD void Vasync_fifo___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vasync_fifo___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vasync_fifo___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/);

VL_ATTR_COLD void Vasync_fifo___024root__trace_register(Vasync_fifo___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_register\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    tracep->addConstCb(&Vasync_fifo___024root__trace_const_0, 0, vlSelf);
    tracep->addFullCb(&Vasync_fifo___024root__trace_full_0, 0, vlSelf);
    tracep->addChgCb(&Vasync_fifo___024root__trace_chg_0, 0, vlSelf);
    tracep->addCleanupCb(&Vasync_fifo___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vasync_fifo___024root__trace_const_0_sub_0(Vasync_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vasync_fifo___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_const_0\n"); );
    // Body
    Vasync_fifo___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vasync_fifo___024root*>(voidSelf);
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    Vasync_fifo___024root__trace_const_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vasync_fifo___024root__trace_const_0_sub_0(Vasync_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_const_0_sub_0\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlWide<3>/*95:0*/ __Vtemp_1;
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    bufp->fullIData(oldp+43,(8U),32);
    bufp->fullIData(oldp+44,(0x00000010U),32);
    bufp->fullQData(oldp+45,(0x00000066616c7365ULL),40);
    __Vtemp_1[0U] = 0x75746564U;
    __Vtemp_1[1U] = 0x74726962U;
    __Vtemp_1[2U] = 0x00646973U;
    bufp->fullWData(oldp+47,(__Vtemp_1),88);
    bufp->fullIData(oldp+50,(4U),32);
    bufp->fullIData(oldp+51,(5U),32);
    bufp->fullCData(oldp+52,(0U),5);
    bufp->fullCData(oldp+53,(1U),5);
}

VL_ATTR_COLD void Vasync_fifo___024root__trace_full_0_sub_0(Vasync_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vasync_fifo___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_full_0\n"); );
    // Body
    Vasync_fifo___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vasync_fifo___024root*>(voidSelf);
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    Vasync_fifo___024root__trace_full_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vasync_fifo___024root__trace_full_0_sub_0(Vasync_fifo___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vasync_fifo___024root__trace_full_0_sub_0\n"); );
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    bufp->fullIData(oldp+1,(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__i),32);
    bufp->fullIData(oldp+2,(vlSelfRef.async_fifo__DOT__i),32);
    bufp->fullCData(oldp+3,(vlSelfRef.async_fifo__DOT__rd_gray_ms),5);
    bufp->fullCData(oldp+4,(vlSelfRef.async_fifo__DOT__rd_gray_wr),5);
    bufp->fullCData(oldp+5,(vlSelfRef.async_fifo__DOT__rd_ptr_bin),5);
    bufp->fullCData(oldp+6,(vlSelfRef.async_fifo__DOT__wr_gray_ms),5);
    bufp->fullCData(oldp+7,(vlSelfRef.async_fifo__DOT__wr_gray_rd),5);
    bufp->fullCData(oldp+8,(vlSelfRef.async_fifo__DOT__wr_ptr_bin),5);
    bufp->fullCData(oldp+9,(vlSelfRef.async_fifo__DOT__mem[0]),8);
    bufp->fullCData(oldp+10,(vlSelfRef.async_fifo__DOT__mem[1]),8);
    bufp->fullCData(oldp+11,(vlSelfRef.async_fifo__DOT__mem[2]),8);
    bufp->fullCData(oldp+12,(vlSelfRef.async_fifo__DOT__mem[3]),8);
    bufp->fullCData(oldp+13,(vlSelfRef.async_fifo__DOT__mem[4]),8);
    bufp->fullCData(oldp+14,(vlSelfRef.async_fifo__DOT__mem[5]),8);
    bufp->fullCData(oldp+15,(vlSelfRef.async_fifo__DOT__mem[6]),8);
    bufp->fullCData(oldp+16,(vlSelfRef.async_fifo__DOT__mem[7]),8);
    bufp->fullCData(oldp+17,(vlSelfRef.async_fifo__DOT__mem[8]),8);
    bufp->fullCData(oldp+18,(vlSelfRef.async_fifo__DOT__mem[9]),8);
    bufp->fullCData(oldp+19,(vlSelfRef.async_fifo__DOT__mem[10]),8);
    bufp->fullCData(oldp+20,(vlSelfRef.async_fifo__DOT__mem[11]),8);
    bufp->fullCData(oldp+21,(vlSelfRef.async_fifo__DOT__mem[12]),8);
    bufp->fullCData(oldp+22,(vlSelfRef.async_fifo__DOT__mem[13]),8);
    bufp->fullCData(oldp+23,(vlSelfRef.async_fifo__DOT__mem[14]),8);
    bufp->fullCData(oldp+24,(vlSelfRef.async_fifo__DOT__mem[15]),8);
    bufp->fullBit(oldp+25,(vlSelfRef.rst));
    bufp->fullBit(oldp+26,(vlSelfRef.wr_clk));
    bufp->fullBit(oldp+27,(vlSelfRef.wr_en));
    bufp->fullCData(oldp+28,(vlSelfRef.wr_data),8);
    bufp->fullBit(oldp+29,(vlSelfRef.wr_full));
    bufp->fullBit(oldp+30,(vlSelfRef.wr_overflow));
    bufp->fullCData(oldp+31,(vlSelfRef.wr_level),5);
    bufp->fullBit(oldp+32,(vlSelfRef.rd_clk));
    bufp->fullBit(oldp+33,(vlSelfRef.rd_en));
    bufp->fullCData(oldp+34,(vlSelfRef.rd_data),8);
    bufp->fullBit(oldp+35,(vlSelfRef.rd_empty));
    bufp->fullBit(oldp+36,(vlSelfRef.rd_underflow));
    bufp->fullCData(oldp+37,(vlSelfRef.rd_level),5);
    bufp->fullCData(oldp+38,(vlSelfRef.async_fifo__DOT__gray2bin__Vstatic__tmp),5);
    bufp->fullCData(oldp+39,(vlSelfRef.async_fifo__DOT__wr_ptr),5);
    bufp->fullCData(oldp+40,(vlSelfRef.async_fifo__DOT__wr_gray),5);
    bufp->fullCData(oldp+41,(vlSelfRef.async_fifo__DOT__rd_ptr),5);
    bufp->fullCData(oldp+42,(vlSelfRef.async_fifo__DOT__rd_gray),5);
}
