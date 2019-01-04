#include <ast/ArithmeticOpNode.h>
#include "SemanticAnalysis.h"

SemanticAnalysis::SemanticAnalysis(std::shared_ptr<const ParserNode> parse_tree, std::shared_ptr<Logger> logger)
    : _parse_tree(std::move(parse_tree)), _logger(std::move(logger)) {}

std::shared_ptr<Node> SemanticAnalysis::check() {
//  std::cout << "\n\n\n\n \t\t\tDISCLAIMER: \n"
//               "\tThe following code is designed in an extremely ill-formed, if not systematically chaotic way \n"
//               "\tThis leads especially to the overuse of for loops, switch statements and worst and foremost dynamic casts\n"
//               "\tThe author obviously didnt proppely inform oneself about inheritance in Cpp thus tought polymorphism works\n"
//               "\tsimilar to javas but that is obviously not the case \n"
//               "BE CAREFUL WHEN YOU READ THE FOLLOWING, IT MIGHT DYNAMICALLY CAST YOUR BRAIN AT READ TIME\n"
//               "INTO AN UNDEFINED PROTEIN-BASED GLIBBERISH LIQUID BEFORE BEEING ABLE TO\n"
//               "SWITCH TO A DIFFERENT STUDENTs PROJECT\n"
//               "Maybe the lacking structure in terms of programming interfaces motivated the author to apply anti-patterns when possible\n"
//               "Maybe a good intermediate assignment for the next years would be to submit interfaces which a random HiWi named after a strange"
//               " platypus\n has to review and fix the best one so that students are\n forced not to use anti-patterns and may cry and shit arround less"
//               " in the form of spaghetti code and mimimi\n"
//               "Anderst formuliert, was folgt ist E-KEL-HAFT\n"
//               "\tThanks for your attention\n\n\n" << std::endl;

  // build symbol table, report if a symbol collision occurs
  this->_symbol_tree = build_symbol_table(nullptr, _parse_tree);
  //std::cout << *_symbol_tree << std::endl;

  // take ops out of parse tree and link them to symbol table entries or temp results, report on type error
  this->_ast = build_ast(_parse_tree, _symbol_tree);

  // do all other checks

  return this->_ast;
}

std::shared_ptr<SymbolScopeNode> SemanticAnalysis::build_symbol_table(std::shared_ptr<SymbolScopeNode> current_scope,
                                                                      std::shared_ptr<const ParserNode> &sub_tree) {
  std::shared_ptr<SymbolScopeNode> scope = std::make_shared<SymbolScopeNode>(current_scope, _logger);
  for (auto &node : sub_tree->getChildren()) {
    auto parser_node = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*node));

    if (parser_node->getParserNodeType() == ParserNodeType::declarations) {

      for (auto &d_node : parser_node->getChildren()) {
        auto d_parser_node = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*d_node));

        switch (d_parser_node->getParserNodeType()) {
        case ParserNodeType::const_declaration: {
          scope->addChild(const_declaration(d_parser_node, scope));
          continue;
        }
        case ParserNodeType::type_declaration: {
          scope->addChild(type_declaration(d_parser_node, scope));
          continue;
        }
        case ParserNodeType::var_declaration: {
          auto vars = var_declaration(d_parser_node, scope, true);
          for (const auto &var : vars) {
            scope->addChild(var);
          }
          continue;
        }
        case ParserNodeType::procedure_declaration: {
          scope->addChild(procedure_declaration(d_parser_node, scope));
          continue;
        }
        default: {
          continue;
        }
        }
      }
    }
  }
  return scope;
}

std::shared_ptr<Node> SemanticAnalysis::build_ast(std::shared_ptr<const ParserNode> sub_tree, std::shared_ptr<SymbolScopeNode> current_scope) {
  auto root = std::make_shared<Node>();

  for (auto & child : sub_tree->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    if (parser_child->getParserNodeType() == ParserNodeType::statement_sequence) {
      for (auto &st_child : parser_child->getChildren()) {
        auto pst_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*st_child));

        if (pst_child->getParserNodeType() == ParserNodeType::statement) {
          for (auto &cst_child : pst_child->getChildren()) {
            auto pcst_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*cst_child));

            switch (pcst_child->getParserNodeType()) {
            case ParserNodeType::assignment: {
              root->addChild(parse_assignment(pcst_child, current_scope));
              break;
            }
            // TODO If and While: parse boolean expr
            case ParserNodeType::while_statement: {
              root->addChild(parse_loop(pcst_child, current_scope));
              break;
            }
            case ParserNodeType::if_statement: {
              root->addChild(parse_branch(pcst_child, current_scope));
              break;
            }
            // TODO look up procedure, parse actual params & check
            case ParserNodeType::procedure_call: {
              root->addChild(parse_call(pcst_child, current_scope));
              break;
            }
            default: continue;
            }
          }
        }
      }
    }
  }
  return root;
}

