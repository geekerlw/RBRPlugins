#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// COLOR                    0   xyzw        0     NONE   float   xyzw
// SV_Position              0   xyzw        1      POS   float   xyzw
//
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c1         cb0             0         1  ( FLT, FLT, FLT, FLT)
// c2         cb0            22         4  ( FLT, FLT, FLT, FLT)
//
//
// Runtime generated constant mappings:
//
// Target Reg                               Constant Description
// ---------- --------------------------------------------------
// c0                              Vertex Shader position offset
//
//
// Level9 shader bytecode:
//
    vs_2_0
    dcl_texcoord v0  // vin<0,1,2,3>

#line 40 "E:\tmp\DirectXTK-mar2023\DirectXTK-mar2023\Src\Shaders\Common.fxh"
    dp4 oPos.z, v0, c4  // ::VSBasicNoFog<6>
    dp4 r0.x, v0, c2  // ::vout<0>
    dp4 r0.y, v0, c3  // ::vout<1>
    dp4 r0.z, v0, c5  // ::vout<3>

#line 52 "E:\tmp\DirectXTK-mar2023\DirectXTK-mar2023\Src\Shaders\BasicEffect.fx"
    mad oPos.xy, r0.z, c0, r0  // ::VSBasicNoFog<4,5>
    mov oPos.w, r0.z  // ::VSBasicNoFog<7>

#line 41 "E:\tmp\DirectXTK-mar2023\DirectXTK-mar2023\Src\Shaders\Common.fxh"
    mov oT0, c1  // ::VSBasicNoFog<0,1,2,3>

// approximately 7 instruction slots used
vs_4_0
dcl_constantbuffer CB0[26], immediateIndexed
dcl_input v0.xyzw
dcl_output o0.xyzw
dcl_output_siv o1.xyzw, position
mov o0.xyzw, cb0[0].xyzw
dp4 o1.x, v0.xyzw, cb0[22].xyzw
dp4 o1.y, v0.xyzw, cb0[23].xyzw
dp4 o1.z, v0.xyzw, cb0[24].xyzw
dp4 o1.w, v0.xyzw, cb0[25].xyzw
ret 
// Approximately 0 instruction slots used
#endif

