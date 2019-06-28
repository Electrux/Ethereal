/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_ALIASES_HPP
#define VM_ALIASES_HPP

#include <vector>
#include <string>
#include <unordered_map>

class aliases_t
{
	std::unordered_map< std::string, std::vector< std::string > > m_aliases;
public:
	aliases_t();
	~aliases_t();

	const std::vector< std::string > & get( const std::string & alias_name ) const;

	bool exists( const std::string & alias_name ) const;

	void add( const std::string & alias_name, const std::vector< std::string > & alias );

	bool del( const std::string & alias_name );

	size_t count() const;
};

#endif // VM_ALIASES_HPP