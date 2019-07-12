/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"
#include "../Ethereal.hpp"

stmt_expr_t * gen_tree( const src_t & src, parse_helper_t * ph, std::vector< stmt_base_t * > & data );

expr_res_t parse_expr( const src_t & src, parse_helper_t * ph, const int end, const bool is_top )
{
	std::vector< stmt_base_t * > data;
	std::vector< stmt_simple_t * > stack;
	stmt_expr_t * res = nullptr;

	int start = ph->tok_ctr();

	while( end == -1 || ( ph->peak()->type != TOK_INVALID && ph->tok_ctr() < end ) ) {
		if( ph->peak()->type == TOK_COLS ) break;

		if( ph->peak()->type == TOK_IDEN && ( end == -1 || ph->tok_ctr() + 1 < end ) && ph->peak( 1 )->type == TOK_LBRACE ) {
			tok_t * name = ph->peak();
			int tok_val = ph->tok_ctr();
			ph->next();
			int rbrace_loc;
			int err = find_next_of( ph, rbrace_loc, { TOK_RBRACE }, TOK_LBRACE );
			if( err < 0 ) {
				if( err == -1 ) {
					PARSE_FAIL( "could not find the equivalent ending brace for parsing the struct '%s'", name->data.c_str() );
				} else if( err == -2 ) {
					PARSE_FAIL( "found end of statement (semicolon) before the equivalent ending brace for struct object" );
				}
				goto fail;
			}
			ph->next();
			expr_res_t struct_args = parse_expr( src, ph, rbrace_loc, false );
			if( struct_args.res != 0 ) goto fail;
			stmt_func_struct_subscr_call_t * struct_call = new stmt_func_struct_subscr_call_t(
				new stmt_simple_t( SIMPLE_TOKEN, name, tok_val ),
				struct_args.expr, tok_val
			);
			struct_call->m_ctype = CT_STRUCT;
			if( !stack.empty() && stack.back()->m_val->type == TOK_DOT ) struct_call->m_post_dot = true;
			data.push_back( struct_call );
		} else if( ( ph->peak()->type == TOK_IDEN || ph->peak()->type == TOK_STR ) &&
		           ( end == -1 || ph->tok_ctr() + 1 < end ) &&
			   ph->peak( 1 )->type == TOK_LBRACK ) {
			tok_t * name = ph->peak();
			int tok_val = ph->tok_ctr();
			ph->next();
			int rbrack_loc;
			int err = find_next_of( ph, rbrack_loc, { TOK_RBRACK }, TOK_LBRACK );
			if( err < 0 ) {
				if( err == -1 ) {
					PARSE_FAIL( "could not find the equivalent ending bracket for parsing the subscript of '%s'", name->data.c_str() );
				} else if( err == -2 ) {
					PARSE_FAIL( "found end of statement (semicolon) before the equivalent ending bracket for subscript declaration" );
				}
				goto fail;
			}
			ph->next();
			expr_res_t subscr = parse_expr( src, ph, rbrack_loc, false );
			if( subscr.res != 0 ) goto fail;
			stmt_func_struct_subscr_call_t * subscr_call = new stmt_func_struct_subscr_call_t(
				new stmt_simple_t( SIMPLE_TOKEN, name, tok_val ),
				subscr.expr, tok_val
			);
			subscr_call->m_ctype = CT_SUBSCR;
			if( !stack.empty() && stack.back()->m_val->type == TOK_DOT ) subscr_call->m_post_dot = true;
			data.push_back( subscr_call );
		} else if( ph->peak()->type == TOK_IDEN && ( end == -1 || ph->tok_ctr() + 1 < end ) && ph->peak( 1 )->type == TOK_LPAREN ) {
			tok_t * name = ph->peak();
			int tok_val = ph->tok_ctr();
			ph->next();
			int rparen_loc;
			int err = find_next_of( ph, rparen_loc, { TOK_RPAREN }, TOK_LPAREN );
			if( err < 0 ) {
				if( err == -1 ) {
					PARSE_FAIL( "could not find the equivalent ending parentheses for parsing the function call '%s'", name->data.c_str() );
				} else if( err == -2 ) {
					PARSE_FAIL( "found end of statement (semicolon) before the equivalent ending parentheses for function call" );
				}
				goto fail;
			}
			ph->next();
			expr_res_t fn_args = parse_expr( src, ph, rparen_loc, false );
			if( fn_args.res != 0 ) goto fail;
			stmt_func_struct_subscr_call_t * fn_call = new stmt_func_struct_subscr_call_t(
				new stmt_simple_t( SIMPLE_TOKEN, name, tok_val ),
				fn_args.expr, tok_val
			);
			if( !stack.empty() && stack.back()->m_val->type == TOK_DOT ) fn_call->m_post_dot = true;
			data.push_back( fn_call );
		} else if( ph->peak()->type == TOK_LBRACK || ph->peak()->type == TOK_LBRACE ) {
			int tok_val = ph->tok_ctr();
			const TokType type = ph->peak()->type;
			TokType eq = type == TOK_LBRACE ? TOK_RBRACE : TOK_RBRACK;
			int r_loc;
			int err = find_next_of( ph, r_loc, { eq }, type );
			if( err < 0 ) {
				if( err == -1 ) {
					PARSE_FAIL( "could not find the equivalent ending bracket for parsing the map/list" );
				} else if( err == -2 ) {
					PARSE_FAIL( "found end of statement (semicolon) before the equivalent ending bracket for map/list declaration" );
				}
				goto fail;
			}
			ph->next();
			expr_res_t collect = parse_expr( src, ph, r_loc, false );
			if( collect.res != 0 ) goto fail;
			int child_cc = 0, args = 0;
			if( collect.expr ) {
				child_comma_count( collect.expr, child_cc );
				args = child_cc + 1;
			}
			if( type == TOK_LBRACE && args % 2 != 0 ) {
				PARSE_FAIL( "map requires arguments in the multiples of 2 for keys and their values" );
				if( collect.expr ) delete collect.expr;
				goto fail;
			}
			stmt_collection_t * collect_call = new stmt_collection_t(
				collect.expr, tok_val, ph->at( tok_val )->line, ph->at( tok_val )->col
			);
			if( type == TOK_LBRACE ) collect_call->m_is_map = true;
			collect_call->m_arg_count = args;
			data.push_back( collect_call );
		} else {
			if( token_is_data( ph->peak() ) ) {
				data.push_back(
					new stmt_simple_t(
						ph->peak()->data == TokStrs[ ph->peak()->type ] &&
						ph->peak()->type != TOK_IDEN &&
						ph->peak()->type != TOK_STR
							? SIMPLE_KEYWORD : SIMPLE_TOKEN,
						ph->peak(), ph->tok_ctr()
					)
				);
				ph->next();
				continue;
			}
			if( !token_is_oper( ph->peak() ) ) {
				PARSE_FAIL( "invalid token '%s' while parsing expression", ph->peak()->data.c_str() );
				goto fail;
			}
			// handle parentheses
			if( ph->peak()->type == TOK_RPAREN || ph->peak()->type == TOK_RBRACE || ph->peak()->type == TOK_RBRACK ) {
				bool found = false;
				TokType eq;
				if( ph->peak()->type == TOK_RPAREN ) eq = TOK_LPAREN;
				else if( ph->peak()->type == TOK_RBRACE ) eq = TOK_LBRACE;
				else eq = TOK_LBRACK;
				while( stack.size() > 0 ) {
					if( stack.back()->m_val->type == eq ) {
						delete stack.back();
						stack.pop_back();
						found = true;
						break;
					}
					data.push_back( stack.back() );
					stack.pop_back();
				}
				if( !found ) {
					if( eq == TOK_LPAREN ) PARSE_FAIL( "could not find equivalent beginning parentheses" );
					else if( eq == TOK_LBRACE ) PARSE_FAIL( "could not find equivalent beginning brace" );
					else if( eq == TOK_LBRACK ) PARSE_FAIL( "could not find equivalent beginning bracket" );
					goto fail;
				}
				ph->next();
				continue;
			}
			// handle unary +/-
			if( ( ph->peak()->type == TOK_ADD || ph->peak()->type == TOK_SUB ) &&
			    ( ph->tok_ctr() == 0 || ( ph->tok_ctr() > start && token_is_oper( ph->peak( -1 ) ) &&
			      ph->peak( -1 )->type != TOK_RPAREN && ph->peak( -1 )->type != TOK_RBRACE && ph->peak( -1 )->type != TOK_RBRACK ) ) ) {
				if( ph->peak()->type == TOK_SUB ) {
					ph->peak()->type = TOK_USUB;
				}
				if( ph->peak()->type == TOK_ADD ) {
					ph->peak()->type = TOK_UADD;
				}
			}
			// do the actual work
			int prec = oper_prec( ph->peak() );
			if( stack.size() > 0 ) {
				// sop = stack operator
				stmt_simple_t * sop = stack.size() > 0 ? stack.back() : nullptr;
				// sprec = stack_oper_precedence
				int sprec = sop != nullptr ? oper_prec( sop->m_val ) : -10;
				while( sop != nullptr &&
				       sop->m_val->type != TOK_LPAREN && sop->m_val->type != TOK_LBRACE && sop->m_val->type != TOK_LBRACK &&
				       prec < sprec ) {
					data.push_back( sop );
					stack.pop_back();
					sop = stack.size() > 0 ? stack.back() : nullptr;
					sprec = sop != nullptr ? oper_prec( sop->m_val ) : -10;
				}
				if( sop != nullptr &&
				    sop->m_val->type != TOK_LPAREN && sop->m_val->type != TOK_LBRACE && sop->m_val->type != TOK_LBRACK &&
				    prec == sprec ) {
					if( oper_assoc( sop->m_val ) == LTR ) {
						data.push_back( sop );
						stack.pop_back();
					}
				}
			}
			stack.push_back( new stmt_simple_t( SIMPLE_OPER, ph->peak(), ph->tok_ctr() ) );
		}
		ph->next();
	}

	for( auto & s : stack ) {
		if( s->m_val->type == TOK_LPAREN ) {
			ph->set_tok_ctr( s->m_tok_ctr );
			PARSE_FAIL( "could not find ending parentheses" );
			goto fail;
		}
	}

	while( !stack.empty() ) {
		data.push_back( stack.back() );
		stack.pop_back();
	}

	if( data.empty() ) {
		return { 0, nullptr };
	}

	res = gen_tree( src, ph, data );
	if( res == nullptr ) goto fail;

	if( is_top ) {
		res->m_is_top_expr = true;
	}

	return { 0, res };
fail:
	for( auto & d : data ) {
		delete d;
	}
	if( end != -1 ) ph->set_tok_ctr( end );
	return { E_PARSE_FAIL, nullptr };
}

