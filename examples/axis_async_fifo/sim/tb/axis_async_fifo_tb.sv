`timescale 1ns / 1ps
`default_nettype none

module axis_async_fifo_tb
#(
   parameter DATA_WIDTH = 64,
   parameter ID_WIDTH   = 8,
   parameter DEST_WIDTH = 1,
   parameter USER_WIDTH = 1,
   parameter DEPTH      = 16,
   parameter RAM_STYLE  = "distributed",
   parameter AUX_DEPTH  = 16,
   parameter AUX_RAM_STYLE = "distributed"
)
(
   input  wire                      s_clk,
   input  wire                      s_rst,
   input  wire [DATA_WIDTH-1:0]     s_axis_tdata,
   input  wire [DATA_WIDTH/8-1:0]   s_axis_tkeep,
   input  wire [DATA_WIDTH/8-1:0]   s_axis_tstrb,
   input  wire                      s_axis_tlast,
   input  wire [ID_WIDTH-1:0]       s_axis_tid,
   input  wire [DEST_WIDTH-1:0]     s_axis_tdest,
   input  wire [USER_WIDTH-1:0]     s_axis_tuser,
   input  wire                      s_axis_tvalid,
   output wire                      s_axis_tready,

   input  wire                      m_clk,
   input  wire                      m_rst,
   output wire [DATA_WIDTH-1:0]     m_axis_tdata,
   output wire [DATA_WIDTH/8-1:0]   m_axis_tkeep,
   output wire [DATA_WIDTH/8-1:0]   m_axis_tstrb,
   output wire                      m_axis_tlast,
   output wire [ID_WIDTH-1:0]       m_axis_tid,
   output wire [DEST_WIDTH-1:0]     m_axis_tdest,
   output wire [USER_WIDTH-1:0]     m_axis_tuser,
   output wire                      m_axis_tvalid,
   input  wire                      m_axis_tready,

   output wire [$clog2(DEPTH+1)-1:0] s_level,
   output wire                      s_overflow,
   output wire [$clog2(((AUX_DEPTH > 0) ? AUX_DEPTH : DEPTH)+1)-1:0] s_aux_level,
   output wire                      s_aux_overflow,
   output wire                      s_bad_frame,
   output wire                      s_good_frame,
   output wire [$clog2(DEPTH+1)-1:0] m_level,
   output wire                      m_underflow,
   output wire [$clog2(((AUX_DEPTH > 0) ? AUX_DEPTH : DEPTH)+1)-1:0] m_aux_level,
   output wire                      m_aux_underflow,
   output wire                      m_bad_frame,
   output wire                      m_good_frame,

   output wire [31:0]               tb_depth,
   output wire [31:0]               tb_data_width
);

localparam KEEP_WIDTH = DATA_WIDTH / 8;
localparam STRB_WIDTH = DATA_WIDTH / 8;

if_axis #(
   .DATA_WIDTH (DATA_WIDTH),
   .ID_WIDTH   (ID_WIDTH),
   .DEST_WIDTH (DEST_WIDTH),
   .USER_WIDTH (USER_WIDTH)
) s_axis_if ();

if_axis #(
   .DATA_WIDTH (DATA_WIDTH),
   .ID_WIDTH   (ID_WIDTH),
   .DEST_WIDTH (DEST_WIDTH),
   .USER_WIDTH (USER_WIDTH)
) m_axis_if ();

assign s_axis_if.tdata   = s_axis_tdata;
assign s_axis_if.tkeep   = s_axis_tkeep;
assign s_axis_if.tstrb   = s_axis_tstrb;
assign s_axis_if.tlast   = s_axis_tlast;
assign s_axis_if.tid     = s_axis_tid;
assign s_axis_if.tdest   = s_axis_tdest;
assign s_axis_if.tuser   = s_axis_tuser;
assign s_axis_if.tvalid  = s_axis_tvalid;
assign s_axis_tready     = s_axis_if.tready;

assign m_axis_tdata   = m_axis_if.tdata;
assign m_axis_tkeep   = m_axis_if.tkeep;
assign m_axis_tstrb   = m_axis_if.tstrb;
assign m_axis_tlast   = m_axis_if.tlast;
assign m_axis_tid     = m_axis_if.tid;
assign m_axis_tdest   = m_axis_if.tdest;
assign m_axis_tuser   = m_axis_if.tuser;
assign m_axis_tvalid  = m_axis_if.tvalid;
assign m_axis_if.tready = m_axis_tready;

assign tb_depth      = DEPTH;
assign tb_data_width = DATA_WIDTH;

axis_async_fifo #(
   .DEPTH              ( DEPTH ),
   .FRAME_FIFO         ( 1 ),
   .ALWAYS_RECEIVE     ( 0 ),
   .RAM_STYLE          ( RAM_STYLE ),
   .AUX_DEPTH          ( AUX_DEPTH ),
   .AUX_RAM_STYLE      ( AUX_RAM_STYLE ),
   .AUX_ENABLE         ( '{keep:1, strb:1, id:1, dest:1, user:1} ),
   .AUX_FIFO_USE       ( '{keep:1, strb:0, id:0, dest:0, user:0} )
) dut (
   .s_clk              ( s_clk ),
   .s_rst              ( s_rst ),
   .s_axis             ( s_axis_if ),
   .s_level            ( s_level ),
   .s_overflow         ( s_overflow ),
   .s_aux_level        ( s_aux_level ),
   .s_aux_overflow     ( s_aux_overflow ),
   .s_bad_frame        ( s_bad_frame ),
   .s_good_frame       ( s_good_frame ),

   .m_clk              ( m_clk ),
   .m_rst              ( m_rst ),
   .m_axis             ( m_axis_if ),
   .m_level            ( m_level ),
   .m_underflow        ( m_underflow ),
   .m_aux_level        ( m_aux_level ),
   .m_aux_underflow    ( m_aux_underflow ),
   .m_bad_frame        ( m_bad_frame ),
   .m_good_frame       ( m_good_frame )
);

endmodule

`resetall