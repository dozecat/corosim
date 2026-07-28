//*****************************************************************************
// Copyright (C) 2026 dozecat. All rights reserved.
// SPDX-License-Identifier: MIT
//
// File:        axis_async_fifo.sv
// Description: AXI4-Stream asynchronous FIFO using dual async FIFOs
//              (data_fifo + aux_fifo) to reduce storage for side-channel signals.
// Repository:  https://github.com/dozecat/axi_lib.git
//
// Modification History:
// Ver   Who       Date        Changes
// ----  ----  ----------  ----------------------------------------------------
// 1.0         2026/6/5    Initial release
// 1.1         2026/6/8    Split single packed FIFO into data_fifo (per-beat)
//                         + aux_fifo (per-frame).  FRAME_FIFO mode removed;
//                         side-channel placement configurable via
//                         AUX_ENABLE / AUX_FIFO_USE.
// 1.2         2026/6/8    Restored FRAME_FIFO mode. Bad frame flag embedded
//                         in aux_fifo (no separate meta FIFO). Read-side FSM
//                         waits for aux_fifo non-empty (frame complete) then
//                         reads bad flag; skips bad frames, registers per-frame
//                         side-channel signals on first beat.
//*****************************************************************************
`timescale 1ns / 1ps
`default_nettype none

module axis_async_fifo
#(
   parameter FRAME_FIFO            = 0,
   parameter ALWAYS_RECEIVE        = 0,
   parameter DEPTH                 = 4096,
   parameter RAM_STYLE             = "block",
   parameter AUX_DEPTH             = 16,
   parameter AUX_RAM_STYLE         = "distributed",
   parameter axis_aux_pkg::axis_aux_t AUX_ENABLE  = '{keep:1, strb:0, id:1, dest:0, user:1},
   parameter axis_aux_pkg::axis_aux_t AUX_FIFO_USE = '{keep:1, strb:1, id:1, dest:1, user:0}
)(
   input  wire                         s_clk,
   input  wire                         s_rst,
   output wire [$clog2(DEPTH+1)-1:0]   s_level,
   output wire                         s_overflow,
   output wire [AUX_LVLW-1:0]          s_aux_level,
   output wire                         s_aux_overflow,
   output wire                         s_bad_frame,
   output wire                         s_good_frame,
   if_axis.slave                       s_axis,

   input  wire                         m_clk,
   input  wire                         m_rst,
   output wire [$clog2(DEPTH+1)-1:0]   m_level,
   output wire                         m_underflow,
   output wire [AUX_LVLW-1:0]          m_aux_level,
   output wire                         m_aux_underflow,
   output wire                         m_bad_frame,
   output wire                         m_good_frame,
   if_axis.master                      m_axis
);

localparam DATA_WIDTH = s_axis.DATA_WIDTH;
localparam KEEP_WIDTH = s_axis.KEEP_WIDTH;
localparam STRB_WIDTH = s_axis.STRB_WIDTH;
localparam ID_WIDTH   = s_axis.ID_WIDTH;
localparam DEST_WIDTH = s_axis.DEST_WIDTH;
localparam USER_WIDTH = s_axis.USER_WIDTH;

localparam LVLW           = $clog2(DEPTH + 1);
localparam AUX_DEPTH_VAL  = (AUX_DEPTH > 0) ? AUX_DEPTH : DEPTH;
localparam AUX_LVLW       = $clog2(AUX_DEPTH_VAL + 1);

localparam D_TDATA_OFF = 0;
localparam D_TKEEP_OFF = D_TDATA_OFF + DATA_WIDTH;
localparam D_TLAST_OFF = D_TKEEP_OFF + (AUX_ENABLE.keep && !AUX_FIFO_USE.keep ? KEEP_WIDTH : 0);
localparam D_TSTRB_OFF = D_TLAST_OFF + 1;
localparam D_TID_OFF   = D_TSTRB_OFF + (AUX_ENABLE.strb && !AUX_FIFO_USE.strb ? STRB_WIDTH : 0);
localparam D_TDEST_OFF = D_TID_OFF   + (AUX_ENABLE.id   && !AUX_FIFO_USE.id   ? ID_WIDTH   : 0);
localparam D_TUSER_OFF = D_TDEST_OFF + (AUX_ENABLE.dest && !AUX_FIFO_USE.dest ? DEST_WIDTH : 0);
localparam DATA_FIFO_WIDTH = D_TUSER_OFF + (AUX_ENABLE.user && !AUX_FIFO_USE.user ? USER_WIDTH : 0);