std::shared_ptr<CallNode> SemanticAnalysis::parse_call(std::shared_ptr<const ParserNode> proc_call, std::shared_ptr<SymbolScopeNode> current_scope) {
return std::shared_ptr<CallNode>();
}

std::shared_ptr<BranchNode> SemanticAnalysis::parse_branch(std::shared_ptr<const ParserNode> if_statement, std::shared_ptr<SymbolScopeNode> current_scope) {
return std::shared_ptr<BranchNode>();
}

std::shared_ptr<LoopNode> SemanticAnalysis::parse_loop(std::shared_ptr<const ParserNode> loop, std::shared_ptr<SymbolScopeNode> current_scope) {
return std::shared_ptr<LoopNode>();
}

std::shared_ptr<AssignmentNode> SemanticAnalysis::parse_assignment(std::shared_ptr<const ParserNode> assignment, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::shared_ptr<AssignmentNode> asg;
  std::vector<std::string> identifier;
  std::shared_ptr<NumberNode> var_expr;
  std::shared_ptr<DeclarationNode> id;

  for (auto &child : assignment->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parse_identifier(parser_child, current_scope);
      if (identifier.empty()) {
        std::string s = "Const declaration identifiers must not be within arrays or records";
        _logger->error(s);
      }
      id = look_up(identifier, current_scope);
      continue;
    }
    case ParserNodeType::expression: {
      var_expr = eval_var_expr(parser_child, current_scope);
      continue;
    }
    default: {
      continue;
    }
    }
  }

return std::make_shared<AssignmentNode>(id, var_expr);
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
      expr = eval_const_expr(parser_child, current_scope);
      continue;
    }
    default: {
      continue;
    }
    }
  }
  return std::make_shared<NumberNode>(identifier[0], expr, DeclarationType::CONST);
}

std::shared_ptr<DeclarationNode> SemanticAnalysis::type_declaration(std::shared_ptr<const ParserNode> type_decl,
                                                                    std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::string> identifier;
  std::shared_ptr<DeclarationNode> type;

  for (auto &child : type_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::identifier: {
      identifier = parse_identifier(parser_child, current_scope);
      if (identifier.size() > 1 || identifier.empty()) {
        std::string s = "expected exactly one identifier in type declaration, found 0 or more than 1";
        _logger->error(s);
      }
      continue;
    }
    case ParserNodeType::type: {
      type = parse_type(parser_child, false, current_scope);
      if (!type) {
        return nullptr;
      }
      type->setName(identifier[0]);
      continue;
    }
    default: {
      continue;
    }
    }
  }
  return type;
}

std::vector<std::shared_ptr<DeclarationNode>> SemanticAnalysis::var_declaration(std::shared_ptr<const ParserNode> var_decl,
                                                                                std::shared_ptr<SymbolScopeNode> scope,
                                                                                bool var_) {
  std::vector<std::string> identifier;
  std::vector<std::string> id_list;
  std::vector<std::shared_ptr<DeclarationNode>> var_declarations; // of one type
  std::shared_ptr<DeclarationNode> var;

  for (auto &child : var_decl->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::ident_list: {
      for (auto &ids :parser_child->getChildren()) {
        auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*ids));
        if (ParserNodeType::identifier == p_child->getParserNodeType()) {
          identifier = parse_identifier(p_child, scope);
          if (identifier.size() > 1) {
            std::string s = "Const declaration identifiers must not be within arrays or records";
            _logger->error(s);
          }
          id_list.push_back(identifier[0]);
        }
      }
      continue;
    }
    case ParserNodeType::type: {
      var = parse_type(parser_child, var_, scope);
    }
    default: {
      continue;
    }
    }
  }
  auto type = var->getType();
  var_declarations.reserve(id_list.size());
  for (auto &id : id_list) {
    if (type == "NumberNode") {
      NumberNode temp = dynamic_cast<NumberNode &>(*var);
      temp.setName(id);
      var_declarations.push_back(std::make_shared<NumberNode>(temp));
    } else if (type == "ArrayNode") {
      ArrayNode temp = dynamic_cast<ArrayNode &>(*var);
      temp.setName(id);
      var_declarations.push_back(std::make_shared<ArrayNode>(temp));
    } else if (type == "RecordNode") {
      RecordNode temp =  dynamic_cast<RecordNode &>(*var);
      temp.setName(id);
      var_declarations.push_back(std::make_shared<RecordNode>(temp));
    } else if (type == "TypeNode") {
      TypeNode temp =  dynamic_cast<TypeNode &>(*var);
      temp.setName(id);
      var_declarations.push_back(std::make_shared<TypeNode>(temp));
    }
  }
  return var_declarations;
}

