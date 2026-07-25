//******************************************************************************
// async_fifo.v
// SPDX-License-Identifier: MIT
//
// Async FIFO based on Cliff Cummings' SNUG 2002 paper:
//   "Simulation and Synthesis Techniques for Asynchronous FIFO Design"
// Dual-clock gray-code pointer synchronization with 2-stage flip-flops.
// Supports full DEPTH usage via extended MSB pointer compare.
//
// Parameter ranges:
//   WIDTH      > 0
//   DEPTH      power-of-2, >= 2
//   FWFT       "true" for first-word fall-through, "false" for registered read
//   RAM_STYLE  Xilinx:"block","distributed" | Altera:"M9K","M20K","MLAB","logic"
//
// https://github.com/dozecat/memory_lib
//******************************************************************************

`timescale 1 ns / 1 ps
`default_nettype none

module async_fifo
#(
   parameter WIDTH            = 8,
   parameter DEPTH            = 1024,
   parameter FWFT             = "false",
   parameter RAM_STYLE        = "distributed"
)(
   input  wire                 rst,

   input  wire                 wr_clk,
   input  wire                 wr_en,
   input  wire [WIDTH-1:0]     wr_data,
   output wire                 wr_full,
   output reg                  wr_overflow,
   output wire [LVLW-1:0]      wr_level,

   input  wire                 rd_clk,
   input  wire                 rd_en,
   output reg [WIDTH-1:0]      rd_data,
   output wire                 rd_empty,
   output reg                  rd_underflow,
   output wire [LVLW-1:0]      rd_level
);

// parameters and width constants
localparam AWID = $clog2(DEPTH);       // memory address width
localparam PTRW = AWID + 1;            // pointer width (extra MSB for full/empty)
localparam LVLW = $clog2(DEPTH + 1);   // level output width (0..DEPTH)

localparam [PTRW-1:0] PTR_ZERO = {PTRW{1'b0}};
localparam [PTRW-1:0] PTR_ONE  = {{PTRW-1{1'b0}}, 1'b1};

// binary-gray conversion functions
function [PTRW-1:0] bin2gray;
   input [PTRW-1:0] bin;
   begin
      bin2gray = (bin >> 1) ^ bin;
   end
endfunction

function [PTRW-1:0] gray2bin;
   input [PTRW-1:0] gray;
   integer i;
   reg [PTRW-1:0] tmp;
   begin
      tmp = gray;
      for (i = 1; i < PTRW; i = 2 * i)
         tmp = tmp ^ (tmp >> i);
      gray2bin = tmp;
   end
endfunction

// pointer registers
reg [PTRW-1:0] wr_ptr;
reg [PTRW-1:0] wr_gray;
(* ASYNC_REG = "true" *)
reg [PTRW-1:0] wr_gray_ms;
(* ASYNC_REG = "true" *)
reg [PTRW-1:0] wr_gray_rd;

reg [PTRW-1:0] rd_ptr;
reg [PTRW-1:0] rd_gray;
(* ASYNC_REG = "true" *)
reg [PTRW-1:0] rd_gray_ms;
(* ASYNC_REG = "true" *)
reg [PTRW-1:0] rd_gray_wr;

wire [PTRW-1:0] rd_ptr_bin = gray2bin(rd_gray_wr);
wire [PTRW-1:0] wr_ptr_bin = gray2bin(wr_gray_rd);

// full / empty detection
wire wfull_val = (wr_ptr[AWID] != rd_ptr_bin[AWID]) &&
                  (wr_ptr[AWID-1:0] == rd_ptr_bin[AWID-1:0]);
wire rempty_val = (rd_ptr == wr_ptr_bin);

assign wr_full  = wfull_val;
assign rd_empty = rempty_val;

// level
wire [PTRW-1:0] wdiff = wr_ptr - rd_ptr_bin;
wire [PTRW-1:0] rdiff = wr_ptr_bin - rd_ptr;
assign wr_level = wdiff[LVLW-1:0];
assign rd_level = rdiff[LVLW-1:0];

// memory
(* ram_style = RAM_STYLE, ramstyle = RAM_STYLE *)
reg [WIDTH-1:0] mem [0:(1<<AWID)-1];

// parameter validation
initial begin
   if ((DEPTH < 2) || ((DEPTH & (DEPTH - 1)) != 0)) begin
      $error("async_fifo DEPTH must be power-of-2 and >= 2, got %0d", DEPTH);
      $finish;
   end
end

integer i;
initial
   for (i = 0; i < (1<<AWID); i = i + 1)
      mem[i] = {WIDTH{1'b0}};

// write pointer
always @(posedge wr_clk or posedge rst) begin
   if (rst) begin
      wr_ptr <= PTR_ZERO;
      wr_gray <= PTR_ZERO;
   end else if (wr_en && !wr_full) begin
      wr_ptr <= wr_ptr + PTR_ONE;
      wr_gray <= bin2gray(wr_ptr + PTR_ONE);
   end
end

// write-side overflow flag
always @(posedge wr_clk or posedge rst) begin
   if (rst)
      wr_overflow <= 1'b0;
   else
      wr_overflow <= wr_en & wr_full;
end

// read pointer sync to wr_clk domain
always @(posedge wr_clk) begin
   rd_gray_ms <= rd_gray;
   rd_gray_wr <= rd_gray_ms;
end

// read pointer
always @(posedge rd_clk or posedge rst) begin
   if (rst) begin
      rd_ptr <= PTR_ZERO;
      rd_gray <= PTR_ZERO;
   end else if (rd_en && !rd_empty) begin
      rd_ptr <= rd_ptr + PTR_ONE;
      rd_gray <= bin2gray(rd_ptr + PTR_ONE);
   end
end

// read-side underflow flag
always @(posedge rd_clk or posedge rst) begin
   if (rst)
      rd_underflow <= 1'b0;
   else
      rd_underflow <= rd_en & rd_empty;
end

// write pointer sync to rd_clk domain
always @(posedge rd_clk) begin
   wr_gray_ms <= wr_gray;
   wr_gray_rd <= wr_gray_ms;
end

// memory write
always @(posedge wr_clk) begin
   if (wr_en && !wr_full)
      mem[wr_ptr[AWID-1:0]] <= wr_data;
end

// memory read
generate
   if (FWFT == "true") begin : fwft
      always @(posedge rd_clk) begin
         if (rd_en && !rd_empty)
            rd_data <= mem[rd_ptr[AWID-1:0] + 1];
         else
            rd_data <= mem[rd_ptr[AWID-1:0]];
      end
   end else begin : registered
      always @(posedge rd_clk) begin
         if (rd_en && !rd_empty)
            rd_data <= mem[rd_ptr[AWID-1:0]];
      end
   end
endgenerate

endmodule

`resetall
