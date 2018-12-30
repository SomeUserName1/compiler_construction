#include <utility>

#include "SemanticAnalysis.h"

SemanticAnalysis::SemanticAnalysis(std::shared_ptr<const ParserNode> parse_tree, std::shared_ptr<Logger> logger)
    : _parse_tree(std::move(parse_tree)), _logger(std::move(logger)) {}

std::shared_ptr<Node> SemanticAnalysis::check() {
  // build symbol table, report if a symbol collision occurs
  this->_symbol_tree = build_symbol_table(nullptr, _parse_tree);

  // take ops out of parse tree and link them to symbol table entries or temp results, report on type error
  this->_ast = build_ast(_parse_tree);

  // do all other checks

  return this->_ast;
}

std::shared_ptr<SymbolScopeNode> SemanticAnalysis::build_symbol_table(std::shared_ptr<SymbolScopeNode> current_scope,
                                                                      std::shared_ptr<const ParserNode> &sub_tree) {
  std::shared_ptr<SymbolScopeNode> scope = std::make_shared<SymbolScopeNode>(current_scope, _logger);

  for (auto &node : sub_tree->getChildren()) {
    auto parser_node = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*node));

    switch (parser_node->getParserNodeType()) {
    case ParserNodeType::const_declaration: {
      scope->addChild(const_declaration(parser_node, scope));
      continue;
    }
    case ParserNodeType::type_declaration: {
      scope->addChild(type_declaration(parser_node, scope));
      continue;
    }
    case ParserNodeType::var_declaration: {
      scope->addChild(var_declaration(parser_node, scope));
      continue;
    }
    case ParserNodeType::procedure_declaration: {
      scope->addChild(procedure_declaration(parser_node, scope));
      continue;
    }
    default: {
      continue;
    }
    }
  }
  return scope;
}

std::shared_ptr<Node> SemanticAnalysis::build_ast(std::shared_ptr<const ParserNode> sub_tree) {
  // TODO
  // 1. find statement sequence
  // 2. parse statement sequence
  // 3. do additional type checks, should be implicit in AST Data structures
  return std::shared_ptr<Node>();
}


std::shared_ptr<NumberNode> SemanticAnalysis::const_declaration(std::shared_ptr<const ParserNode> const_decl,
    std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::string> identifier;
  int expr;

  for (auto &child : const_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parse_identifier(parser_child, current_scope);
      if (identifier.size() > 1) {
        std::string s = "Const declaration identifiers must not be within arrays or records";
        _logger->error(s);
      }
      continue;
    }
    case ParserNodeType::expression: {
      expr = eval_expr(parser_child, std::move(current_scope));
      continue;
    }
    default: {
      continue;
    }
    }
  }
  auto decl = std::make_shared<NumberNode>(identifier[0], expr, DeclarationType::CONST);

  return decl;
}



std::shared_ptr<TypeNode> SemanticAnalysis::type_declaration(std::shared_ptr<const ParserNode> type_decl, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::string> identifier;
  std::shared_ptr<Node> type;

  for (auto &child : type_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parse_identifier(parser_child, current_scope);
      if (identifier.size() > 1) {
        std::string s = "Const declaration identifiers must not be within arrays or records";
        _logger->error(s);
      }
      continue;
    }
    case ParserNodeType::type: {
      type = parse_type(parser_child, false, current_scope);
      if (!type) {
        return nullptr;
      }
      continue;
    }
    default: {
      continue;
    }
    }
  }
  auto decl = std::make_shared<TypeNode>(identifier[0], type, DeclarationType::TYPE);
  return decl;
}

std::shared_ptr<DeclarationNode> SemanticAnalysis::var_declaration(std::shared_ptr<const ParserNode> var_decl, std::shared_ptr<SymbolScopeNode> scope) {
  std::vector<std::string> identifier;
  std::shared_ptr<Node> var;

  for (auto &child : var_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parse_identifier(parser_child, scope);
      if (identifier.size() > 1) {
        std::string s = "Const declaration identifiers must not be within arrays or records";
        _logger->error(s);
      }
    }
    case ParserNodeType::type: {
      // TODO check and create propper node; either Array, record, type or numbernode; assing name
      var = parse_type(parser_child, true, scope);
    }
    default: {
      continue;
    }
    }
  }
  // TODO rework
  auto decl = std::make_shared<DeclarationNode>(identifier[0], DeclarationType::VAR);
  decl->addChild(var);
  return decl;
}

