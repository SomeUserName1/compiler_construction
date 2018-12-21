//
// Created by Michael Grossniklaus on 11/20/18.
//


#include "Parser.h"

Parser::Parser(Scanner *scanner, Logger *logger) :
    scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const Node> Parser::parse() {
  auto parse_tree(module());

  std::cout << *parse_tree << std::endl;

  return parse_tree;
}

const std::unique_ptr<Node> Parser::module() {
  // Module declaration
  decideToken(TokenType::kw_module);

  auto moduleNode = std::make_unique<Node>(NodeType::module, word->getPosition());
  const auto ident = identifier();
  moduleNode->addChild(*ident);
  decideToken(TokenType::semicolon);

  // Declarations
  moduleNode->addChild(*declarations());

  // Optional: Begin (main method)
  if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
    decideToken(TokenType::kw_begin);
    moduleNode->addChild(*statement_sequence());
  }

  //End
  decideToken(TokenType::kw_end);
  if (ident->getValue() != identifier()->getValue()) {
    std::string s = std::string("Expected module identifier module()");
    fail(s);
  }
  decideToken(TokenType::period);

  return moduleNode;
}

const std::shared_ptr<Node> Parser::identifier() {
  word = scanner_->nextToken();

  if (word->getType() != TokenType::const_ident) {
    std::string s = std::string("Expected identifier. identifier()");
    fail(s);
  }
  auto *identifier = (IdentToken *) &*word;
  auto node = std::make_shared<Node>(NodeType::identifier, word->getPosition(), identifier->getValue());

  return node;
}

const std::shared_ptr<Node> Parser::declarations() {
  auto node = std::make_shared<Node>(NodeType::declarations, word->getPosition());

  // CONSTs
  if (scanner_->peekToken()->getType() == TokenType::kw_const) {
    decideToken(TokenType::kw_const);

    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
      node->addChild(*const_declarations());
    }
  }

  // TYPEs
  if (scanner_->peekToken()->getType() == TokenType::kw_type) {
    decideToken(TokenType::kw_type);
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
      node->addChild(*type_declarations());
    }
  }

  // VARs
  if (scanner_->peekToken()->getType() == TokenType::kw_var) {
    decideToken(TokenType::kw_var);
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
      node->addChild(*var_declarations());
    }
  }


  // Optional Procedures
  while (scanner_->peekToken()->getType() == TokenType::kw_procedure) {
    node->addChild(*procedure_declaration());
  }

  return node;
}

const std::shared_ptr<Node> Parser::const_declarations() {
  auto node = std::make_shared<Node>(NodeType::const_declarations, word->getPosition());

  node->addChild(*identifier());
  decideToken(TokenType::op_eq);
  node->addChild(*expression());
  decideToken(TokenType::semicolon);

  return node;
}

const std::shared_ptr<Node> Parser::type_declarations() {
  auto node = std::make_shared<Node>(NodeType::type_declarations, word->getPosition());

  node->addChild(*identifier());
  decideToken(TokenType::op_eq);
  node->addChild(*type());
  decideToken(TokenType::semicolon);

  return node;
}

const std::shared_ptr<Node> Parser::var_declarations() {
  auto node = std::make_shared<Node>(NodeType::var_declarations, word->getPosition());

  ident_list();
  decideToken(TokenType::colon);
  node->addChild(*type());
  decideToken(TokenType::semicolon);

  return node;
}

const std::shared_ptr<Node> Parser::procedure_declaration() {
  auto node = std::make_shared<Node>(NodeType::procedure_declaration, word->getPosition());

  node->addChild(*procedure_heading());
  decideToken(TokenType::semicolon);
  node->addChild(*procedure_body());

  return node;
}

const std::shared_ptr<Node> Parser::expression() {
  auto node = std::make_shared<Node>(NodeType::expression, word->getPosition());

  node->addChild(*simple_expression());

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::op_eq || type == TokenType::op_neq
      || type == TokenType::op_lt || type == TokenType::op_leq
      || type == TokenType::op_gt || type == TokenType::op_geq) {
    node->addChild(*binary_op());
    node->addChild(*simple_expression());
  }

  return node;
}

const std::shared_ptr<Node> Parser::simple_expression() {
  auto node = std::make_shared<Node>(NodeType::simple_expression, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::op_plus || type == TokenType::op_minus) {
    node->addChild(*binary_op());
  }
  node->addChild(*term());

  type = scanner_->peekToken()->getType();
  while (type == TokenType::op_plus || type == TokenType::op_minus
      || type == TokenType::op_or) {
    node->addChild(*binary_op());
    node->addChild(*term());

    type = scanner_->peekToken()->getType();
  }

  return node;
}

const std::shared_ptr<Node> Parser::term() {
  auto node = std::make_shared<Node>(NodeType::term, word->getPosition());

  node->addChild(*factor());

  TokenType type = scanner_->peekToken()->getType();
  while (type == TokenType::op_times || type == TokenType::op_div
      || type == TokenType::op_mod || type == TokenType::op_and) {
    node->addChild(*binary_op());
    node->addChild(*factor());

    type = scanner_->peekToken()->getType();
  }

  return node;
}