localparam A_TKEEP_OFF = 0;
localparam A_TSTRB_OFF = A_TKEEP_OFF + (AUX_ENABLE.keep && AUX_FIFO_USE.keep ? KEEP_WIDTH : 0);
localparam A_TID_OFF   = A_TSTRB_OFF + (AUX_ENABLE.strb && AUX_FIFO_USE.strb ? STRB_WIDTH : 0);
localparam A_TDEST_OFF = A_TID_OFF   + (AUX_ENABLE.id   && AUX_FIFO_USE.id   ? ID_WIDTH   : 0);
localparam A_TUSER_OFF = A_TDEST_OFF + (AUX_ENABLE.dest && AUX_FIFO_USE.dest ? DEST_WIDTH : 0);
localparam A_BAD_OFF   = A_TUSER_OFF + (AUX_ENABLE.user && AUX_FIFO_USE.user ? USER_WIDTH : 0);
localparam AUX_FIFO_RAW_WIDTH = A_BAD_OFF;
localparam AUX_FIFO_WIDTH = FRAME_FIFO ? (AUX_FIFO_RAW_WIDTH > 0 ? AUX_FIFO_RAW_WIDTH + 1 : 1)
                                        : AUX_FIFO_RAW_WIDTH;

initial begin
   $display("axis_async_fifo: DATA=%0d KEEP=%0d/%0d STRB=%0d/%0d ID=%0d/%0d DEST=%0d/%0d USER=%0d/%0d",
            DATA_WIDTH, KEEP_WIDTH, AUX_ENABLE.keep, STRB_WIDTH, AUX_ENABLE.strb,
            ID_WIDTH, AUX_ENABLE.id, DEST_WIDTH, AUX_ENABLE.dest, USER_WIDTH, AUX_ENABLE.user);
   $display("axis_async_fifo: D_FIFO_WIDTH=%0d A_FIFO_WIDTH=%0d DEPTH=%0d FRAME=%0d",
            DATA_FIFO_WIDTH, AUX_FIFO_WIDTH, DEPTH, FRAME_FIFO);
end

initial begin
   if (DEPTH < 2 || (DEPTH & (DEPTH - 1)) != 0) begin
      $error("axis_async_fifo DEPTH must be power-of-2 and >= 2, got %0d", DEPTH);
      $finish;
   end
end

initial begin
   if (AUX_ENABLE.keep && s_axis.KEEP_WIDTH < 1)
      $error("axis_async_fifo: KEEP enable requires KEEP_WIDTH >= 1");
end

initial begin
   if (AUX_ENABLE.strb && s_axis.STRB_WIDTH < 1)
      $error("axis_async_fifo: STRB enable requires STRB_WIDTH >= 1");
end

initial begin
   if (AUX_ENABLE.id && s_axis.ID_WIDTH < 1)
      $error("axis_async_fifo: ID enable requires ID_WIDTH >= 1");
end

initial begin
   if (AUX_ENABLE.dest && s_axis.DEST_WIDTH < 1)
      $error("axis_async_fifo: DEST enable requires DEST_WIDTH >= 1");
end

initial begin
   if (AUX_ENABLE.user && s_axis.USER_WIDTH < 1)
      $error("axis_async_fifo: USER enable requires USER_WIDTH >= 1");
end

generate
   if (AUX_DEPTH > 0 && ((AUX_DEPTH & (AUX_DEPTH - 1)) != 0))
      $error("axis_async_fifo AUX_DEPTH must be power-of-2, got %0d", AUX_DEPTH);
endgenerate