std::shared_ptr<ProcedureNode> SemanticAnalysis::procedure_declaration(std::shared_ptr<const ParserNode> proc_decl, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::string> identifier;
  std::shared_ptr<ParserNode> proc_h;
  std::shared_ptr<const ParserNode> proc_b;

  for (auto &child : proc_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parse_identifier(parser_child, current_scope);
      if (identifier.size() > 1) {
        std::string s = "Const declaration identifiers must not be within arrays or records";
        _logger->error(s);
      }
    }
    case ParserNodeType::procedure_heading: {
      proc_h = parser_child;
    }
    case ParserNodeType::procedure_body: {
      proc_b = parser_child;
    }
    default: {
      continue;
    }
    }
  }
  auto params = parse_params(proc_h);
  auto scope_child = build_symbol_table(std::move(current_scope), proc_b);
  auto proc_ast = build_ast(proc_b);
  auto decl = std::make_shared<ProcedureNode>(identifier[0], params, scope_child);

  return decl;
}

std::shared_ptr<DeclarationNode> SemanticAnalysis::look_up(std::vector<std::string> identifier, std::shared_ptr<SymbolScopeNode> scope) {
  // find declaration (node) for identifier
  auto ident = identifier[0];

  std::shared_ptr<DeclarationNode> node;
  for (auto &decl_ : scope->getChildren()) {
    auto decl = std::make_shared<DeclarationNode>(dynamic_cast<DeclarationNode &>(*decl_));
    if (decl->getName() == ident) {
      node = decl;
    }
  }
  if (!node) {
    if (scope == this->_symbol_tree) {
      if (identifier.size() > 1) {
        std::string s = "Couldn't find symbol" + ident + "did you declare it?";
        _logger->error(s);
      }
    } else {
      return look_up(identifier, scope->getContainingScope());
    }
  }

  for (int i = 1; i < identifier.size(); i++) {

    if (identifier[i] == ".") {
      // case . : look for field in record
      auto record = std::make_shared<RecordNode>(dynamic_cast<RecordNode &>(*node));
      if (!record) throw "unexpected type when looking up" + ident + "expected record type due to the . selector";
      node = record->getField(identifier[i+1]);
      i++;
    } else if (identifier[i] == "[") {
      // case [ : look for array including size
      auto array = std::make_shared<ArrayNode>(dynamic_cast<ArrayNode &>(*node));
      if (!array) throw "unexpected type when looking up" + ident + "expected array type due to the [] selector";
      node = array->getValue(std::stoi(identifier[i+1]));
      i = i+2;
    }
  }
  return node;
}

std::shared_ptr<ArrayNode> SemanticAnalysis::parse_array(std::shared_ptr<ParserNode> array_type) {
  // TODO

}

std::shared_ptr<RecordNode> SemanticAnalysis::parse_record(std::shared_ptr<ParserNode> record_type, std::shared_ptr<SymbolScopeNode> scope) {
  // TODO
}

std::vector<std::shared_ptr<Node>> SemanticAnalysis::parse_params(std::shared_ptr<ParserNode> params) {
  // TODO
  return std::vector<std::shared_ptr<Node>>();
}

std::shared_ptr<Node> SemanticAnalysis::parse_type(std::shared_ptr<ParserNode> type_, bool var, std::shared_ptr<SymbolScopeNode> current_scope) {
  auto pt_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*type_->getChildren()[0]));
  std::shared_ptr<DeclarationNode> node;

  switch (pt_child->getParserNodeType()) {
  case ParserNodeType::identifier: {
    if (pt_child->getValue() == "INTEGER") {
      return var ? std::make_shared<NumberNode>("TOBESET", -999999, DeclarationType::VAR)
          : std::make_shared<NumberNode>("TYPE", 0, DeclarationType::TYPE);
    }
    node = look_up(parse_identifier(pt_child, std::move(current_scope)), std::move(current_scope));
    auto type = std::make_shared<TypeNode>(dynamic_cast<TypeNode &>(*node));
    if (!type)  {
      std::string msg = "no type declaration found with such a name" + pt_child->getValue();
      _logger->error(msg);
      return nullptr;
    }
    break;
  }
  case ParserNodeType::array_type: {
    // TODO remember to set var or type
    node = parse_array(pt_child);
    break;
  }
  case ParserNodeType::record_type: {
    // TODO remember to set var or type
    node = parse_record(pt_child, current_scope);
    break;
  }
  default:return nullptr; //unreachable
  }
  return node;
}

std::vector<std::string> SemanticAnalysis::parse_identifier(std::shared_ptr<ParserNode> identifer,
                                                            std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::string> result({identifer->getValue()});

  // End identifier
  if (identifer->getChildren().empty())
    return result;

  for (auto &selector : identifer->getChildren()) {
    auto tokens = selector->getChildren();
    for (int i = 0; i < tokens.size(); i++) {
      auto token = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*tokens[i]));
      if (token->getValue().empty()) {
        return result;
      } else if (token->getValue() == ".") {
        result.push_back(token->getValue());
        auto id = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*tokens[i+1]));
        result.push_back(id->getValue());
        i++;
      } else if (token->getValue() == "[") {
        result.push_back(token->getValue());
        auto expr = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*tokens[i+1]));
        result.push_back(to_string(eval_expr(expr, current_scope)));
        result.emplace_back("]");
        i = i+2;
      }
    }
  }
  return result;
}

