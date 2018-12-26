#include <utility>

#include "SemanticAnalysis.h"

SemanticAnalysis::SemanticAnalysis(std::shared_ptr<const ParserNode> parse_tree, std::shared_ptr<Logger> logger)
    : _parse_tree(std::move(parse_tree)), _logger(std::move(logger)) {}

std::shared_ptr<SymbolScopeNode> SemanticAnalysis::build_symbol_table(std::shared_ptr<SymbolScopeNode> parent,
                                                                      std::shared_ptr<const ParserNode> &sub_tree) {
  std::shared_ptr<SymbolScopeNode> scope = std::make_shared<SymbolScopeNode>(parent, _logger);

  for (auto &node : sub_tree->getChildren()) {
    auto parser_node = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*node));

    switch (parser_node->getParserNodeType()) {
    case ParserNodeType::const_declaration: {
      scope->addChild(const_declaration(parser_node));
      continue;
    }
    case ParserNodeType::type_declaration: {
      scope->addChild(type_declaration(parser_node, parent));
      continue;
    }
    case ParserNodeType::var_declaration: {
      scope->addChild(var_declaration(parser_node));
      continue;
    }
    case ParserNodeType::procedure_declaration: {
      scope->addChild(procedure_declaration(parser_node, parent));
      continue;
    }
    default: {
      continue;
    }
    }
  }
  return scope;
}

std::shared_ptr<NumberNode> SemanticAnalysis::const_declaration(std::shared_ptr<const ParserNode> const_decl) {
  std::string identifier;
  std::shared_ptr<ParserNode> expr;

  for (auto &child : const_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parser_child->getValue();
      continue;
    }
    case ParserNodeType::expression: {
      expr = parser_child;
      continue;
    }
    default: {
      continue;
    }
    }
  }
  int const_expr = eval_expr(expr);
  auto decl = std::make_shared<NumberNode>(identifier, const_expr, DeclarationType::CONST);

  return decl;
}

int SemanticAnalysis::eval_expr(std::shared_ptr<ParserNode> expr) {
  int result = 0;
  for (auto &child : expr->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::simple_expression: {
      result = eval_simple_expr(parser_child);
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

int SemanticAnalysis::eval_simple_expr(std::shared_ptr<ParserNode> simple_expr) {
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
      numbers.push_back(eval_term(p_child));
      continue;
    }
    default:std::string msg = "unexpected token, check your parsing of the tree fool";
      _logger->error(msg);
      return 0;
    }
  }
  return calculate(numbers, ops);
}

int SemanticAnalysis::eval_term(std::shared_ptr<ParserNode> term) {
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
      numbers.push_back(eval_factor(p_child));
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

int SemanticAnalysis::eval_factor(std::shared_ptr<ParserNode> factor) {
  for (auto &child : factor->getChildren()) {
    auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (p_child->getParserNodeType()) {
    case ParserNodeType::binary_op: {
      std::string msg = "~ not allowed in constant declarations right hand side";
      _logger->error(msg);
      return 0;
    }
    case ParserNodeType::number: {
      return std::stoi(p_child->getValue());
    }
    case ParserNodeType::expression: {
      return eval_expr(p_child);
    }
    case ParserNodeType::identifier: {
      look_up(p_child->getValue(), this->_symbol_tree);
    }
    default:
      std::string msg = "unexpected token, check your parsing of the tree fool";
      _logger->error(msg);
      return 0;
    }
  }
  return 0;
}

std::shared_ptr<TypeNode> SemanticAnalysis::type_declaration(std::shared_ptr<const ParserNode> type_decl, std::shared_ptr<SymbolScopeNode> parent) {
  std::string identifier;
  std::shared_ptr<Node> type;

  for (auto &child : type_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parser_child->getValue();
      continue;
    }
    case ParserNodeType::type: {
      type = parse_type(parser_child, parent);
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
  auto decl = std::make_shared<TypeNode>(identifier, type);
  return decl;
}

std::shared_ptr<Node> SemanticAnalysis::parse_type(std::shared_ptr<ParserNode> type, std::shared_ptr<SymbolScopeNode> parent) {
    auto pt_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*type->getChildren()[0]));
    std::shared_ptr<DeclarationNode> node;

    switch (pt_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      node = look_up(pt_child->getValue(), std::move(parent));
      if (!node) {
        std::string msg = "no declaration found with such a name" + pt_child->getValue();
        _logger->error(msg);
        return nullptr;
      }
      break;
    }
    case ParserNodeType::array_type: {
      node = parse_array(pt_child);
      break;
    }
    case ParserNodeType::record_type: {
      node = parse_record(pt_child, parent);
      break;
    }
    default:return nullptr; //unreachable
    }
  return node;
}

std::shared_ptr<DeclarationNode> SemanticAnalysis::look_up(std::string identifier, std::shared_ptr<SymbolScopeNode> scope) {

}

std::shared_ptr<ArrayNode> SemanticAnalysis::parse_array(std::shared_ptr<ParserNode> array_type) {

}

std::shared_ptr<RecordNode> SemanticAnalysis::parse_record(std::shared_ptr<ParserNode> record_type, std::shared_ptr<SymbolScopeNode> scope) {

}


std::shared_ptr<DeclarationNode> SemanticAnalysis::var_declaration(std::shared_ptr<const ParserNode> var_decl) {
  std::string identifier;
  std::shared_ptr<ParserNode> var;

  for (auto &child : var_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parser_child->getValue();
    }
    case ParserNodeType::type: {
      var = parser_child;
    }
    default: {
      continue;
    }
    }
  }
  auto decl = std::make_shared<DeclarationNode>(identifier, DeclarationType::VAR);
  decl->addChild(var);
  return decl;
}

std::shared_ptr<ProcedureNode> SemanticAnalysis::procedure_declaration(std::shared_ptr<const ParserNode> proc_decl, std::shared_ptr<SymbolScopeNode> parent) {
  std::string identifier;
  std::shared_ptr<ParserNode> proc_h;
  std::shared_ptr<const ParserNode> proc_b;

  for (auto &child : proc_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parser_child->getValue();
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
  auto scope_child = build_symbol_table(std::move(parent), proc_b);
  auto decl = std::make_shared<ProcedureNode>(identifier, params, scope_child);


  return decl;
}

std::vector<std::shared_ptr<Node>> SemanticAnalysis::parse_params(std::shared_ptr<ParserNode> params) {

}

std::shared_ptr<Node> SemanticAnalysis::build_ast() {
  return std::shared_ptr<Node>();
}
std::shared_ptr<Node> SemanticAnalysis::check() {
  // build symbol table, report if a symbol collision occurs
  //
  this->_symbol_tree = build_symbol_table(nullptr, _parse_tree);

  // take ops out of parse tree and link them to symbol table entries or temp results, report on type error
  this->_ast = build_ast();

  // do all other checks

  return this->_ast;
}