wire [KEEP_WIDTH-1:0]      afifo_tkeep;
wire [STRB_WIDTH-1:0]      afifo_tstrb;
wire [ID_WIDTH-1:0]        afifo_tid;
wire [DEST_WIDTH-1:0]      afifo_tdest;
wire [USER_WIDTH-1:0]      afifo_tuser;
wire                       afifo_bad_frame;
wire                       afifo_rd_empty;
wire                       afifo_rd_en_fsm;
wire [KEEP_WIDTH-1:0]      frame_tkeep;
wire [STRB_WIDTH-1:0]      frame_tstrb;
wire [ID_WIDTH-1:0]        frame_tid;
wire [DEST_WIDTH-1:0]      frame_tdest;
wire [USER_WIDTH-1:0]      frame_tuser;
wire                       frame_is_bad;

wire                       dfifo_wr_en;
wire [DATA_FIFO_WIDTH-1:0] dfifo_wr_data;
wire                       dfifo_wr_full;
wire                       dfifo_wr_overflow;
wire [LVLW-1:0]            dfifo_wr_level;

wire                       dfifo_rd_en;
wire [DATA_FIFO_WIDTH-1:0] dfifo_rd_data;
wire                       dfifo_rd_empty;
wire                       dfifo_rd_underflow;
wire [LVLW-1:0]            dfifo_rd_level;

wire [DATA_WIDTH-1:0]      dfifo_tdata;
wire                       dfifo_tlast;
wire [KEEP_WIDTH-1:0]      dfifo_tkeep;
wire [STRB_WIDTH-1:0]      dfifo_tstrb;
wire [ID_WIDTH-1:0]        dfifo_tid;
wire [DEST_WIDTH-1:0]      dfifo_tdest;
wire [USER_WIDTH-1:0]      dfifo_tuser;

assign s_axis.tready = (FRAME_FIFO || ALWAYS_RECEIVE) ? 1'b1 : !dfifo_wr_full;
assign dfifo_wr_en   = s_axis.tvalid & s_axis.tready;

assign dfifo_wr_data[D_TDATA_OFF +: DATA_WIDTH] = s_axis.tdata;
assign dfifo_wr_data[D_TLAST_OFF] = s_axis.tlast;

generate
   if (AUX_ENABLE.keep && !AUX_FIFO_USE.keep)
      assign dfifo_wr_data[D_TKEEP_OFF +: KEEP_WIDTH] = s_axis.tkeep;
   if (AUX_ENABLE.strb && !AUX_FIFO_USE.strb)
      assign dfifo_wr_data[D_TSTRB_OFF +: STRB_WIDTH] = s_axis.tstrb;
   if (AUX_ENABLE.id && !AUX_FIFO_USE.id)
      assign dfifo_wr_data[D_TID_OFF +: ID_WIDTH] = s_axis.tid;
   if (AUX_ENABLE.dest && !AUX_FIFO_USE.dest)
      assign dfifo_wr_data[D_TDEST_OFF +: DEST_WIDTH] = s_axis.tdest;
   if (AUX_ENABLE.user && !AUX_FIFO_USE.user)
      assign dfifo_wr_data[D_TUSER_OFF +: USER_WIDTH] = s_axis.tuser;
endgenerate

generate
   if (FRAME_FIFO) begin : gen_frame_wr
      localparam FCW = $clog2(DEPTH + 1);
      reg [FCW-1:0]   frame_beat_cnt;
      reg              frame_overflow;

      wire frame_tlast = s_axis.tvalid & s_axis.tready & s_axis.tlast;

      always_ff @(posedge s_clk) begin
         if (s_rst) begin
            frame_beat_cnt  <= 0;
            frame_overflow  <= 0;
         end else if (frame_tlast) begin
            frame_beat_cnt  <= 0;
            frame_overflow  <= 0;
         end else if (dfifo_wr_en) begin
            frame_beat_cnt  <= frame_beat_cnt + 1;
            if (dfifo_wr_full)
               frame_overflow <= 1;
         end
      end

      wire frame_cmp = frame_beat_cnt >= DEPTH[FCW-1:0];
      assign frame_is_bad = frame_overflow | (dfifo_wr_en & dfifo_wr_full) | frame_cmp;

      reg s_good_frame_r, s_bad_frame_r;

      always_ff @(posedge s_clk) begin
         if (s_rst) begin
            s_good_frame_r <= 0;
            s_bad_frame_r  <= 0;
         end else begin
            s_good_frame_r <= frame_tlast & ~frame_is_bad;
            s_bad_frame_r  <= frame_tlast &  frame_is_bad;
         end
      end

      assign s_good_frame = s_good_frame_r;
      assign s_bad_frame  = s_bad_frame_r;

   end