const std::shared_ptr<Node> Parser::number() {
  word = scanner_->nextToken();

  if (word->getType() != TokenType::const_number) {
    std::string s = std::string("Expected number.");
    fail(s);
  }
  auto *number = (NumberToken *) &*word;
  auto node = std::make_shared<Node>(NodeType::number, word->getPosition(), std::to_string(number->getValue()));

  return node;
}

const std::shared_ptr<Node> Parser::factor() {
  auto node = std::make_shared<Node>(NodeType::factor, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::const_ident) {
    node->addChild(*identifier());
    node->addChild(*selector());
  } else if (type == TokenType::const_number) {
    node->addChild(*number());
  } else if (type == TokenType::lparen) {
    decideToken(TokenType::lparen);
    node->addChild(*expression());
    decideToken(TokenType::rparen);
  } else if (type == TokenType::op_not) {
    decideToken(TokenType::op_not);
    node->addChild(*factor());
  } else {
    word = scanner_->nextToken();
    std::string s = std::string("Expected a factor but was something else factor()");
    fail(s);
  }

  return node;
}

const std::shared_ptr<Node> Parser::type() {
  auto node = std::make_shared<Node>(NodeType::factor, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::const_ident) {
    node->addChild(*identifier());
  } else if (type == TokenType::kw_array) {
    node->addChild(*array_type());
  } else if (type == TokenType::kw_record) {
    node->addChild(*record_type());
  } else {
    std::string s = std::string("Unknown error (expected type) type()");
    fail(s);
  }

  return node;
}

const std::shared_ptr<Node> Parser::array_type() {
  auto node = std::make_shared<Node>(NodeType::array_type, word->getPosition());

  decideToken(TokenType::kw_array);
  node->addChild(*expression());
  decideToken(TokenType::kw_of);
  node->addChild(*type());

  return node;
}

const std::shared_ptr<Node> Parser::record_type() {
  auto node = std::make_shared<Node>(NodeType::record_type, word->getPosition());

  decideToken(TokenType::kw_record);
  node->addChild(*field_list());

  TokenType type = scanner_->peekToken()->getType();
  while (type == TokenType::semicolon) {
    decideToken(TokenType::semicolon);
    node->addChild(*field_list());

    type = scanner_->peekToken()->getType();
  }
  decideToken(TokenType::kw_end);

  return node;
}

const std::shared_ptr<Node> Parser::field_list() {
  auto node = std::make_shared<Node>(NodeType::field_list, word->getPosition());

  TokenType t_type = scanner_->peekToken()->getType();
  if (t_type == TokenType::const_ident) {
    node->addChild(*ident_list());
    decideToken(TokenType::colon);
    node->addChild(*type());
  }
  return node;
}

const std::shared_ptr<Node> Parser::ident_list() {
  auto node = std::make_shared<Node>(NodeType::ident_list, word->getPosition());

  node->addChild(*identifier());

  while (scanner_->peekToken()->getType() == TokenType::comma) {
    decideToken(TokenType::comma);
    node->addChild(*identifier());
  }

  return node;
}

const std::shared_ptr<Node> Parser::procedure_heading() {
  auto node = std::make_shared<Node>(NodeType::procedure_heading, word->getPosition());

  decideToken(TokenType::kw_procedure);
  node->addChild(*identifier());

  if (scanner_->peekToken()->getType() == TokenType::lparen) {
    node->addChild(*formal_parameters());
  }
  return node;
}

const std::shared_ptr<Node> Parser::procedure_body() {
  auto node = std::make_shared<Node>(NodeType::procedure_body, word->getPosition());

  node->addChild(*declarations());

  if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
    decideToken(TokenType::kw_begin);
    node->addChild(*statement_sequence());
  }
  decideToken(TokenType::kw_end);
  node->addChild(*identifier());
  decideToken(TokenType::semicolon);

  return node;
}

const std::shared_ptr<Node> Parser::formal_parameters() {
  auto node = std::make_shared<Node>(NodeType::formal_parameters, word->getPosition());

  decideToken(TokenType::lparen);

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::kw_var || type == TokenType::const_ident) {
    node->addChild(*fp_section());
    while (scanner_->peekToken()->getType() == TokenType::semicolon) {
      decideToken(TokenType::semicolon);
      node->addChild(*fp_section());
    }
  }
  decideToken(TokenType::rparen);

  return node;
}

const std::shared_ptr<Node> Parser::fp_section() {
  auto node = std::make_shared<Node>(NodeType::fp_section, word->getPosition());

  if (scanner_->peekToken()->getType() == TokenType::kw_var) {
    decideToken(TokenType::kw_var);
  }
  node->addChild(*ident_list());
  decideToken(TokenType::colon);
  node->addChild(*type());

  return node;
}

const std::shared_ptr<Node> Parser::statement_sequence() {
  auto node = std::make_shared<Node>(NodeType::statement_sequence, word->getPosition());

  node->addChild(*statement());
  while (scanner_->peekToken()->getType() == TokenType::semicolon) {
    decideToken(TokenType::semicolon);
    node->addChild(*statement());
  }
  return node;
}