std::shared_ptr<ProcedureNode> SemanticAnalysis::procedure_declaration(std::shared_ptr<const ParserNode> proc_decl,
                                                                       std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::string> identifier;
  std::vector<std::shared_ptr<DeclarationNode>> params;
  std::shared_ptr<SymbolScopeNode> scope_child;
  std::shared_ptr<Node> proc_ast;

  for (auto &child : proc_decl->getChildren()) {
    auto parser_child = std::make_shared<const ParserNode>(dynamic_cast<const ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {

    case ParserNodeType::procedure_heading: {
      for (auto &ph_child : parser_child->getChildren()) {
        auto cpp_rapist = std::make_shared<const ParserNode>(dynamic_cast<const ParserNode &>(*ph_child));
        switch (cpp_rapist->getParserNodeType()) {
        case ParserNodeType::identifier: {
          identifier = parse_identifier(cpp_rapist, current_scope);
          if (identifier.size() > 1 || identifier.empty()) {
            std::string s = "Const declaration identifiers must not be within arrays or records";
            _logger->error(s);
          }
          break;
        }
        case ParserNodeType::formal_parameters: {
          params = parse_params(cpp_rapist, current_scope);
          break;
        }
        default:continue;
        }
      }
      break;
    }
    case ParserNodeType::procedure_body: {
      // TODO ast, then this then finished
      // std::cout << *parser_child << std::endl;
      scope_child = build_symbol_table(current_scope, parser_child);
      proc_ast = build_ast(parser_child, scope_child);
    }
    default: {
      continue;
    }
    }
  }

  auto decl = std::make_shared<ProcedureNode>(identifier[0], params, scope_child, proc_ast);

  return decl;
}

std::shared_ptr<DeclarationNode> SemanticAnalysis::look_up(std::vector<std::string> identifier,
                                                           std::shared_ptr<SymbolScopeNode> scope) {
  // find declaration (node) for identifier
  auto ident = identifier[0];

  if (!scope) {
    std::string s = "Couldn't find symbol " + ident + " did you declare it?";
    _logger->error(s);
  }

  std::string type;
  std::shared_ptr<Node> node;
  for (const auto &decl_ : scope->getChildren()) {
    auto decl = std::make_shared<DeclarationNode>(dynamic_cast<DeclarationNode &>(*decl_));
    if (decl->getName() == ident) {
      node = decl_;
      type = decl->getType();
      break;
    }
  }
  if (!node) {
    if (!scope) {
      std::string s = "Couldn't find symbol" + ident + "did you declare it?";
      _logger->error(s);
    } else {
      return look_up(identifier, scope->getContainingScope());
    }
  }

  for (int i = 1; i < identifier.size(); i++) {

    if (identifier[i] == ".") {
      // case . : look for field in record
      auto record = std::make_shared<RecordNode>(dynamic_cast<RecordNode &>(*node));
      node = record->getField(identifier[i + 1]);
      i++;
    } else if (identifier[i] == "[") {
      // case [ : look for array including size
      auto array = std::make_shared<ArrayNode>(dynamic_cast<ArrayNode &>(*node));
      node = array->getValue(std::stoi(identifier[i + 1]));
      i = i + 2;
    }
  }

  if (type == "NumberNode") {
    return std::make_shared<NumberNode>(dynamic_cast<NumberNode &>(*node));
  } else if (type == "ArrayNode") {
    return std::make_shared<ArrayNode>(dynamic_cast<ArrayNode &>(*node));
  } else if (type == "RecordNode") {
    return std::make_shared<RecordNode>(dynamic_cast<RecordNode &>(*node));
  } else if (type == "TypeNode") {
    return std::make_shared<TypeNode>(dynamic_cast<TypeNode &>(*node));
  } else if (type == "ProcedureNode") {
    return std::make_shared<ProcedureNode>(dynamic_cast<ProcedureNode &>(*node));
  } else {
    std::cout << "shuck this fit!! " << type << std::endl;
    return nullptr;
  }
}

std::shared_ptr<ArrayNode> SemanticAnalysis::parse_array(std::shared_ptr<ParserNode> array_type,
                                                         bool var,
                                                         std::shared_ptr<SymbolScopeNode> current_scope) {
  int expr = 0;
  std::shared_ptr<DeclarationNode> type;

  for (auto &child : array_type->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::expression: {
      expr = eval_const_expr(parser_child, current_scope);
      continue;
    }
    case ParserNodeType::type: {
      type = parse_type(parser_child, false, current_scope);
    }
    default: {
      continue;
    }
    }
  }
  return var ? std::make_shared<ArrayNode>("TOBESET", DeclarationType::VAR, expr, type) :
         std::make_shared<ArrayNode>("TYPE", DeclarationType::TYPE, expr, type);
}

