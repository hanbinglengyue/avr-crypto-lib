/* shabal.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * \file    shabal.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-17
 * \license GPLv3 or later
 * 
 */

#include <stdint.h>
#include "shabal.h"
#include <avr/pgmspace.h>
#include <string.h>

#define SHABAL_O1 13
#define SHABAL_O2  9
#define SHABAL_O3  6


static inline
uint32_t shabal_u(uint32_t a){
	return (a<<1)+a; /* a*3 */
}

static inline
uint32_t shabal_v(uint32_t a){
	return (a<<2)+a; /* a*5 */
}

#define ROTL32(a,n) (((a)<<(n))|((a)>>(32-(n))))

static
void shabal_p(shabal_ctx_t* ctx, const void* m){
	uint8_t i,j;
	for(i=0;i<16;++i){
		ctx->b[i] = ROTL32(ctx->b[i],17);
	}
	for(j=0;j<SHABAL_P;++j){
		for(i=0;i<16;++i){
			ctx->a[(i+16*j)%SHABAL_R] =
				shabal_u(ctx->a[(i+16*j)%SHABAL_R] 
					^ shabal_v(ROTL32(ctx->a[(i+16*j+SHABAL_R-1)%SHABAL_R],15))
				    ^ ctx->c[(8-i+16)%16])
				^ ctx->b[(i+SHABAL_O1)%16]
				^ ((ctx->b[(i+SHABAL_O2)%16]) & ~(ctx->b[(i+SHABAL_O3)%16]))
				^ ((uint32_t*)m)[i];
			ctx->b[i] = ROTL32(ctx->b[i], 1) ^ ~(ctx->a[(i+16*j)%SHABAL_R]);
		}
	}
	
	for(j=0;j<36;++j){
		ctx->a[j%SHABAL_R] += ctx->c[(j+3)%16];
	}
/*
	for (j=0; j<36; j++){
        ctx->a[(36 - 1 - j)%12] = 
                ctx->a[(36 - 1 - j) % 12]
                + ctx->c[(36 * 16 + 6 - j) % 16];

	}
*/
}

uint32_t shabal192_iv[] PROGMEM = {
	/* A */
	0xFD749ED4, 0xB798E530, 0x33904B6F, 0x46BDA85E,
	0x076934B4, 0x454B4058, 0x77F74527, 0xFB4CF465,
    0x62931DA9, 0xE778C8DB, 0x22B3998E, 0xAC15CFB9,
	/* B */
	0x58BCBAC4, 0xEC47A08E, 0xAEE933B2, 0xDFCBC824,
	0xA7944804, 0xBF65BDB0, 0x5A9D4502, 0x59979AF7,
    0xC5CEA54E, 0x4B6B8150, 0x16E71909, 0x7D632319,
	0x930573A0, 0xF34C63D1, 0xCAF914B4, 0xFDD6612C,
	/* C */
	0x61550878, 0x89EF2B75, 0xA1660C46, 0x7EF3855B,
	0x7297B58C, 0x1BC67793, 0x7FB1C723, 0xB66FC640,
    0x1A48B71C, 0xF0976D17, 0x088CE80A, 0xA454EDF3,
	0x1C096BF4, 0xAC76224B, 0x5215781C, 0xCD5D2669
};


uint32_t shabal224_iv[] PROGMEM = {
	/* A */
	0xA5201467, 0xA9B8D94A, 0xD4CED997, 0x68379D7B, 
	0xA7FC73BA, 0xF1A2546B, 0x606782BF, 0xE0BCFD0F,
	0x2F25374E, 0x069A149F, 0x5E2DFF25, 0xFAECF061,
	/* B */ 
	0xEC9905D8, 0xF21850CF, 0xC0A746C8, 0x21DAD498,
	0x35156EEB, 0x088C97F2, 0x26303E40, 0x8A2D4FB5,
    0xFEEE44B6, 0x8A1E9573, 0x7B81111A, 0xCBC139F0, 
	0xA3513861, 0x1D2C362E, 0x918C580E, 0xB58E1B9C,
	/* C */
	0xE4B573A1, 0x4C1A0880, 0x1E907C51, 0x04807EFD, 
	0x3AD8CDE5, 0x16B21302, 0x02512C53, 0x2204CB18,
    0x99405F2D, 0xE5B648A1, 0x70AB1D43, 0xA10C25C2, 
	0x16F1AC05, 0x38BBEB56, 0x9B01DC60, 0xB1096D83
};

uint32_t shabal256_iv[] PROGMEM = {
	/* A */ 
	0x52F84552, 0xE54B7999, 0x2D8EE3EC, 0xB9645191, 
	0xE0078B86, 0xBB7C44C9, 0xD2B5C1CA, 0xB0D2EB8C,
	0x14CE5A45, 0x22AF50DC, 0xEFFDBC6B, 0xEB21B74A,
	/* B */ 
	0xB555C6EE, 0x3E710596, 0xA72A652F, 0x9301515F, 
	0xDA28C1FA, 0x696FD868, 0x9CB6BF72, 0x0AFE4002,
    0xA6E03615, 0x5138C1D4, 0xBE216306, 0xB38B8890, 
	0x3EA8B96B, 0x3299ACE4, 0x30924DD4, 0x55CB34A5,
	/* C */ 
	0xB405F031, 0xC4233EBA, 0xB3733979, 0xC0DD9D55,
	0xC51C28AE, 0xA327B8E1, 0x56C56167, 0xED614433,
	0x88B59D60, 0x60E2CEBA, 0x758B4B8B, 0x83E82A7F, 
	0xBC968828, 0xE6E00BF7, 0xBA839E55, 0x9B491C60
};