stmt_expr_t * gen_tree( const src_t & src, parse_helper_t * ph, std::vector< stmt_base_t * > & data )
{
	std::vector< stmt_base_t * > var_stack;
	const int start_tok_ctr = data.front()->m_tok_ctr;

	for( auto it = data.begin(); it != data.end(); ) {
		if( ( * it )->m_type != GRAM_SIMPLE || ( ( stmt_simple_t * )( * it ) )->m_stype != SIMPLE_OPER ) {
			var_stack.push_back( * it );
			it = data.erase( it );
			continue;
		}

		const tok_t * op = ( ( stmt_simple_t * )( * it ) )->m_val;
		if( var_stack.empty() ) {
			ph->set_tok_ctr( ( * it )->m_tok_ctr );
			PARSE_FAIL( "no operands for operator '%s'", TokStrs[ op->type ] );
			goto fail;
		}

		int arg_count = oper_arg_count( op );
		if( arg_count < 0 ) {
			ph->set_tok_ctr( ( * it )->m_tok_ctr );
			PARSE_FAIL( "the operator '%s' should not be here", TokStrs[ op->type ] );
			goto fail;
		}

		if( ( int )var_stack.size() < arg_count ) {
			int available = 0;
			for( auto it = var_stack.rbegin(); it != var_stack.rend(); ++it ) {
				if( ( ( stmt_simple_t * )( * it ) )->m_stype == SIMPLE_OPER ) break;
				++available;
			}
			ph->set_tok_ctr( ( * it )->m_tok_ctr );
			PARSE_FAIL( "not enough arguments for operator '%s' (expected: %d, available: %d)", TokStrs[ op->type ], arg_count, available );
			goto fail;
		}

		stmt_base_t * top2 = nullptr;
		if( arg_count >= 1 ) {
			top2 = var_stack.back();
			var_stack.pop_back();
		}

		stmt_base_t * top1 = nullptr;
		if( arg_count >= 2 ) {
			top1 = var_stack.back();
			var_stack.pop_back();
		}

		/*	oper
			/  \
		      top1 top2
		*/
		if( token_is_one_of_assign( op ) &&
		    ( ( top1 == nullptr || ( ( top1->m_type == GRAM_SIMPLE &&
		    ( ( stmt_simple_t * )top1 )->m_val->type != TOK_IDEN ) && ( ( stmt_simple_t * )top1 )->m_val->type != TOK_SELF &&
		    top1->m_type != GRAM_EXPR ) ) ) ) {
			ph->set_tok_ctr( ( * it )->m_tok_ctr );
			PARSE_FAIL( "expected an lvalue on the left of the assignment operator" );
			if( arg_count >= 1 ) delete top2;
			if( arg_count >= 2 ) delete top1;
			goto fail;
		}

		stmt_expr_t * expr = new stmt_expr_t( top1, ( stmt_simple_t * ) * it, top2, ( * it )->m_tok_ctr );
		it = data.erase( it );
		var_stack.push_back( expr );
	}

	if( var_stack.empty() || var_stack.size() > 1 ) {
		ph->set_tok_ctr( start_tok_ctr );
		PARSE_FAIL( "invalid expression, should generate single value, but is generating %zu", var_stack.size() );
		goto fail;
	}

	if( var_stack.back()->m_type != GRAM_EXPR ) {
		stmt_base_t * data = var_stack.back();
		var_stack.pop_back();
		stmt_expr_t * expr = new stmt_expr_t( nullptr, nullptr, data, data->m_tok_ctr );
		var_stack.push_back( expr );
	}

	return ( stmt_expr_t * )var_stack.back();

fail:
	for( auto & vs : var_stack ) delete vs;
	return nullptr;
}