endgenerate

async_fifo #(
   .WIDTH               ( DATA_FIFO_WIDTH ),
   .DEPTH               ( DEPTH ),
   .FWFT                ( "true" ),
   .RAM_STYLE           ( RAM_STYLE )
) data_fifo_inst (
   .wr_rst              ( s_rst ),
   .wr_clk              ( s_clk ),
   .wr_en               ( dfifo_wr_en ),
   .wr_data             ( dfifo_wr_data ),
   .wr_full             ( dfifo_wr_full ),
   .wr_overflow         ( dfifo_wr_overflow ),
   .wr_level            ( dfifo_wr_level ),

   .rd_rst              ( m_rst ),
   .rd_clk              ( m_clk ),
   .rd_en               ( dfifo_rd_en ),
   .rd_data             ( dfifo_rd_data ),
   .rd_empty            ( dfifo_rd_empty ),
   .rd_underflow        ( dfifo_rd_underflow ),
   .rd_level            ( dfifo_rd_level )
);

assign dfifo_tdata = dfifo_rd_data[D_TDATA_OFF +: DATA_WIDTH];
assign dfifo_tlast = dfifo_rd_data[D_TLAST_OFF];
assign dfifo_tkeep = (AUX_ENABLE.keep && !AUX_FIFO_USE.keep) ? dfifo_rd_data[D_TKEEP_OFF +: KEEP_WIDTH] : '0;
assign dfifo_tstrb = (AUX_ENABLE.strb && !AUX_FIFO_USE.strb) ? dfifo_rd_data[D_TSTRB_OFF +: STRB_WIDTH] : '0;
assign dfifo_tid   = (AUX_ENABLE.id   && !AUX_FIFO_USE.id)   ? dfifo_rd_data[D_TID_OFF   +: ID_WIDTH]   : '0;
assign dfifo_tdest = (AUX_ENABLE.dest && !AUX_FIFO_USE.dest) ? dfifo_rd_data[D_TDEST_OFF +: DEST_WIDTH] : '0;
assign dfifo_tuser = (AUX_ENABLE.user && !AUX_FIFO_USE.user) ? dfifo_rd_data[D_TUSER_OFF +: USER_WIDTH] : '0;