const BYTE BasicEffect_VSBasicNoFog[] =
{
     68,  88,  66,  67,  12, 105, 
    213,  73, 168,  40,  82, 183, 
     54, 255, 251,  88, 176,  90, 
    130, 211,   1,   0,   0,   0, 
     24,   5,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    172,   3,   0,   0, 144,   4, 
      0,   0, 196,   4,   0,   0, 
     65, 111, 110,  57, 116,   3, 
      0,   0, 116,   3,   0,   0, 
      0,   2, 254, 255,  52,   3, 
      0,   0,  64,   0,   0,   0, 
      2,   0,  36,   0,   0,   0, 
     60,   0,   0,   0,  60,   0, 
      0,   0,  36,   0,   1,   0, 
     60,   0,   0,   0,   0,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  22,   0, 
      4,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   2, 254, 255, 254, 255, 
    172,   0,  68,  66,  85,  71, 
     40,   0,   0,   0, 132,   2, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0, 176,   0, 
      0,   0,   8,   0,   0,   0, 
    184,   0,   0,   0,   3,   0, 
      0,   0,  72,   2,   0,   0, 
    248,   0,   0,   0,  69,  58, 
     92, 116, 109, 112,  92,  68, 
    105, 114, 101,  99, 116,  88, 
     84,  75,  45, 109,  97, 114, 
     50,  48,  50,  51,  92,  68, 
    105, 114, 101,  99, 116,  88, 
     84,  75,  45, 109,  97, 114, 
     50,  48,  50,  51,  92,  83, 
    114,  99,  92,  83, 104,  97, 
    100, 101, 114, 115,  92,  67, 
    111, 109, 109, 111, 110,  46, 
    102, 120, 104,   0,  69,  58, 
     92, 116, 109, 112,  92,  68, 
    105, 114, 101,  99, 116,  88, 
     84,  75,  45, 109,  97, 114, 
     50,  48,  50,  51,  92,  68, 
    105, 114, 101,  99, 116,  88, 
     84,  75,  45, 109,  97, 114, 
     50,  48,  50,  51,  92,  83, 
    114,  99,  92,  83, 104,  97, 
    100, 101, 114, 115,  92,  66, 
     97, 115, 105,  99,  69, 102, 
    102, 101,  99, 116,  46, 102, 
    120,   0,  40,   0,   0,   0, 
    106,   0,   0,   0,   0,   0, 
    255, 255, 184,   2,   0,   0, 
     40,   0,   0,   0, 196,   2, 
      0,   0,  40,   0,   0,   0, 
    212,   2,   0,   0,  40,   0, 
      0,   0, 228,   2,   0,   0, 
     40,   0,   0,   0, 244,   2, 
      0,   0,  52,   0,   1,   0, 
      4,   3,   0,   0,  52,   0, 
      1,   0,  24,   3,   0,   0, 
     41,   0,   0,   0,  36,   3, 
      0,   0,  86,  83,  66,  97, 
    115, 105,  99,  78, 111,  70, 
    111, 103,   0,  68, 105, 102, 
    102, 117, 115, 101,   0, 171, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     80, 111, 115, 105, 116, 105, 
    111, 110,  80,  83,   0, 171, 
      5,   1,   0,   0,  16,   1, 
      0,   0,  32,   1,   0,   0, 
     16,   1,   0,   0,   5,   0, 
      0,   0,   1,   0,   8,   0, 
      1,   0,   2,   0,  44,   1, 
      0,   0,   1,   0,   0,   0, 
    255, 255, 255, 255,   6,   0, 
    255, 255,   5,   0,   0,   0, 
      4,   0,   5,   0, 255, 255, 
    255, 255,   6,   0,   0,   0, 
    255, 255, 255, 255, 255, 255, 
      7,   0,   7,   0,   0,   0, 
      0,   0,   1,   0,   2,   0, 
      3,   0, 118, 105, 110,   0, 
     80, 111, 115, 105, 116, 105, 
    111, 110,   0, 171, 171, 171, 
    128,   1,   0,   0,  16,   1, 
      0,   0,   5,   0,   0,   0, 
      1,   0,   4,   0,   1,   0, 
      1,   0, 140,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   2,   0,   3,   0, 
    118, 111, 117, 116,   0,  80, 
    111, 115,  95, 112, 115,   0, 
     83, 112, 101,  99, 117, 108, 
     97, 114,   0, 171, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      3,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  70, 111, 
    103,  70,  97,  99, 116, 111, 
    114,   0, 171, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 181,   1,   0,   0, 
     16,   1,   0,   0,   5,   1, 
      0,   0,  16,   1,   0,   0, 
    188,   1,   0,   0, 200,   1, 
      0,   0, 216,   1,   0,   0, 
    228,   1,   0,   0,   5,   0, 
      0,   0,   1,   0,  12,   0, 
      1,   0,   4,   0, 244,   1, 
      0,   0,   2,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
    255, 255,   3,   0,   0,   0, 
    255, 255,   1,   0, 255, 255, 
    255, 255,   4,   0,   0,   0, 
    255, 255, 255, 255,   3,   0, 
    255, 255,   0,   0,   0,   0, 
    248,   0,   0,   0,  60,   1, 
      0,   0,   4,   0,   0,   0, 
     76,   1,   0,   0, 248,   0, 
      0,   0, 124,   1,   0,   0, 
    148,   1,   0,   0,   1,   0, 
      0,   0, 164,   1,   0,   0, 
      0,   0,   0,   0, 176,   1, 
      0,   0,  20,   2,   0,   0, 
      3,   0,   0,   0,  36,   2, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     31,   0,   0,   2,   5,   0, 
      0, 128,   0,   0,  15, 144, 
      9,   0,   0,   3,   0,   0, 
      4, 192,   0,   0, 228, 144, 
      4,   0, 228, 160,   9,   0, 
      0,   3,   0,   0,   1, 128, 
      0,   0, 228, 144,   2,   0, 
    228, 160,   9,   0,   0,   3, 
      0,   0,   2, 128,   0,   0, 
    228, 144,   3,   0, 228, 160, 
      9,   0,   0,   3,   0,   0, 
      4, 128,   0,   0, 228, 144, 
      5,   0, 228, 160,   4,   0, 
      0,   4,   0,   0,   3, 192, 
      0,   0, 170, 128,   0,   0, 
    228, 160,   0,   0, 228, 128, 
      1,   0,   0,   2,   0,   0, 
      8, 192,   0,   0, 170, 128, 
      1,   0,   0,   2,   0,   0, 
     15, 224,   1,   0, 228, 160, 
    255, 255,   0,   0,  83,  72, 
     68,  82, 220,   0,   0,   0, 
     64,   0,   1,   0,  55,   0, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,  26,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   0,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   6, 242,  32,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  17,   0, 
      0,   8,  18,  32,  16,   0, 
      1,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,  22,   0,   0,   0, 
     17,   0,   0,   8,  34,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  30,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,  23,   0, 
      0,   0,  17,   0,   0,   8, 
     66,  32,  16,   0,   1,   0, 
      0,   0,  70,  30,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     24,   0,   0,   0,  17,   0, 
      0,   8, 130,  32,  16,   0, 
      1,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,  25,   0,   0,   0, 
     62,   0,   0,   1,  73,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,  15, 
      0,   0,  83,  86,  95,  80, 
    111, 115, 105, 116, 105, 111, 
    110,   0,  79,  83,  71,  78, 
     76,   0,   0,   0,   2,   0, 
      0,   0,   8,   0,   0,   0, 
     56,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     62,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   0,   0,   0, 
     67,  79,  76,  79,  82,   0, 
     83,  86,  95,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
    171, 171
};