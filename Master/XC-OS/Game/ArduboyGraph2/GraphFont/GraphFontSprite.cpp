#pragma once

const uint8_t GRAPHFONT_ATLAS[] PROGMEM = { // Starts at 33, ends at 126
	/**/   0,   1,   4,   7,  10,  13,  17,
	 18,  20,  22,  25,  28,  29,  32,  33,
	 36,  39,  42,  45,  48,  51,  54,  57,
	 60,  63,  66,  67,  68,  71,  73,  76,
	 79,  83,  86,  89,  92,  95,  97,  99,
	102, 105, 108, 110, 113, 115, 120, 124,
	127, 130, 133, 136, 138, 141, 144, 147,
	152, 155, 158, 160, 162, 165, 167, 170,
	173, 175, 178, 181, 183, 186, 189, 191,
	194, 197, 198, 200, 203, 204, 209, 212,
	215, 218, 221, 223, 225, 227, 230, 233,
	238, 241, 244, 246, 249, 250, 253,   0 // (0 - 253 = 256 - 253, so last entry still works.)
};

const uint8_t GRAPHFONT_SPRITE[] PROGMEM = {
	0x2f, 0x03, 0x00, 0x03, 0x3f, 0x0a, 0x3f, 0x16,
	0x3f, 0x1a, 0x09, 0x04, 0x12, 0x1a, 0x25, 0x1a,
	0x28, 0x03, 0x1e, 0x21, 0x21, 0x1e, 0x0a, 0x04,
	0x0a, 0x04, 0x0e, 0x04, 0x60, 0x04, 0x04, 0x04,
	0x20, 0x08, 0x04, 0x02, 0x1e, 0x21, 0x1e, 0x22,
	0x3f, 0x20, 0x39, 0x25, 0x22, 0x21, 0x25, 0x1a,
	0x07, 0x04, 0x3f, 0x17, 0x25, 0x19, 0x1e, 0x25,
	0x19, 0x01, 0x39, 0x07, 0x1a, 0x25, 0x1a, 0x12,
	0x25, 0x1e, 0x14, 0x34, 0x04, 0x0a, 0x11, 0x14,
	0x14, 0x11, 0x0a, 0x04, 0x01, 0x2d, 0x06, 0x1e,
	0x21, 0x2d, 0x2e, 0x3e, 0x05, 0x3e, 0x3f, 0x25,
	0x1a, 0x1e, 0x21, 0x21, 0x3f, 0x21, 0x1e, 0x3f,
	0x25, 0x3f, 0x05, 0x1e, 0x21, 0x3d, 0x3f, 0x04,
	0x3f, 0x21, 0x3f, 0x21, 0x21, 0x1f, 0x3f, 0x04,
	0x3b, 0x3f, 0x20, 0x3f, 0x02, 0x04, 0x02, 0x3f,
	0x3f, 0x02, 0x04, 0x3f, 0x1e, 0x21, 0x1e, 0x3f,
	0x05, 0x02, 0x1e, 0x31, 0x2e, 0x3f, 0x05, 0x3a,
	0x26, 0x19, 0x01, 0x3f, 0x01, 0x3f, 0x20, 0x3f,
	0x1f, 0x30, 0x1f, 0x1f, 0x20, 0x1c, 0x20, 0x1f,
	0x3b, 0x04, 0x3b, 0x03, 0x3c, 0x03, 0x39, 0x27,
	0x3f, 0x21, 0x02, 0x04, 0x08, 0x21, 0x3f, 0x02,
	0x01, 0x02, 0x20, 0x20, 0x20, 0x01, 0x02, 0x14,
	0x2c, 0x38, 0x3f, 0x24, 0x18, 0x18, 0x24, 0x18,
	0x24, 0x3f, 0x18, 0x34, 0x28, 0x3e, 0x05, 0x48,
	0x54, 0x3c, 0x3f, 0x04, 0x38, 0x3d, 0x40, 0x3d,
	0x3f, 0x04, 0x3a, 0x3f, 0x3c, 0x04, 0x3c, 0x04,
	0x38, 0x3c, 0x04, 0x38, 0x18, 0x24, 0x18, 0x7c,
	0x24, 0x18, 0x18, 0x24, 0x7c, 0x3c, 0x04, 0x28,
	0x14, 0x1f, 0x24, 0x1c, 0x20, 0x3c, 0x1c, 0x20,
	0x1c, 0x1c, 0x20, 0x38, 0x20, 0x3c, 0x34, 0x08,
	0x34, 0x4c, 0x50, 0x3c, 0x34, 0x2c, 0x04, 0x3b,
	0x21, 0x3b, 0x21, 0x3b, 0x04, 0x02, 0x06, 0x04
};