std::shared_ptr<RecordNode> SemanticAnalysis::parse_record(std::shared_ptr<ParserNode> record_type,
                                                           bool var,
                                                           std::shared_ptr<SymbolScopeNode> scope) {
  std::shared_ptr<SymbolScopeNode> record_scope = std::make_shared<SymbolScopeNode>(scope, _logger);

  for (auto &parser_child : record_type->getChildren()) {
    auto field_list = std::make_shared<const ParserNode>(dynamic_cast<const ParserNode &>(*parser_child));

    if (ParserNodeType::field_list == field_list->getParserNodeType()) {
      // parse each ident list w. type
      auto vars = var_declaration(field_list, scope, var);
      for (const auto &var_d : vars) {
        record_scope->addChild(var_d);
      }
    }
  }
  return var ? std::make_shared<RecordNode>("TOBESET", DeclarationType::VAR, record_scope) :
         std::make_shared<RecordNode>("TYPE", DeclarationType::TYPE, record_scope);
}

std::vector<std::shared_ptr<DeclarationNode>> SemanticAnalysis::parse_params(std::shared_ptr<const ParserNode> params,
                                                                             std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::shared_ptr<DeclarationNode>> result;

  for (const auto &tok : params->getChildren()) {
    auto token = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*tok));

    if (token->getParserNodeType() == ParserNodeType::fp_section) {
      auto fp_token = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*token->getChildren()[0]));
      for (auto &var : var_declaration(token, current_scope,
                                       fp_token->getParserNodeType() == ParserNodeType::terminal_token
                                           && fp_token->getValue() == "VAR"))
        result.push_back(var);
    }
  }
  return result;
}

std::shared_ptr<DeclarationNode> SemanticAnalysis::parse_type(std::shared_ptr<ParserNode> type_,
                                                              bool var,
                                                              std::shared_ptr<SymbolScopeNode> current_scope) {
  auto pt_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*type_->getChildren()[0]));

  current_scope->getChildren();

  switch (pt_child->getParserNodeType()) {
  case ParserNodeType::identifier: {
    if (pt_child->getValue() == "INTEGER") {
      return var ? std::make_shared<NumberNode>("TOBESET", -999999, DeclarationType::VAR)
                 : std::make_shared<NumberNode>("TYPE", 0, DeclarationType::TYPE);
    }
    auto node = look_up(parse_identifier(pt_child, current_scope), current_scope);
    if (node->getDeclType() != DeclarationType::TYPE) {
      throw "cant use a variable as a type";
    }
    auto type = node->getType();
    if (type == "ArrayNode") {
      auto array = dynamic_cast<ArrayNode &>(*node);
      return var ? std::make_shared<ArrayNode>("TOBESET", DeclarationType::VAR, array.getSize(), array.getArrayType())
                 : std::make_shared<ArrayNode>(array);
    } else if (type == "RecordNode") {
      auto record = dynamic_cast<RecordNode &>(*node);
      return var ? std::make_shared<RecordNode>("TOBESET", DeclarationType::VAR, record.getScope())
                 : std::make_shared<RecordNode>(record);
    } else if (type == "TypeNode") {
      auto type_n = dynamic_cast<TypeNode &>(*node);
      return var ? std::make_shared<TypeNode>("TOBESET", type_n.getAliased(), DeclarationType::VAR)
                 : std::make_shared<TypeNode>(type_n);
    }
  }
  case ParserNodeType::array_type: {
    return parse_array(pt_child, var, current_scope);
  }
  case ParserNodeType::record_type: {
    return parse_record(pt_child, var, current_scope);
  }
  default:return nullptr; //unreachable
  }
}

