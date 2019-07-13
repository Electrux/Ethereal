
#include "VMStack.hpp"

vm_stack_t::vm_stack_t() {}

vm_stack_t::~vm_stack_t()
{
	for( auto & val : m_vec ) {
		VAR_DREF( val );
	}
}