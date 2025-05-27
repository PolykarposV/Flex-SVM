// Copyright 2021 The CFU-Playground Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module signed_chunk_abs (
    input  wire [31:0] in_data,
    input  wire [1:0]  mode,         // 2'b00 = 4-bit, 2'b01 = 8-bit, 2'b10 = 16-bit
    output reg  [31:0] magnitudes,   // Contains all magnitudes packed
    output reg  [7:0]  signs         // One sign bit per chunk (MSB = chunk0)
);

    integer i;
    reg signed [15:0] chunk;    

    always @(*) begin
        magnitudes = 0;
        signs = 0;

        case (mode)
            2'b00: begin // 4-bit mode -> 8 chunks
                for (i = 0; i < 8; i = i + 1) begin
                    // Extract chunk and interpret as signed
                    chunk = in_data[i*4 +: 4];
                    signs[i] = chunk[3];  // MSB is sign bit
                    magnitudes[i*4 +: 4] = (chunk[3] == 1) ? -chunk : chunk;
                end
            end

            2'b01: begin // 8-bit mode -> 4 chunks
                for (i = 0; i < 4; i = i + 1) begin
                    chunk = in_data[i*8 +: 8];
                    // Group signs for each batch (i.e., signs[7:6] for batch 1, signs[5:4] for batch 2)
                    signs[2*i + 1] = chunk[7];  // sign bit 7 for batch i
                    signs[2*i] = chunk[7];      // sign bit 7 for batch i
                    magnitudes[i*8 +: 8] = (chunk[7] == 1) ? -chunk : chunk;
                end
            end

            2'b10: begin // 16-bit mode -> 2 chunks
                for (i = 0; i < 2; i = i + 1) begin
                    chunk = in_data[i*16 +: 16];
                    // Group signs for each batch (i.e., signs[7:4] for batch 1, signs[3:0] for batch 2)
                    signs[4*i + 3] = chunk[15]; // sign bit 15 for batch i
                    signs[4*i + 2] = chunk[15]; // sign bit 15 for batch i
                    signs[4*i + 1] = chunk[15]; // sign bit 15 for batch i
                    signs[4*i] = chunk[15];     // sign bit 15 for batch i
                    magnitudes[i*16 +: 16] = (chunk[15] == 1) ? -chunk : chunk;
                end
            end

            default: begin
                magnitudes = 32'd0;
                signs = 8'd0;
            end
        endcase
    end

endmodule