uint32_t shabal384_iv[] PROGMEM = {
	/* A */ 
	0xC8FCA331, 0xE55C504E, 0x003EBF26, 0xBB6B8D83, 
	0x7B0448C1, 0x41B82789, 0x0A7C9601, 0x8D659CFF,
	0xB6E2673E, 0xCA54C77B, 0x1460FD7E, 0x3FCB8F2D,
	/* B */ 
	0x527291FC, 0x2A16455F, 0x78E627E5, 0x944F169F, 
	0x1CA6F016, 0xA854EA25, 0x8DB98ABE, 0xF2C62641,
    0x30117DCB, 0xCF5C4309, 0x93711A25, 0xF9F671B8, 
	0xB01D2116, 0x333F4B89, 0xB285D165, 0x86829B36,
	/* C */ 
	0xF764B11A, 0x76172146, 0xCEF6934D, 0xC6D28399, 
	0xFE095F61, 0x5E6018B4, 0x5048ECF5, 0x51353261,
    0x6E6E36DC, 0x63130DAD, 0xA9C69BD6, 0x1E90EA0C, 
	0x7C35073B, 0x28D95E6D, 0xAA340E0D, 0xCB3DEE70
};

uint32_t shabal512_iv[] PROGMEM = {
	/* A */ 
	0x20728DFD, 0x46C0BD53, 0xE782B699, 0x55304632, 
	0x71B4EF90, 0x0EA9E82C, 0xDBB930F1, 0xFAD06B8B,
	0xBE0CAE40, 0x8BD14410, 0x76D2ADAC, 0x28ACAB7F,
	/* B */ 
	0xC1099CB7, 0x07B385F3, 0xE7442C26, 0xCC8AD640, 
	0xEB6F56C7, 0x1EA81AA9, 0x73B9D314, 0x1DE85D08,
	0x48910A5A, 0x893B22DB, 0xC5A0DF44, 0xBBC4324E, 
	0x72D2F240, 0x75941D99, 0x6D8BDE82, 0xA1A7502B,
	/* C */
	0xD9BF68D1, 0x58BAD750, 0x56028CB2, 0x8134F359, 
	0xB5D469D8, 0x941A8CC2, 0x418B2A6E, 0x04052780,
	0x7F07D787, 0x5194358F, 0x3C60D665, 0xBE97D79A, 
	0x950C3434, 0xAED9A06D, 0x2537DC8D, 0x7CDB5969,
};

void shabal192_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal192_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal192_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal192_iv[SHABAL_R+16+i]));
	}
}

void shabal224_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal224_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal224_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal224_iv[SHABAL_R+16+i]));
	}
}
void shabal256_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal256_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal256_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal256_iv[SHABAL_R+16+i]));
	}
}
void shabal384_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal384_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal384_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal384_iv[SHABAL_R+16+i]));
	}
}
void shabal512_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal512_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal512_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal512_iv[SHABAL_R+16+i]));
	}
}

void shabal_nextBlock(shabal_ctx_t* ctx, const void* block){
	uint8_t i;
	uint32_t* t;
	for(i=0;i<16;++i){
		ctx->b[i] += ((uint32_t*)block)[i];
	}
	ctx->a[0] ^= ctx->w.w32[0];
	ctx->a[1] ^= ctx->w.w32[1];
	shabal_p(ctx, block);
	for(i=0;i<16;++i){
		ctx->c[i] -= ((uint32_t*)block)[i];
	}
	ctx->w.w64++;
	t = ctx->c;
	ctx->c = ctx->b;
	ctx->b = t;
}

void shabal_lastBlock(shabal_ctx_t* ctx, const void* block, uint16_t length_b){
	uint8_t i,j;
	uint32_t* t;
	uint8_t buffer[64];
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(ctx, block);
		block = (uint8_t*)block + SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	memset(buffer, 0, 64);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b%8);
	
	for(i=0;i<16;++i){
		ctx->b[i] += ((uint32_t*)buffer)[i];
	}
	ctx->a[0] ^= ctx->w.w32[0];
	ctx->a[1] ^= ctx->w.w32[1];
	shabal_p(ctx, buffer);
	t = ctx->c;
	ctx->c = ctx->b;
	ctx->b = t;

	for(j=0; j<3;++j){
		ctx->a[0] ^= ctx->w.w32[0];
		ctx->a[1] ^= ctx->w.w32[1];
		shabal_p(ctx, buffer);
		t = ctx->c;
		ctx->c = ctx->b;
		ctx->b = t;
	}
	 
}

static inline
void shabal_ctx2hash(void* dest, const shabal_ctx_t* ctx, uint16_t outlength_b){
	memcpy(dest, &(ctx->c[16-outlength_b/32]), outlength_b/8);
}

void shabal192_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 192);
}

void shabal224_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 224);
}

void shabal256_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 256);
}

void shabal384_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 384);
}

void shabal512_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 512);
}

void shabal192(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal192_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal192_ctx2hash(dest, &ctx);
}

void shabal224(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal224_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal224_ctx2hash(dest, &ctx);
}

void shabal256(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal256_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal256_ctx2hash(dest, &ctx);
}

void shabal384(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal384_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal384_ctx2hash(dest, &ctx);
}

void shabal512(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal512_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal512_ctx2hash(dest, &ctx);
}