std::vector<std::string> SemanticAnalysis::parse_identifier(std::shared_ptr<const ParserNode> identifer,
                                                            std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::string> result({identifer->getValue()});
  // identifier after End keyword
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
        auto id = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*tokens[i + 1]));
        result.push_back(id->getValue());
        i++;
      } else if (token->getValue() == "[") {
        result.push_back(token->getValue());
        auto expr = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*tokens[i + 1]));
        result.push_back(to_string(eval_const_expr(expr, current_scope)));
        result.emplace_back("]");
        i = i + 2;
      }
    }
  }
  return result;
}

int SemanticAnalysis::eval_const_expr(std::shared_ptr<ParserNode> expr, std::shared_ptr<SymbolScopeNode> current_scope) {
  int result = 0;
  for (auto &child : expr->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::simple_expression: {
      result = eval_const_simple_expr(parser_child, current_scope);
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

int SemanticAnalysis::eval_const_simple_expr(std::shared_ptr<ParserNode> simple_expr,
                                       std::shared_ptr<SymbolScopeNode> current_scope) {
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
      numbers.push_back(eval_const_term(p_child, current_scope));
      continue;
    }
    default:std::string msg = "unexpected token, check your parsing of the tree fool";
      _logger->error(msg);
      return 0;
    }
  }
  return calculate(numbers, ops);
}

int SemanticAnalysis::eval_const_term(std::shared_ptr<ParserNode> term, std::shared_ptr<SymbolScopeNode> current_scope) {
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
      numbers.push_back(eval_const_factor(p_child, current_scope));
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
    } else if (op == "+") {
      result = result + numbers.back();
      numbers.pop_back();
    } else if (op == "*") {
      result = result * numbers.back();
      numbers.pop_back();
    } else if (op == "/") {
      result = result / numbers.back();
      numbers.pop_back();
    } else if (op == "%") {
      result = result % numbers.back();
      numbers.pop_back();
    } else {
      std::string msg = "Wrong operator in const declaration rhs, check your code mate";
      _logger->error(msg);
    }
  }
  return result;
}

int SemanticAnalysis::eval_const_factor(std::shared_ptr<ParserNode> factor, std::shared_ptr<SymbolScopeNode> current_scope) {
  int res = -999999999;

  for (const auto &child : factor->getChildren()) {
    auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));
    switch (p_child->getParserNodeType()) {
    case ParserNodeType::binary_op: {
      std::string msg = "~ not allowed in constant declarations right hand side";
      _logger->error(msg);
      break;
    }
    case ParserNodeType::number: {
      res = std::stoi(p_child->getValue());
      break;
    }
    case ParserNodeType::expression: {
      res = eval_const_expr(p_child, current_scope);
      break;
    }
    case ParserNodeType::identifier: {
      auto declaration_node = look_up(parse_identifier(p_child, current_scope), current_scope);
      auto number = std::make_shared<NumberNode>(dynamic_cast<NumberNode &>(*declaration_node));
      if (!number || number->getDeclType() != DeclarationType::CONST) {
        std::string msg = "no const declaration found with such a name" + p_child->getValue();
        _logger->error(msg);
        break;
      }
      res = number->getValue();
      break;
    }
    default:continue;
    }
  }
  return res;
}

std::shared_ptr<NumberNode> SemanticAnalysis::eval_var_expr(std::shared_ptr<ParserNode> expr, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::shared_ptr<NumberNode> result;
  for (auto &child : expr->getChildren()) {
    auto parser_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (parser_child->getParserNodeType()) {
    case ParserNodeType::simple_expression: {
      result = eval_var_simple_expr(parser_child, current_scope);
      continue;
    }
    case ParserNodeType::binary_op:
    default: {
      std::string s = "comparators are not allowed on the rhs of var assignments";
      _logger->error(s);
      return result;
    }
    }
  }
  return result;
}

