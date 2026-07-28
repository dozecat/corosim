//*****************************************************************************
// Copyright (C) 2026 dozecat. All rights reserved.
// SPDX-License-Identifier: MIT
//
// File:        axis_aux_pkg.sv
// Description: AXI4-Stream side-channel configuration type
// Repository:  https://github.com/dozecat/axi_lib.git
//
// Modification History:
// Ver   Who       Date        Changes
// ----  ----  ----------  ----------------------------------------------------
// 1.0         2026/6/9    Initial release
//*****************************************************************************

package axis_aux_pkg;
   typedef struct packed {
      logic keep;
      logic strb;
      logic id;
      logic dest;
      logic user;
   } axis_aux_t;
endpackage