/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef MEM_POOL_HPP
#define MEM_POOL_HPP

#include <vector>
#include <map>
#include <list>

typedef unsigned char u8;

struct __sys_align_t
{
	char c;
	size_t sz;
};

static constexpr size_t POOL_SIZE	 = 4 * 1024;
static constexpr size_t ALIGNMENT	 = sizeof( __sys_align_t ) - sizeof( size_t );

struct mem_pool_t
{
	u8 * head;
	u8 * mem;
};

class mem_mgr_t
{
	std::vector< mem_pool_t > m_pools;	
	std::map< size_t, std::list< u8 * > > m_free_chunks;

	void alloc_pool();
public:
	mem_mgr_t();
	~mem_mgr_t();
	void * alloc( size_t sz );
	void free( void * ptr, size_t sz );
};

namespace mem
{
extern mem_mgr_t _memory;

inline void * alloc( size_t sz )
{
	return _memory.alloc( sz );
}

inline void free( void * ptr, size_t sz )
{
	return _memory.free( ptr, sz );
}
}

#endif // MEM_POOL_HPP