bool stmt_expr_t::bytecode( src_t & src ) const
{
	if( !m_oper ) {
		if( m_rhs ) m_rhs->bytecode( src );
		if( m_lhs ) m_lhs->bytecode( src );
	} else {
		const tok_t * oper = m_oper->m_val;

		if( oper_assoc( oper ) == LTR ) {
			int ques_col_loc = -1;

			if( m_lhs ) m_lhs->bytecode( src );

			if( oper->type == TOK_QUEST ) {
				src.bcode.push_back( { m_oper->m_tok_ctr, oper->line, oper->col, IC_JUMP_FALSE, { OP_INT, "<placeholder>" } } );
				ques_col_loc = src.bcode.size() - 1;
			}
			if( oper->type == TOK_COL ) {
				src.bcode.push_back( { m_oper->m_tok_ctr, oper->line, oper->col, IC_JUMP, { OP_INT, "<placeholder>" } } );
				ques_col_loc = src.bcode.size() - 1;
			}

			int curr_bcode_size = src.bcode.size();
			if( m_rhs ) m_rhs->bytecode( src );

			if( oper->type == TOK_QUEST || oper->type == TOK_COL ) {
				int rhs_bcode_size = src.bcode.size() - curr_bcode_size + ( oper->type == TOK_QUEST );
				src.bcode[ ques_col_loc ].oper.val = std::to_string( ques_col_loc + rhs_bcode_size + 1 );
			}
		} else {
			if( m_rhs ) m_rhs->bytecode( src );
			if( m_lhs ) m_lhs->bytecode( src );
		}
	}

	if( !m_oper ) return true;

	const TokType ttype = m_oper ? m_oper->m_val->type : TOK_INVALID;

	int line = m_oper ? m_oper->m_val->line : src.toks[ m_tok_ctr ].line;
	int col = m_oper ? m_oper->m_val->col : src.toks[ m_tok_ctr ].col;

	if( ttype == TOK_COMMA || ttype == TOK_QUEST || ttype == TOK_COL ) return true;

	if( ttype == TOK_DOT ) {
		if( src.bcode.size() > 0 && ( src.bcode.back().opcode == IC_FN_MEM_CALL || src.bcode.back().opcode == IC_FN_MEM_CALL || src.bcode.back().opcode == IC_STRUCT_MEM_DECL ) ) {
			goto end;
		}
	}

	if( ttype == TOK_ASSN ) {
		int child_cc = 0;
		child_comma_count( this, child_cc );
		if( child_cc == 0 ) src.bcode.push_back( { m_oper->m_tok_ctr, line, col,
						       m_is_top_expr ? IC_STORE : IC_STORE_LOAD, { OP_NONE, "" } } );
		else src.bcode.push_back( { m_oper->m_tok_ctr, line, col,
					m_is_top_expr ? IC_STORE : IC_STORE_LOAD, { OP_INT, std::to_string( child_cc / 2 + 1 ) } } );
		return true;
	}

	m_oper->bytecode( src );
	src.bcode.push_back( { m_oper->m_tok_ctr, line, col, IC_FN_CALL,
			   { OP_INT, std::to_string( oper_arg_count( m_oper->m_val ) ) } } );

end:
	if( m_is_top_expr ) {
		src.bcode.push_back( { m_tok_ctr, line, col, IC_POP, { OP_NONE, "" } } );
	}

	return true;
}

void child_comma_count( const stmt_expr_t * expr, int & cc )
{
	if( expr->m_lhs && expr->m_lhs->m_type == GRAM_EXPR ) {
		const stmt_expr_t * e = ( const stmt_expr_t * )expr->m_lhs;
		if( e->m_oper != nullptr && e->m_oper->m_val->type == TOK_COMMA ) child_comma_count( e, cc );
	}
	if( expr->m_rhs && expr->m_rhs->m_type == GRAM_EXPR ) {
		const stmt_expr_t * e = ( const stmt_expr_t * )expr->m_rhs;
		if( e->m_oper != nullptr && e->m_oper->m_val->type == TOK_COMMA ) child_comma_count( e, cc );
	}
	if( expr->m_oper && expr->m_oper->m_val->type == TOK_COMMA ) ++cc;
}