generate
   if (AUX_FIFO_WIDTH > 0) begin : gen_aux
      wire                       afifo_wr_en;
      wire [AUX_FIFO_WIDTH-1:0]  afifo_wr_data;
      wire                       afifo_wr_full;
      wire                       afifo_wr_overflow;
      wire [AUX_LVLW-1:0]        afifo_wr_level;
      wire                       afifo_rd_en;
      wire [AUX_FIFO_WIDTH-1:0]  afifo_rd_data;
      wire                       afifo_rd_underflow;
      wire [AUX_LVLW-1:0]        afifo_rd_level;

      async_fifo #(
         .WIDTH                  ( AUX_FIFO_WIDTH ),
         .DEPTH                  ( AUX_DEPTH_VAL ),
         .FWFT                   ( "true" ),
         .RAM_STYLE              ( AUX_RAM_STYLE )
      ) aux_fifo_inst (
         .wr_rst                 ( s_rst ),
         .wr_clk                 ( s_clk ),
         .wr_en                  ( afifo_wr_en ),
         .wr_data                ( afifo_wr_data ),
         .wr_full                ( afifo_wr_full ),
         .wr_overflow            ( afifo_wr_overflow ),
         .wr_level               ( afifo_wr_level ),

         .rd_rst                 ( m_rst ),
         .rd_clk                 ( m_clk ),
         .rd_en                  ( afifo_rd_en ),
         .rd_data                ( afifo_rd_data ),
         .rd_empty               ( afifo_rd_empty ),
         .rd_underflow           ( afifo_rd_underflow ),
         .rd_level               ( afifo_rd_level )
      );

      assign afifo_wr_en = FRAME_FIFO
         ? (s_axis.tvalid & s_axis.tready & s_axis.tlast)
         : (s_axis.tvalid & s_axis.tready);

      if (AUX_ENABLE.keep && AUX_FIFO_USE.keep)
         assign afifo_wr_data[A_TKEEP_OFF +: KEEP_WIDTH] = s_axis.tkeep;
      if (AUX_ENABLE.strb && AUX_FIFO_USE.strb)
         assign afifo_wr_data[A_TSTRB_OFF +: STRB_WIDTH] = s_axis.tstrb;
      if (AUX_ENABLE.id && AUX_FIFO_USE.id)
         assign afifo_wr_data[A_TID_OFF +: ID_WIDTH] = s_axis.tid;
      if (AUX_ENABLE.dest && AUX_FIFO_USE.dest)
         assign afifo_wr_data[A_TDEST_OFF +: DEST_WIDTH] = s_axis.tdest;
      if (AUX_ENABLE.user && AUX_FIFO_USE.user)
         assign afifo_wr_data[A_TUSER_OFF +: USER_WIDTH] = s_axis.tuser;

      if (FRAME_FIFO)
         assign afifo_wr_data[A_BAD_OFF] = frame_is_bad;

      if (FRAME_FIFO) begin
         assign afifo_rd_en = afifo_rd_en_fsm;
      end else begin
         assign afifo_rd_en = dfifo_rd_en;
      end

      assign afifo_tkeep = (AUX_ENABLE.keep && AUX_FIFO_USE.keep) ? afifo_rd_data[A_TKEEP_OFF +: KEEP_WIDTH] : '0;
      assign afifo_tstrb = (AUX_ENABLE.strb && AUX_FIFO_USE.strb) ? afifo_rd_data[A_TSTRB_OFF +: STRB_WIDTH] : '0;
      assign afifo_tid   = (AUX_ENABLE.id   && AUX_FIFO_USE.id)   ? afifo_rd_data[A_TID_OFF   +: ID_WIDTH]   : '0;
      assign afifo_tdest = (AUX_ENABLE.dest && AUX_FIFO_USE.dest) ? afifo_rd_data[A_TDEST_OFF +: DEST_WIDTH] : '0;
      assign afifo_tuser = (AUX_ENABLE.user && AUX_FIFO_USE.user) ? afifo_rd_data[A_TUSER_OFF +: USER_WIDTH] : '0;

      assign afifo_bad_frame = FRAME_FIFO ? afifo_rd_data[A_BAD_OFF] : 1'b0;

      assign s_overflow      = dfifo_wr_overflow | afifo_wr_overflow;
      assign s_aux_level     = afifo_wr_level;
      assign s_aux_overflow  = afifo_wr_overflow;
      assign m_aux_level     = afifo_rd_level;
      assign m_aux_underflow = afifo_rd_underflow;
   end else begin : no_aux
      assign afifo_tkeep      = '0;
      assign afifo_tstrb      = '0;
      assign afifo_tid        = '0;
      assign afifo_tdest      = '0;
      assign afifo_tuser      = '0;
      assign afifo_bad_frame  = 1'b0;
      assign afifo_rd_empty   = 1'b1;
      assign s_overflow       = dfifo_wr_overflow;
      assign s_aux_level      = '0;
      assign s_aux_overflow   = 1'b0;
      assign m_aux_level      = '0;
      assign m_aux_underflow  = 1'b0;
   end
endgenerate

