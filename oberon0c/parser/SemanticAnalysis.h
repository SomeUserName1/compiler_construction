/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * 1. Symbols
 *    1.1 Modules, Procedures, Records define new scopes ### check
 * 2. Types
 *    2.1 evaluate constant expressions (CONST, ARRAY indexes) ### check
 *    2.6 selector must be on array or record. if array check boundaries, if record check existence of field ### check
 *    2.8 assignment to an array must be of same type as array ### check
 *
 *    2.2 VAR declaration type exists and assignment lhs compatible rhs (lhs is var, lhs and rhs type compat) ### check
 *    2.3 comparison types lhs = rhs and return bool ### check
 *    2.4 boolean op types lhs = rhs and return bool ### check
 *    2.5 Formal param: struct, constant and expr cannot be VAR params, struct passed by ref,
 *      match acutal params on call
 *
 *    2.7 IF & WHILE conditions must be bool ### check
 *
 *
 *
 */

#ifndef OBERON0C_TYPECHECKER_H
#define OBERON0C_TYPECHECKER_H

#include <Node.h>
#include <parse_tree/ParserNode.h>
#include <symbol_table/SymbolScopeNode.h>
#include <symbol_table/NumberNode.h>
#include <symbol_table/TypeNode.h>
#include <symbol_table/ProcedureNode.h>
#include <symbol_table/NumberNode.h>
#include <algorithm>
#include <symbol_table/ArrayNode.h>
#include <symbol_table/RecordNode.h>
#include <ast/CallNode.h>
#include <ast/BranchNode.h>
#include <ast/AssignmentNode.h>
#include <ast/LoopNode.h>
#include <ast/ArithmeticOpNode.h>
#include <ast/ComparatorNode.h>
#include <symbol_table/BooleanNode.h>
#include <ast/ArithmeticOpNode.h>
#include <ast/BooleanOpNode.h>
#include <stdlib.h>

class SemanticAnalysis {
private:
  std::shared_ptr<const ParserNode> _parse_tree;
  std::shared_ptr<SymbolScopeNode> _symbol_tree;
  std::shared_ptr<Node> _ast;
  std::shared_ptr<Logger> _logger;
  std::shared_ptr<SymbolScopeNode> build_symbol_table(std::shared_ptr<SymbolScopeNode> parent,
                                                      std::shared_ptr<const ParserNode> &sub_tree);
  std::shared_ptr<NumberNode> const_declaration(std::shared_ptr<const ParserNode> const_decl,
                                                std::shared_ptr<SymbolScopeNode> current_scope);
  std::vector<std::string> parse_identifier(std::shared_ptr<const ParserNode> identifer,
                                            std::shared_ptr<SymbolScopeNode> current_scope, bool decl);
  int eval_const_expr(std::shared_ptr<ParserNode> expr, std::shared_ptr<SymbolScopeNode> current_scope);
  int eval_const_simple_expr(std::shared_ptr<ParserNode> simple_expr, std::shared_ptr<SymbolScopeNode> current_scope);
  int eval_const_term(std::shared_ptr<ParserNode> term, std::shared_ptr<SymbolScopeNode> current_scope);
  int calculate(std::vector<int> numbers, std::vector<std::string> ops);
  int eval_const_factor(std::shared_ptr<ParserNode> factor, std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<DeclarationNode> type_declaration(std::shared_ptr<const ParserNode> type_decl,
                                                    std::shared_ptr<SymbolScopeNode> parent);
  std::shared_ptr<DeclarationNode> parse_type(std::shared_ptr<ParserNode> type,
                                              bool var,
                                              std::shared_ptr<SymbolScopeNode> parent);
  std::shared_ptr<ArrayNode> parse_array(std::shared_ptr<ParserNode> array_type,
                                         bool var,
                                         std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<RecordNode> parse_record(std::shared_ptr<ParserNode> record_type,
                                           bool var,
                                           std::shared_ptr<SymbolScopeNode> scope);
  std::shared_ptr<DeclarationNode> look_up(std::vector<std::string> identifier, std::shared_ptr<SymbolScopeNode> scope);
  std::vector<std::shared_ptr<DeclarationNode>> var_declaration(std::shared_ptr<const ParserNode> var_decl,
                                                                std::shared_ptr<SymbolScopeNode> scope,
                                                                bool var);
  std::shared_ptr<ProcedureNode> procedure_declaration(std::shared_ptr<const ParserNode> proc_decl,
                                                       std::shared_ptr<SymbolScopeNode> parent);
  std::vector<std::shared_ptr<DeclarationNode>> parse_params(std::shared_ptr<const ParserNode> params,
                                                             std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<Node> build_ast(std::shared_ptr<const ParserNode> statement_sequence,
                                  std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<CallNode> parse_call(std::shared_ptr<const ParserNode> const_decl,
                                       std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<LoopNode> parse_loop(std::shared_ptr<const ParserNode> const_decl,
                                       std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<BranchNode> parse_branch(std::shared_ptr<const ParserNode> const_decl,
                                           std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<AssignmentNode> parse_assignment(std::shared_ptr<const ParserNode> const_decl,
                                                   std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<NumberNode> eval_var_expr(std::shared_ptr<ParserNode> expr,
                                            std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<NumberNode> eval_var_simple_expr(std::shared_ptr<ParserNode> simple_expr,
                                                   std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<NumberNode> eval_var_term(std::shared_ptr<ParserNode> term,
                                            std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<NumberNode> build_arith_op_tree(std::vector<std::shared_ptr<NumberNode>> numbers,
                                                  std::vector<std::string> ops);
  std::shared_ptr<NumberNode> eval_var_factor(std::shared_ptr<ParserNode> factor,
                                              std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<BooleanNode> eval_bool_expr(std::shared_ptr<ParserNode> expr,
                                              std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<NumberNode> eval_bool_simple_expr(std::shared_ptr<ParserNode> simple_expr,
                                                    std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<NumberNode> eval_bool_term(std::shared_ptr<ParserNode> term,
                                             std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<NumberNode> build_bool_op_tree(std::vector<std::shared_ptr<NumberNode>> numbers,
                                                 std::vector<std::string> ops);
  std::shared_ptr<BooleanNode> build_comp_op_tree(std::vector<std::shared_ptr<NumberNode>> numbers,
                                                  std::vector<std::string> ops);
  std::shared_ptr<NumberNode> eval_bool_factor(std::shared_ptr<ParserNode> factor,
                                               std::shared_ptr<SymbolScopeNode> current_scope);
  std::shared_ptr<BooleanNode> negate(std::shared_ptr<NumberNode> node);
  std::shared_ptr<Node> parse_statement_sequence(std::shared_ptr<const ParserNode> sub_tree,
                                                 std::shared_ptr<SymbolScopeNode> current_scope);

public:
  SemanticAnalysis(std::shared_ptr<const ParserNode> parse_tree, std::shared_ptr<Logger> logger);
  std::shared_ptr<Node> check();
};

#endif //OBERON0C_TYPECHECKER_H