int SemanticAnalysis::eval_expr(std::shared_ptr<ParserNode> expr, std::shared_ptr<SymbolScopeNode> current_scope) {
  int result = 0;
  for (auto &child : expr->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::simple_expression: {
      result = eval_simple_expr(parser_child, current_scope);
      continue;
    }
    case ParserNodeType::binary_op:
    default: {
      std::string s = "comparators are not allowed on the rhs of const declarations";
      _logger->error(s);
      return result;
    }
    }
  }
  return result;
}

int SemanticAnalysis::eval_simple_expr(std::shared_ptr<ParserNode> simple_expr, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<int> numbers;
  std::vector<std::string> ops;
  for (auto &child : simple_expr->getChildren()) {
    auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (p_child->getParserNodeType()) {
    case ParserNodeType::binary_op: {
      if (p_child->getValue() == "OR") {
        std::string msg = "OR not allowed in constant declarations right hand side";
        _logger->error(msg);
        return 0;
      }
      ops.push_back(p_child->getValue());
      continue;
    }
    case ParserNodeType::term: {
      numbers.push_back(eval_term(p_child, current_scope));
      continue;
    }
    default:std::string msg = "unexpected token, check your parsing of the tree fool";
      _logger->error(msg);
      return 0;
    }
  }
  return calculate(numbers, ops);
}

int SemanticAnalysis::eval_term(std::shared_ptr<ParserNode> term, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<int> numbers;
  std::vector<std::string> ops;
  for (auto &child : term->getChildren()) {
    auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (p_child->getParserNodeType()) {
    case ParserNodeType::binary_op: {
      if (p_child->getValue() == "AND") {
        std::string msg = "AND not allowed in constant declarations right hand side";
        _logger->error(msg);
        return 0;
      }
      ops.push_back(p_child->getValue());
      continue;
    }
    case ParserNodeType::factor: {
      numbers.push_back(eval_factor(p_child, current_scope));
      continue;
    }
    default:std::string msg = "unexpected token, check your parsing of the tree fool";
      _logger->error(msg);
      return 0;
    }
  }

  return calculate(numbers, ops);
}

int SemanticAnalysis::calculate(std::vector<int> numbers, std::vector<std::string> ops) {
  std::reverse(numbers.begin(), numbers.end());
  int result = 0;
  if (ops.size() < numbers.size()) {
    // no leading signum
    result = numbers.back();
    numbers.pop_back();
  }
  for (const auto &op : ops) {
    // Why no fucking switch for strings without another enum cpp, why() ??!??! newStandard() ??!??! throw stupid_lang_designers_exception
    if (op == "-") {
      result = result - numbers.back();
      numbers.pop_back();
    } else if (op == "+"){
      result = result + numbers.back();
      numbers.pop_back();
    } else if (op == "*"){
      result = result * numbers.back();
      numbers.pop_back();
    } else if (op == "/"){
      result = result / numbers.back();
      numbers.pop_back();
    } else if (op == "%"){
      result = result % numbers.back();
      numbers.pop_back();
    } else {
      std::string msg = "Wrong operator in const declaration rhs, check your code mate";
      _logger->error(msg);
    }
  }
  return result;
}

int SemanticAnalysis::eval_factor(std::shared_ptr<ParserNode> factor, std::shared_ptr<SymbolScopeNode> current_scope) {
  auto &child = factor->getChildren()[0];
  auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

  switch (p_child->getParserNodeType()) {
  case ParserNodeType::binary_op: {
    std::string msg = "~ not allowed in constant declarations right hand side";
    _logger->error(msg);
    return -999999;
  }
  case ParserNodeType::number: {
    return std::stoi(p_child->getValue());
  }
  case ParserNodeType::expression: {
    return eval_expr(p_child, std::move(current_scope));
  }
  case ParserNodeType::identifier: {
    auto declaration_node = look_up(parse_identifier(p_child, current_scope), current_scope);
    auto number = std::make_shared<NumberNode>(dynamic_cast<NumberNode &>(*declaration_node));
    if (!number)  {
      std::string msg = "no type declaration found with such a name" + p_child->getValue();
      _logger->error(msg);
      return -99999999;
    }
    return number->getValue();
  }
  default:
    std::string msg = "unexpected token, check your parsing of the tree fool";
    _logger->error(msg);
    return -9999999;
  }
}