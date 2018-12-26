/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * 2. Types
 *    2.1 evaluate constant expressions (CONST, ARRAY indizes)
 *    2.2 VAR declaration type exists and assignment lhs var and types lhs compatible rhs
 *    2.3 comparison types lhs = rhs and return bool
 *    2.4 boolean op types lhs = rhs and return bool
 *    2.5 Formal param: structured, constant and expr cannot be VAR params, structured passed by ref,
 *      match acutal params on call
 *    2.6 selector must be on array or record. if array check boundaries, if record check existence of field
 *    2.7 IF & WHILE conditions must be bool
 *    2.8 assignment to an array must be of same type as array
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

class SemanticAnalysis {
private:
  std::shared_ptr<const ParserNode> _parse_tree;
  std::shared_ptr<SymbolScopeNode> _symbol_tree;
  std::shared_ptr<Node> _ast;
  std::shared_ptr<Logger> _logger;
public:
  explicit SemanticAnalysis(std::shared_ptr<const ParserNode> parse_tree, std::shared_ptr<Logger> logger);
  std::shared_ptr<Node> check();
  std::shared_ptr<SymbolScopeNode> build_symbol_table(std::shared_ptr<SymbolScopeNode> parent, std::shared_ptr<const ParserNode> &sub_tree);
  std::shared_ptr<NumberNode> const_declaration(std::shared_ptr<const ParserNode> const_decl);
  int eval_expr(std::shared_ptr<ParserNode> expr);
  int eval_simple_expr(std::shared_ptr<ParserNode> simple_expr);
  int eval_term(std::shared_ptr<ParserNode> term);
  int calculate(std::vector<int> numbers, std::vector<std::string> ops);
  int eval_factor(std::shared_ptr<ParserNode> factor);
  std::shared_ptr<TypeNode> type_declaration(std::shared_ptr<const ParserNode> type_decl, std::shared_ptr<SymbolScopeNode> parent);
  std::shared_ptr<Node> parse_type(std::shared_ptr<ParserNode> type, std::shared_ptr<SymbolScopeNode> parent);
  std::shared_ptr<ArrayNode> parse_array(std::shared_ptr<ParserNode> array_type);
  std::shared_ptr<RecordNode> parse_record(std::shared_ptr<ParserNode> record_type, std::shared_ptr<SymbolScopeNode> scope);
  std::shared_ptr<DeclarationNode> look_up(std::string identifier, std::shared_ptr<SymbolScopeNode> scope);
  std::shared_ptr<DeclarationNode> var_declaration(std::shared_ptr<const ParserNode> var_decl);
  std::shared_ptr<ProcedureNode> procedure_declaration(std::shared_ptr<const ParserNode> proc_decl, std::shared_ptr<SymbolScopeNode> parent);
  std::vector<std::shared_ptr<Node>> parse_params(std::shared_ptr<ParserNode> params);
  std::shared_ptr<Node> build_ast();
};


#endif //OBERON0C_TYPECHECKER_H