const std::shared_ptr<Node> Parser::statement() {
  auto node = std::make_shared<Node>(NodeType::statement, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::const_ident) {
    node->addChild(*IdSel());
  } else if (type == TokenType::kw_if) {
    node->addChild(*if_statement());
  } else if (type == TokenType::kw_while) {
    node->addChild(*while_statement());
  }
  return node;
}

// Implements identifier selector before Assignment or ProcedureCall
const std::shared_ptr<Node> Parser::IdSel() {
  std::shared_ptr<Node> node;

  const auto ident = identifier();
  const auto select = selector();

  if (scanner_->peekToken()->getType() == TokenType::op_becomes) {
    node = std::make_shared<Node>(NodeType::assignment, word->getPosition());
    node->addChild(*ident);
    node->addChild(*select);
    decideToken(TokenType::op_becomes);
    node->addChild(*expression());
  } else {
    node = std::make_shared<Node>(NodeType::procedure_call, word->getPosition());
    node->addChild(*ident);
    node->addChild(*select);

    if (scanner_->peekToken()->getType() == TokenType::lparen) {
      node->addChild(*actual_parameters());
    }
  }

  return node;
}

const std::shared_ptr<Node> Parser::if_statement() {
  auto node = std::make_shared<Node>(NodeType::if_statement, word->getPosition());

  decideToken(TokenType::kw_if);
  node->addChild(*expression());
  decideToken(TokenType::kw_then);
  node->addChild(*statement_sequence());

  while (scanner_->peekToken()->getType() == TokenType::kw_elsif) {
    decideToken(TokenType::kw_elsif);
    node->addChild(*expression());
    decideToken(TokenType::kw_then);
    node->addChild(*statement_sequence());
  }
  if (scanner_->peekToken()->getType() == TokenType::kw_else) {
    decideToken(TokenType::kw_else);
    node->addChild(*statement_sequence());
  }
  decideToken(TokenType::kw_end);

  return node;
}

const std::shared_ptr<Node> Parser::while_statement() {
  auto node = std::make_shared<Node>(NodeType::while_statement, word->getPosition());

  decideToken(TokenType::kw_while);
  node->addChild(*expression());
  decideToken(TokenType::kw_do);
  node->addChild(*statement_sequence());
  decideToken(TokenType::kw_end);

  return node;
}

const std::shared_ptr<Node> Parser::actual_parameters() {
  auto node = std::make_shared<Node>(NodeType::acutal_parameters, word->getPosition());

  decideToken(TokenType::lparen);
  if (scanner_->peekToken()->getType() != TokenType::rparen) {
    node->addChild(*expression());
    while (scanner_->peekToken()->getType() == TokenType::comma) {
      decideToken(TokenType::comma);
      node->addChild(*expression());
    }
  }
  decideToken(TokenType::rparen);

  return node;
}

const std::shared_ptr<Node> Parser::selector() {
  auto node = std::make_shared<Node>(NodeType::selector, word->getPosition());

  while (scanner_->peekToken()->getType() == TokenType::period
      || scanner_->peekToken()->getType() == TokenType::lbrack) {
    if (scanner_->peekToken()->getType() == TokenType::period) {
      decideToken(TokenType::period);
      node->addChild(*identifier());
    } else {
      decideToken(TokenType::lbrack);
      node->addChild(*expression());
      decideToken(TokenType::rbrack);
    }
  }
  return node;
}

const std::shared_ptr<Node> Parser::binary_op() {
  word = scanner_->nextToken();

  TokenType type = word->getType();
  switch (type) {
    // Boolean Ops
  case TokenType::op_eq:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "=");
  case TokenType::op_neq:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "!=");
  case TokenType::op_and:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "AND");
  case TokenType::op_or:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "OR");
    // Comparators
  case TokenType::op_lt:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "<");
  case TokenType::op_leq:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "<=");
  case TokenType::op_gt:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), ">");
  case TokenType::op_geq:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), ">=");
    // Arithmetic Ops
  case TokenType::op_plus:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "+");
  case TokenType::op_minus:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "-");
  case TokenType::op_times:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "*");
  case TokenType::op_div:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "DIV");
  case TokenType::op_mod:return std::make_shared<Node>(NodeType::binary_op, word->getPosition(), "MOD");
    // No match, degrade
  default:std::string s = std::string("Expected binary operator in method binary op");
    fail(s);
    // Unreachable as fail throws
    return nullptr;
  }
}

void Parser::fail(std::string &msg) {
  std::stringstream ss;
  ss << "\"" << *word << "\"" << std::endl;
  msg = msg + " expected but got " + ss.str();

  logger_->error(word->getPosition(), msg);
  throw std::invalid_argument("You failed!" + msg);
}

void Parser::decideToken(TokenType type) {
  word = scanner_->nextToken();
  if (word->getType() != type) {
    fail(reinterpret_cast<std::string &>(type));
  }
}