generate
   if (FRAME_FIFO) begin : gen_frame_rd
      reg [LVLW-1:0]             skip_cnt;
      reg                        afifo_rd_en_fsm_r;
      reg                        frame_dfifo_rd_en;
      reg [KEEP_WIDTH-1:0]       frame_tkeep_r;
      reg [STRB_WIDTH-1:0]       frame_tstrb_r;
      reg [ID_WIDTH-1:0]         frame_tid_r;
      reg [DEST_WIDTH-1:0]       frame_tdest_r;
      reg [USER_WIDTH-1:0]       frame_tuser_r;
      reg                        m_good_frame_r;
      reg                        m_bad_frame_r;

      wire frame_dfifo_tlast = dfifo_rd_data[D_TLAST_OFF];

      enum logic [1:0] {
         WAIT    = 0,
         FORWARD = 1,
         DISCARD = 2
      } state, nstate;

      always_ff @(posedge m_clk) begin
         if (m_rst)
            state <= WAIT;
         else
            state <= nstate;
      end

      always_comb begin
         nstate = state;
         case (state)
            WAIT:
            begin
               if (!afifo_rd_empty && !dfifo_rd_empty)
                  nstate = afifo_bad_frame ? DISCARD : FORWARD;
               else
                  nstate = WAIT;
            end

            FORWARD:
            begin
               if (m_axis.tvalid && m_axis.tready && frame_dfifo_tlast)
                  nstate = WAIT;
               else
                  nstate = FORWARD;
            end

            DISCARD:
            begin
               if (frame_dfifo_tlast)
                  nstate = WAIT;
               else if (dfifo_rd_empty && skip_cnt != 0)
                  nstate = WAIT;
               else if (skip_cnt == DEPTH[LVLW-1:0])
                  nstate = WAIT;
               else
                  nstate = DISCARD;
            end
            default: nstate = WAIT;
         endcase
       end

       always_ff @(posedge m_clk) begin
         if (m_rst)
            afifo_rd_en_fsm_r <= 0;
         else if (state == WAIT && !afifo_rd_empty && !dfifo_rd_empty)
            afifo_rd_en_fsm_r <= 1;
         else
            afifo_rd_en_fsm_r <= 0;
      end

      always_ff @(posedge m_clk) begin
         if (m_rst) begin
            frame_tkeep_r <= {KEEP_WIDTH{1'b1}};
            frame_tstrb_r <= {STRB_WIDTH{1'b1}};
            frame_tid_r   <= {ID_WIDTH{1'b0}};
            frame_tdest_r <= {DEST_WIDTH{1'b0}};
            frame_tuser_r <= {USER_WIDTH{1'b0}};
         end else if (state == WAIT && !afifo_rd_empty && !dfifo_rd_empty && !afifo_bad_frame) begin
            if (AUX_ENABLE.keep && AUX_FIFO_USE.keep)
               frame_tkeep_r <= afifo_tkeep;
            if (AUX_ENABLE.strb && AUX_FIFO_USE.strb)
               frame_tstrb_r <= afifo_tstrb;
            if (AUX_ENABLE.id && AUX_FIFO_USE.id)
               frame_tid_r   <= afifo_tid;
            if (AUX_ENABLE.dest && AUX_FIFO_USE.dest)
               frame_tdest_r <= afifo_tdest;
            if (AUX_ENABLE.user && AUX_FIFO_USE.user)
               frame_tuser_r <= afifo_tuser;
         end
      end

      always_ff @(posedge m_clk) begin
         if (m_rst)
            m_good_frame_r <= 0;
         else if (state == FORWARD && m_axis.tvalid && m_axis.tready && frame_dfifo_tlast)
            m_good_frame_r <= 1;
         else
            m_good_frame_r <= 0;
      end

      always_ff @(posedge m_clk) begin
         if (m_rst)
            frame_dfifo_rd_en <= 0;
         else if (state == DISCARD)
            frame_dfifo_rd_en <= !frame_dfifo_tlast & !dfifo_rd_empty;
         else
            frame_dfifo_rd_en <= 0;
      end

      always_ff @(posedge m_clk) begin
         if (m_rst)
            m_bad_frame_r <= 0;
         else if (state == DISCARD) begin
            if (frame_dfifo_tlast)
               m_bad_frame_r <= 1;
            else if (dfifo_rd_empty) begin
               if (skip_cnt != 0)
                  m_bad_frame_r <= 1;
               else
                  m_bad_frame_r <= 0;
            end else if (skip_cnt == DEPTH[LVLW-1:0])
               m_bad_frame_r <= 1;
            else
               m_bad_frame_r <= 0;
         end else
            m_bad_frame_r <= 0;
      end

      always_ff @(posedge m_clk) begin
         if (m_rst)
            skip_cnt <= 0;
         else if (state == WAIT && !afifo_rd_empty && !dfifo_rd_empty && afifo_bad_frame)
            skip_cnt <= 0;
         else if (state == DISCARD && !frame_dfifo_tlast && !dfifo_rd_empty && skip_cnt != DEPTH[LVLW-1:0])
            skip_cnt <= skip_cnt + 1;
      end

      assign afifo_rd_en_fsm = afifo_rd_en_fsm_r;
      assign frame_tkeep     = frame_tkeep_r;
      assign frame_tstrb     = frame_tstrb_r;
      assign frame_tid       = frame_tid_r;
      assign frame_tdest     = frame_tdest_r;
      assign frame_tuser     = frame_tuser_r;

      assign m_axis.tvalid  = (state == FORWARD) && !dfifo_rd_empty;
      assign dfifo_rd_en    = (state == FORWARD) ? (m_axis.tvalid && m_axis.tready)
                                                 : frame_dfifo_rd_en;
      assign m_good_frame   = m_good_frame_r;
      assign m_bad_frame    = m_bad_frame_r;

   end else begin : gen_no_frame_rd
      assign dfifo_rd_en   = m_axis.tready & !dfifo_rd_empty;
      assign m_axis.tvalid = !dfifo_rd_empty;

      assign s_good_frame  = 1'b0;
      assign s_bad_frame   = 1'b0;
      assign m_good_frame  = 1'b0;
      assign m_bad_frame   = 1'b0;
   end
endgenerate

assign m_axis.tdata = dfifo_tdata;
assign m_axis.tlast = dfifo_tlast;

generate
   if (!FRAME_FIFO) begin : gen_mux_normal
      assign m_axis.tkeep = AUX_ENABLE.keep ? (AUX_FIFO_USE.keep ? afifo_tkeep : dfifo_tkeep) : '0;
      assign m_axis.tstrb = AUX_ENABLE.strb ? (AUX_FIFO_USE.strb ? afifo_tstrb : dfifo_tstrb) : '0;
      assign m_axis.tid   = AUX_ENABLE.id   ? (AUX_FIFO_USE.id   ? afifo_tid   : dfifo_tid)   : '0;
      assign m_axis.tdest = AUX_ENABLE.dest ? (AUX_FIFO_USE.dest ? afifo_tdest : dfifo_tdest) : '0;
      assign m_axis.tuser = AUX_ENABLE.user ? (AUX_FIFO_USE.user ? afifo_tuser : dfifo_tuser) : '0;
   end else begin : gen_mux_frame
      assign m_axis.tkeep = AUX_ENABLE.keep ? (AUX_FIFO_USE.keep ? frame_tkeep : dfifo_tkeep) : '0;
      assign m_axis.tstrb = AUX_ENABLE.strb ? (AUX_FIFO_USE.strb ? frame_tstrb : dfifo_tstrb) : '0;
      assign m_axis.tid   = AUX_ENABLE.id   ? (AUX_FIFO_USE.id   ? frame_tid   : dfifo_tid)   : '0;
      assign m_axis.tdest = AUX_ENABLE.dest ? (AUX_FIFO_USE.dest ? frame_tdest : dfifo_tdest) : '0;
      assign m_axis.tuser = AUX_ENABLE.user ? (AUX_FIFO_USE.user ? frame_tuser : dfifo_tuser) : '0;
   end
endgenerate

assign m_underflow   = dfifo_rd_underflow;
assign s_level       = dfifo_wr_level;
assign m_level       = dfifo_rd_level;

endmodule
`resetall