module cfu
#(
  parameter WIDTH = 32
)(
  input  wire             i_clk,
  input  wire             i_rst,
  input      [WIDTH-1:0]   i_cfu_rs1,
  input      [WIDTH-1:0]   i_cfu_rs2,
  input      [2:0]    i_cfu_op,
  input               i_cfu_valid,
  input               i_ibus_ack,
  input               i_rf_rreq,
  input [31:0]         i_instruction,
  output              o_cfu_ready,
  output     [WIDTH-1:0]   o_cfu_rd
);

  wire valid  = i_cfu_valid;

  reg [7:0] id;
  reg signed [31:0] cur_sum;
  reg [7:0] max_id;
  reg signed [31:0] cur_max;
  reg signed [31:0] cur_sum_holder;

  wire [7:0] myprod1;
  wire [7:0] myprod2;
  wire [7:0] myprod3;
  wire [7:0] myprod4;
  wire [7:0] myprod5;
  wire [7:0] myprod6;
  wire [7:0] myprod7;
  wire [7:0] myprod8;
  wire [31:0] magnitudes;
  wire [7:0] signs;

  wire signed[21:0] weighted_sum;

  reg enable;
  reg done;

  reg signed [31:0] my_o;

    signed_chunk_abs uut (
        .in_data(i_cfu_rs2),
        .mode(i_cfu_op[2:1]),
        .magnitudes(magnitudes),
        .signs(signs)
    );

  assign myprod1 = i_cfu_rs1[31:28] * magnitudes[31:28];
  assign myprod2 = i_cfu_rs1[27:24] * magnitudes[27:24];
  assign myprod3 = i_cfu_rs1[23:20] * magnitudes[23:20];
  assign myprod4 = i_cfu_rs1[19:16] * magnitudes[19:16];
  assign myprod5 = i_cfu_rs1[15:12] * magnitudes[15:12];
  assign myprod6 = i_cfu_rs1[11:8] * magnitudes[11:8];
  assign myprod7 = i_cfu_rs1[7:4] * magnitudes[7:4];
  assign myprod8 = i_cfu_rs1[3:0] * magnitudes[3:0];

  assign weighted_sum =
    ((signs[0]) ? -(myprod8) : myprod8) +
    ((i_cfu_op[2:1] != 2'b00) ? ((signs[1]) ? -(myprod7 << 4) : (myprod7 << 4)) : (signs[1])? -myprod7 : myprod7) +
    ((i_cfu_op[2:1] == 2'b10) ? ((signs[2]) ? -(myprod6 << 8) : (myprod6 << 8)) : (signs[2])? -myprod6 : myprod6) +
    ((i_cfu_op[2:1] == 2'b01) ? ((signs[3]) ? -(myprod5 << 4) : (myprod5 << 4)) :
     (i_cfu_op[2:1] == 2'b10) ? ((signs[3]) ? -(myprod5 << 12) : (myprod5 << 12)) : (signs[3])? -myprod5 : myprod5) +
    ((signs[4]) ? -(myprod4) : myprod4) +
    ((i_cfu_op[2:1] != 2'b00) ? ((signs[5]) ? -(myprod3 << 4) : (myprod3 << 4)) : (signs[5])? -myprod3 : myprod3) +
    ((i_cfu_op[2:1] == 2'b10) ? ((signs[6]) ? -(myprod2 << 8) : (myprod2 << 8)) : (signs[6])? -myprod2 : myprod2) +
    ((i_cfu_op[2:1] == 2'b01) ? ((signs[7]) ? -(myprod1 << 4) : (myprod1 << 4)) :
     (i_cfu_op[2:1] == 2'b10) ? ((signs[7]) ? -(myprod1 << 12) : (myprod1 << 12)) : (signs[7])? -myprod1 : myprod1);

  always @(posedge i_clk) begin
    if(i_rst) begin
      enable <= 0;
      id <= 0;
      max_id <= 0;
      cur_sum <= 0;
      cur_max <= 0;
    end
    else if (valid) begin
      if(i_cfu_op[2:0] == 3'b111) begin
	id <= 0;
	cur_sum <= 0;
	cur_max <= 32'h80000000;
	max_id <= 0;
      end
      else if((i_cfu_op[2:0] == 3'b000 || i_cfu_op[2:0] == 3'b010 || i_cfu_op[2:0] == 3'b100) && !enable) begin
	cur_sum <= cur_sum + weighted_sum;
      end
      else if((i_cfu_op[2:0] == 3'b001 || i_cfu_op[2:0] == 3'b011 || i_cfu_op[2:0] == 3'b101) && !enable) begin
        cur_sum_holder <= cur_sum + weighted_sum;
	if(cur_max < (cur_sum + weighted_sum)) begin
	  cur_max <= cur_sum + weighted_sum;
	  max_id <= id;
	end
	  id <= id + 1;
	  cur_sum <= 0;
      end
      enable <= 1'b1;
    end else begin
      enable <= 0;
    end
  end

  always @(posedge i_clk) begin
    if(!i_rst & valid & enable) begin
      my_o <= (i_cfu_op[2:0] == 3'b001 || i_cfu_op[2:0] == 3'b011 || i_cfu_op[2:0] == 3'b101)? {cur_sum_holder[31],23'h000000,max_id} : 0;
    end
    else begin
      my_o <= 0;
    end
    done <= enable;
  end

  assign o_cfu_rd = my_o;
  assign o_cfu_ready = done & valid;


endmodule
