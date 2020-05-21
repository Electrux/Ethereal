/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "MemPool.hpp"

static inline size_t nearest_mult8( size_t sz )
{
	if( sz > 512 ) return sz;
	else if( sz >=   1 && sz <=   8 ) return   8;
	else if( sz >=   9 && sz <=  16 ) return  16;
	else if( sz >=  17 && sz <=  24 ) return  24;
	else if( sz >=  25 && sz <=  32 ) return  32;
	else if( sz >=  33 && sz <=  40 ) return  40;
	else if( sz >=  41 && sz <=  48 ) return  48;
	else if( sz >=  49 && sz <=  56 ) return  56;
	else if( sz >=  57 && sz <=  64 ) return  64;
	else if( sz >=  65 && sz <=  72 ) return  72;
	else if( sz >=  73 && sz <=  80 ) return  80;
	else if( sz >=  81 && sz <=  88 ) return  88;
	else if( sz >=  89 && sz <=  96 ) return  96;
	else if( sz >=  97 && sz <= 104 ) return 104;
	else if( sz >= 105 && sz <= 112 ) return 112;
	else if( sz >= 113 && sz <= 120 ) return 120;
	else if( sz >= 121 && sz <= 128 ) return 128;
	else if( sz >= 129 && sz <= 136 ) return 136;
	else if( sz >= 137 && sz <= 144 ) return 144;
	else if( sz >= 145 && sz <= 152 ) return 152;
	else if( sz >= 153 && sz <= 160 ) return 160;
	else if( sz >= 161 && sz <= 168 ) return 168;
	else if( sz >= 169 && sz <= 176 ) return 176;
	else if( sz >= 177 && sz <= 184 ) return 184;
	else if( sz >= 185 && sz <= 192 ) return 192;
	else if( sz >= 193 && sz <= 200 ) return 200;
	else if( sz >= 201 && sz <= 208 ) return 208;
	else if( sz >= 209 && sz <= 216 ) return 216;
	else if( sz >= 217 && sz <= 224 ) return 224;
	else if( sz >= 225 && sz <= 232 ) return 232;
	else if( sz >= 233 && sz <= 240 ) return 240;
	else if( sz >= 241 && sz <= 248 ) return 248;
	else if( sz >= 249 && sz <= 256 ) return 256;
	else if( sz >= 257 && sz <= 264 ) return 264;
	else if( sz >= 265 && sz <= 272 ) return 272;
	else if( sz >= 273 && sz <= 280 ) return 280;
	else if( sz >= 281 && sz <= 288 ) return 288;
	else if( sz >= 289 && sz <= 296 ) return 296;
	else if( sz >= 297 && sz <= 304 ) return 304;
	else if( sz >= 305 && sz <= 312 ) return 312;
	else if( sz >= 313 && sz <= 320 ) return 320;
	else if( sz >= 321 && sz <= 328 ) return 328;
	else if( sz >= 329 && sz <= 336 ) return 336;
	else if( sz >= 337 && sz <= 344 ) return 344;
	else if( sz >= 345 && sz <= 352 ) return 352;
	else if( sz >= 353 && sz <= 360 ) return 360;
	else if( sz >= 361 && sz <= 368 ) return 368;
	else if( sz >= 369 && sz <= 376 ) return 376;
	else if( sz >= 377 && sz <= 384 ) return 384;
	else if( sz >= 385 && sz <= 392 ) return 392;
	else if( sz >= 393 && sz <= 400 ) return 400;
	else if( sz >= 401 && sz <= 408 ) return 408;
	else if( sz >= 409 && sz <= 416 ) return 416;
	else if( sz >= 417 && sz <= 424 ) return 424;
	else if( sz >= 425 && sz <= 432 ) return 432;
	else if( sz >= 433 && sz <= 440 ) return 440;
	else if( sz >= 441 && sz <= 448 ) return 448;
	else if( sz >= 449 && sz <= 456 ) return 456;
	else if( sz >= 457 && sz <= 464 ) return 464;
	else if( sz >= 465 && sz <= 472 ) return 472;
	else if( sz >= 473 && sz <= 480 ) return 480;
	else if( sz >= 481 && sz <= 488 ) return 488;
	else if( sz >= 489 && sz <= 496 ) return 496;
	else if( sz >= 497 && sz <= 504 ) return 504;
	else if( sz >= 505 && sz <= 512 ) return 512;
	return sz;
}

mem_mgr_t mem::_memory;
//static size_t tot_alloc = 0;
//static size_t tot_alloc_direct = 0;
//static size_t tot_alloc_req = 0;
void mem_mgr_t::alloc_pool()
{
	u8 * alloc = new u8[ POOL_SIZE ];
	//tot_alloc += POOL_SIZE;
	m_pools.push_back( { alloc, alloc } );
}

mem_mgr_t::mem_mgr_t() { alloc_pool(); }
mem_mgr_t::~mem_mgr_t()
{
	for( auto & c : m_free_chunks ) {
		if( c.first > POOL_SIZE ) {
			for( auto & blk : c.second ) {
				delete[] blk;
			}
		}
		c.second.clear();
	}
	m_free_chunks.clear();
	for( auto & p : m_pools ) {
		delete[] p.mem;
	}
	//fprintf( stdout, "Total allocated: %zu, directly: %zu, requests: %zu\n", tot_alloc, tot_alloc_direct, tot_alloc_req );
}

void * mem_mgr_t::alloc( size_t sz )
{
	if( sz == 0 ) return nullptr;
	//tot_alloc_direct += sz;
	//++tot_alloc_req;

	sz = nearest_mult8( sz );

	if( sz > POOL_SIZE ) {
		//fprintf( stdout, "Allocating manually ... %zu\n", sz );
		return new u8[ sz ];
	}

	if( m_free_chunks[ sz ].size() == 0 ) {
		for( auto & p : m_pools ) {
			size_t free_space = POOL_SIZE - ( p.head - p.mem );
			if( free_space >= sz ) {
				u8 * loc = p.head;
				p.head += sz;
				//fprintf( stdout, "Allocating from pool ... %zu\n", sz );
				return loc;
			}
		}
		alloc_pool();
		auto & p = m_pools.back();
		u8 * loc = p.head;
		p.head += sz;
		//fprintf( stdout, "Allocating from NEW pool ... %zu\n", sz );
		return loc;
	}

	u8 * loc = m_free_chunks[ sz ].front();
	m_free_chunks[ sz ].pop_front();
	//fprintf( stdout, "Using previously allocated ... %zu\n", sz );
	return loc;
}

void mem_mgr_t::free( void * ptr, size_t sz )
{
	if( ptr == nullptr || sz == 0 ) return;
	//fprintf( stdout, "Giving back to pool ... %zu\n", sz );
	m_free_chunks[ sz ].push_front( ( u8 * )ptr );
}
