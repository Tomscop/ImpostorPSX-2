/*
 * PSn00bSDK MDEC library
 * (C) 2022-2023 spicyjpeg - MPL licensed
 */

/**
 * @file psxpress.h
 * @brief MDEC library header
 *
 * @details This is a fully original reimplementation of the official SDK's
 * "data compression" library. This library is made up of two parts, the MDEC
 * API and functions to decompress Huffman-encoded bitstreams (.BS files, or
 * frames in .STR files) into data to be fed to the MDEC. Two different
 * implementations of the latter are provided, one using the GTE and scratchpad
 * region and an older one using a large lookup table in main RAM.
 *
 * FMV playback is not part of this library per se, but can implemented using
 * the APIs defined here alongside some code to stream data from the CD drive.
 *
 * Currently bitstream versions 1, 2 and 3 are supported. Version 0 and .IKI
 * bitstreams are not supported, but no encoder is publicly available for those
 * anyway.
 */

#ifndef __PSXPRESS_H
#define __PSXPRESS_H

#include <stdint.h>
#include <stddef.h>

/* Structure definitions */

typedef struct _VLC_TableV2 {
	uint16_t ac0[2];
	uint32_t ac2[8], ac3[64];
	uint16_t ac4[8], ac5[8], ac7[16], ac8[32];
	uint16_t ac9[32], ac10[32], ac11[32], ac12[32];
} VLC_TableV2;

typedef struct _VLC_TableV3 {
	uint16_t ac0[2];
	uint32_t ac2[8], ac3[64];
	uint16_t ac4[8], ac5[8], ac7[16], ac8[32];
	uint16_t ac9[32], ac10[32], ac11[32], ac12[32];
	uint8_t  dc[128], dc_len[9];
	uint8_t  _reserved[3];
} VLC_TableV3;

typedef struct _VLC_Context {
	const uint32_t	*input;
	uint32_t		window, next_window, remaining;
	int8_t			is_v3, bit_offset, block_index, coeff_index;
	uint16_t		quant_scale;
	int16_t			last_y, last_cr, last_cb;
} VLC_Context;

// Despite what some docs claim, the "number of 32-byte blocks" and "always
// 0x3800" fields are actually a single 32-bit field which is copied over to
// the output buffer, then parsed by DecDCTin() and written to the MDEC0
// register.
typedef struct {
	uint32_t mdec0_header;
	uint16_t quant_scale;
	uint16_t version;
} BS_Header;

/* Public API */

#ifdef __cplusplus
extern "C" {
#endif

int DecDCTvlcStart(VLC_Context *ctx, uint32_t *buf, size_t max_size, const uint32_t *bs);

/**
 * @brief Resumes or finishes decompressing a .BS file into MDEC codes.
 *
 * @details Resumes the decompression process started by DecDCTvlcStart(). The
 * state of the decompressor is contained entirely in the VLC_Context structure
 * so an arbitrary number of bitstreams can be decoded concurrently (although
 * the limited CPU power makes it impractical to do so) by keeping a separate
 * context for each bitstream.
 *
 * This function behaves like DecDCTvlcStart(), returning 1 if more data has to
 * be written or 0 otherwise. DecDCTvlcContinue() shall not be called after a
 * previous call to DecDCTvlcStart() or DecDCTvlcContinue() with the same
 * context returned 0; in that case the context shall be discarded or reused to
 * decode another bitstream.
 *
 * The contents of the GTE's LZCS and LZCR registers, if any, will be
 * destroyed.
 *
 * See DecDCTvlcStart() for more details.
 *
 * @param ctx Pointer to already initialized VLC_Context structure
 * @param buf
 * @param max_size Maximum number of 32-bit words to output
 * @return 0, 1 if more data needs to be output or -1 in case of failure
 *
 * @see DecDCTvlcStart()
 */
int DecDCTvlcContinue(VLC_Context *ctx, uint32_t *buf, size_t max_size);

/**
 * @brief Decompresses a .BS file into MDEC codes.
 *
 * @details A wrapper around DecDCTvlcStart() and DecDCTvlcContinue() for
 * compatibility with the official SDK. This function uses an internal context;
 * additionally, the maximum output buffer size is not passed as an argument
 * but is instead set by calling DecDCTvlcSize().
 *
 * This function behaves identically to DecDCTvlcContinue() if bs = 0 and
 * DecDCTvlcStart() otherwise.
 *
 * See DecDCTvlcStart() for more details.
 *
 * WARNING: InitGeom() must be called prior to using DecDCTvlc() for the first
 * time. Attempting to call this function with the GTE disabled will result in
 * a crash.
 *
 * @param bs Pointer to bitstream data or 0 to resume decoding
 * @param buf
 * @return 0, 1 if more data needs to be output or -1 in case of failure
 *
 * @see DecDCTvlcSize(), DecDCTvlcCopyTableV2(), DecDCTvlcCopyTableV3()
 */
void DecDCTvlcCopyTableV3(VLC_TableV3 *addr);

/**
 * @brief Decompresses or begins decompressing a .BS file into MDEC codes
 * (alternate implementation).
 *
 * @details Begins decompressing the contents of a .BS file (or of a single STR
 * frame) into a buffer that can be passed to DecDCTin(). This function uses a
 * large (34 KB) lookup table that must be loaded into main RAM beforehand by
 * calling DecDCTvlcBuild(), but does not use the GTE nor the scratchpad.
 * Depending on the specific bitstream being decoded DecDCTvlcStart2() might be
 * slightly faster or slower than DecDCTvlcStart() with its lookup table copied
 * to the scratchpad (see DecDCTvlcCopyTableV2() and DecDCTvlcCopyTableV3()).
 * DecDCTvlcStart() with the table in main RAM tends to be much slower.
 *
 * A VLC_Context object must be created and passed to this function, which will
 * then proceed to initialize its fields. The max_size argument sets the
 * maximum number of words that will be written to the output buffer; if more
 * data needs to be written, this function will return 1. To continue decoding
 * call DecDCTvlcContinue2() with the same VLC_Context object (the output
 * buffer can be different). If max_size = 0, the entire frame will always be
 * decoded in one shot.
 *
 * Only bitstream version 2 is currently supported.
 *
 * @param ctx Pointer to VLC_Context structure (which will be initialized)
 * @param buf
 * @param max_size Maximum number of 32-bit words to output
 * @param bs
 * @return 0, 1 if more data needs to be output or -1 in case of failure
 *
 * @see DecDCTvlcContinue2(), DecDCTvlcBuild()
 */

#ifdef __cplusplus
}
#endif

#endif