std::shared_ptr<NumberNode> SemanticAnalysis::eval_var_simple_expr(std::shared_ptr<ParserNode> simple_expr,
                                             std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::shared_ptr<NumberNode>> numbers;
  std::vector<std::string> ops;
  for (auto &child : simple_expr->getChildren()) {
    auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (p_child->getParserNodeType()) {
    case ParserNodeType::binary_op: {
      if (p_child->getValue() == "OR") {
        std::string msg = "OR not allowed in var assignment right hand side";
        _logger->error(msg);
        return nullptr;
      }
      ops.push_back(p_child->getValue());
      continue;
    }
    case ParserNodeType::term: {
      numbers.push_back(eval_var_term(p_child, current_scope));
      continue;
    }
    default:std::string msg = "unexpected token, check your parsing of the tree fool";
      _logger->error(msg);
      return nullptr;
    }
  }
  return build_arith_op_tree(numbers, ops);
}

std::shared_ptr<NumberNode> SemanticAnalysis::eval_var_term(std::shared_ptr<ParserNode> term, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::vector<std::shared_ptr<NumberNode>> numbers;
  std::vector<std::string> ops;
  for (auto &child : term->getChildren()) {
    auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));

    switch (p_child->getParserNodeType()) {
    case ParserNodeType::binary_op: {
      if (p_child->getValue() == "AND") {
        std::string msg = "AND not allowed in var assignments right hand side";
        _logger->error(msg);
        std::cout << "right here" << std::endl;
        return nullptr;
      }
      ops.push_back(p_child->getValue());
      continue;
    }
    case ParserNodeType::factor: {
      numbers.push_back(eval_var_factor(p_child, current_scope));
      continue;
    }
    default:std::string msg = "unexpected token, check your parsing of the tree fool";
      _logger->error(msg);
      std::cout << "right here" << std::endl;
      return nullptr;
    }
  }
  return build_arith_op_tree(numbers, ops);
}



std::shared_ptr<NumberNode> SemanticAnalysis::build_arith_op_tree(std::vector<std::shared_ptr<NumberNode>> numbers, std::vector<std::string> ops) {
  if (ops.empty()) {
    return numbers[0];}
  std::reverse(numbers.begin(), numbers.end());
  std::reverse(ops.begin(), ops.end());
  auto arith = std::make_shared<ArithmeticOpNode>(ops.back());
  if (ops.size() == numbers.size() ) {
    arith->addChild(std::make_shared<NumberNode>("temp", 0, DeclarationType::VAR));
  } else {
    arith->addChild(numbers.back());
    numbers.pop_back();
  }
  ops.pop_back();

  std::shared_ptr<ArithmeticOpNode> prev = arith;
  std::shared_ptr<ArithmeticOpNode> next;
  while(!ops.empty()) {
    next = std::make_shared<ArithmeticOpNode>(ops.back());
    next->addChild(numbers.back());
    prev->addChild(next);
    ops.pop_back();
    numbers.pop_back();
    prev = next;
  }
  prev->addChild(numbers.back());

  std::cout << "right here" << std::endl;
  return arith;
}

std::shared_ptr<NumberNode> SemanticAnalysis::eval_var_factor(std::shared_ptr<ParserNode> factor, std::shared_ptr<SymbolScopeNode> current_scope) {
  std::shared_ptr<NumberNode> number;

  for (const auto &child : factor->getChildren()) {
    auto p_child = std::make_shared<ParserNode>(dynamic_cast<ParserNode &>(*child));
    switch (p_child->getParserNodeType()) {
    case ParserNodeType::binary_op: {
      std::string msg = "~ not allowed in constant declarations right hand side";
      _logger->error(msg);
      break;
    }
    case ParserNodeType::number: {
      number = std::make_shared<NumberNode>("temp", std::stoi(p_child->getValue()), DeclarationType::CONST);
    }
    case ParserNodeType::expression: {
      number = eval_var_expr(p_child, current_scope);
    }
    case ParserNodeType::identifier: {
      auto declaration_node = look_up(parse_identifier(p_child, current_scope), current_scope);
      number = std::make_shared<NumberNode>(dynamic_cast<NumberNode &>(*declaration_node));
    }
    default:continue;
    }
  }
  return number;
}