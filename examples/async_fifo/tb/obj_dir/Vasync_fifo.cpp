// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vasync_fifo__pch.h"
#include "verilated_vcd_c.h"

//============================================================
// Constructors

Vasync_fifo::Vasync_fifo(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vasync_fifo__Syms(contextp(), _vcname__, this)}
    , rst{vlSymsp->TOP.rst}
    , wr_clk{vlSymsp->TOP.wr_clk}
    , rd_clk{vlSymsp->TOP.rd_clk}
    , wr_en{vlSymsp->TOP.wr_en}
    , wr_data{vlSymsp->TOP.wr_data}
    , wr_full{vlSymsp->TOP.wr_full}
    , wr_overflow{vlSymsp->TOP.wr_overflow}
    , wr_level{vlSymsp->TOP.wr_level}
    , rd_en{vlSymsp->TOP.rd_en}
    , rd_data{vlSymsp->TOP.rd_data}
    , rd_empty{vlSymsp->TOP.rd_empty}
    , rd_underflow{vlSymsp->TOP.rd_underflow}
    , rd_level{vlSymsp->TOP.rd_level}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vasync_fifo::Vasync_fifo(const char* _vcname__)
    : Vasync_fifo(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vasync_fifo::~Vasync_fifo() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vasync_fifo___024root___eval_debug_assertions(Vasync_fifo___024root* vlSelf);
#endif  // VL_DEBUG
void Vasync_fifo___024root___eval_static(Vasync_fifo___024root* vlSelf);
void Vasync_fifo___024root___eval_initial(Vasync_fifo___024root* vlSelf);
void Vasync_fifo___024root___eval_settle(Vasync_fifo___024root* vlSelf);
void Vasync_fifo___024root___eval(Vasync_fifo___024root* vlSelf);

void Vasync_fifo::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vasync_fifo::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vasync_fifo___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vasync_fifo___024root___eval_static(&(vlSymsp->TOP));
        Vasync_fifo___024root___eval_initial(&(vlSymsp->TOP));
        Vasync_fifo___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vasync_fifo___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vasync_fifo::eventsPending() { return false; }

uint64_t Vasync_fifo::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vasync_fifo::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vasync_fifo___024root___eval_final(Vasync_fifo___024root* vlSelf);

VL_ATTR_COLD void Vasync_fifo::final() {
    Vasync_fifo___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vasync_fifo::hierName() const { return vlSymsp->name(); }
const char* Vasync_fifo::modelName() const { return "Vasync_fifo"; }
unsigned Vasync_fifo::threads() const { return 1; }
void Vasync_fifo::prepareClone() const { contextp()->prepareClone(); }
void Vasync_fifo::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vasync_fifo::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false, false, false}};
};

//============================================================
// Trace configuration

void Vasync_fifo___024root__trace_decl_types(VerilatedVcd* tracep);

void Vasync_fifo___024root__trace_init_top(Vasync_fifo___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vasync_fifo___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vasync_fifo___024root*>(voidSelf);
    Vasync_fifo__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(vlSymsp->name(), VerilatedTracePrefixType::SCOPE_MODULE);
    Vasync_fifo___024root__trace_decl_types(tracep);
    Vasync_fifo___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vasync_fifo___024root__trace_register(Vasync_fifo___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD void Vasync_fifo::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedVcdC* const stfp = dynamic_cast<VerilatedVcdC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vasync_fifo::trace()' called on non-VerilatedVcdC object;"
            " use --trace-fst with VerilatedFst object, and --trace-vcd with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP));
    Vasync_fifo___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
