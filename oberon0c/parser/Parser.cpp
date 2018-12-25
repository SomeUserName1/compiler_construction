//
// Created by Michael Grossniklaus on 11/20/18.
//


#include "Parser.h"

Parser::Parser(Lexer *scanner, Logger *logger) :
    scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const ParserNode> Parser::parse() {
  auto parse_tree(module());

  if (!this->_state)
    throw std::invalid_argument("Found errors, see ");
  std::cout << *parse_tree << std::endl;

  return parse_tree;
}

const std::unique_ptr<ParserNode> Parser::module() {
  auto moduleParserNode = std::make_unique<ParserNode>(ParserNodeType::module, this->scanner_->peekToken()->getPosition());

  // Module declaration
  moduleParserNode->addChild(decideToken(TokenType::kw_module));
  const auto ident = identifier();
  moduleParserNode->addChild({ident, decideToken(TokenType::semicolon)});

  // Declarations
  moduleParserNode->addChild(declarations());

  // Optional: Begin (main method)
  if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
    moduleParserNode->addChild({decideToken(TokenType::kw_begin), statement_sequence()});
  }

  //End
  moduleParserNode->addChild(decideToken(TokenType::kw_end));
  const auto id_end = identifier();
  if (ident->getValue() != id_end->getValue()) {
    std::string s = std::string("Expected module identifier at MODULE and END to be equal");
    fail(s);
  } else {
    moduleParserNode->addChild(id_end);
  }

  moduleParserNode->addChild(decideToken(TokenType::period));

  return moduleParserNode;
}

const std::shared_ptr<ParserNode> Parser::identifier() {
  std::shared_ptr<ParserNode> node;
  word = scanner_->nextToken();

  if (word->getType() != TokenType::const_ident) {
    node = std::make_shared<ParserNode>(ParserNodeType::syntax_error, word->getPosition(), "expected identifier");
    std::string s = std::string("Expected identifier");
    fail(s);
  } else {
    auto identifier = dynamic_cast<const IdentToken &>(*word);
    node = std::make_shared<ParserNode>(ParserNodeType::identifier, word->getPosition(), identifier.getValue());
  }
  return node;
}

const std::shared_ptr<ParserNode> Parser::declarations() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::declarations, word->getPosition());
  // CONSTs
  if (scanner_->peekToken()->getType() == TokenType::kw_const) {
    node->addChild(decideToken(TokenType::kw_const));

    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
      node->addChild(const_declarations());
    }
  }
  // TYPEs
  if (scanner_->peekToken()->getType() == TokenType::kw_type) {
    node->addChild(decideToken(TokenType::kw_type));
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
      node->addChild(type_declarations());
    }
  }
  // VARs
  if (scanner_->peekToken()->getType() == TokenType::kw_var) {
    node->addChild(decideToken(TokenType::kw_var));
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
      node->addChild(var_declarations());
    }
  }
  // Optional Procedures
  while (scanner_->peekToken()->getType() == TokenType::kw_procedure) {
    node->addChild(procedure_declaration());
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::const_declarations() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::const_declarations, word->getPosition());

  node->addChild({identifier(), decideToken(TokenType::op_eq), expression(), decideToken(TokenType::semicolon)});

  return node;
}

const std::shared_ptr<ParserNode> Parser::type_declarations() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::type_declarations, word->getPosition());

  node->addChild({identifier(), decideToken(TokenType::op_eq), type(), decideToken(TokenType::semicolon)});

  return node;
}

const std::shared_ptr<ParserNode> Parser::var_declarations() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::var_declarations, word->getPosition());

  node->addChild({ident_list(), decideToken(TokenType::colon), type(), decideToken(TokenType::semicolon)});

  return node;
}

const std::shared_ptr<ParserNode> Parser::procedure_declaration() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::procedure_declaration, word->getPosition());

  node->addChild({procedure_heading(), decideToken(TokenType::semicolon), procedure_body()});

  return node;
}

const std::shared_ptr<ParserNode> Parser::expression() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::expression, word->getPosition());

  node->addChild(simple_expression());

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::op_eq || type == TokenType::op_neq
      || type == TokenType::op_lt || type == TokenType::op_leq
      || type == TokenType::op_gt || type == TokenType::op_geq) {
    node->addChild({binary_op(), simple_expression()});
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::simple_expression() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::simple_expression, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::op_plus || type == TokenType::op_minus) {
    node->addChild(binary_op());
  }
  node->addChild(term());

  type = scanner_->peekToken()->getType();
  while (type == TokenType::op_plus || type == TokenType::op_minus
      || type == TokenType::op_or) {
    node->addChild({binary_op(), term()});

    type = scanner_->peekToken()->getType();
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::term() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::term, word->getPosition());

  node->addChild(factor());

  TokenType type = scanner_->peekToken()->getType();
  while (type == TokenType::op_times || type == TokenType::op_div
      || type == TokenType::op_mod || type == TokenType::op_and) {
    node->addChild({binary_op(), factor()});

    type = scanner_->peekToken()->getType();
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::number() {
  std::shared_ptr<ParserNode> node;
  word = scanner_->nextToken();

  if (word->getType() != TokenType::const_number) {
    node = std::make_shared<ParserNode>(ParserNodeType::syntax_error, word->getPosition(), "expected number here");
    std::string s = std::string("Expected number.");
    fail(s);
  } else {
    auto number = dynamic_cast<const NumberToken &>(*word);
    node = std::make_shared<ParserNode>(ParserNodeType::number, word->getPosition(), std::to_string(number.getValue()));
  }
  return node;
}

const std::shared_ptr<ParserNode> Parser::factor() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::factor, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  switch(type) {
    case TokenType::const_ident: {

      node->addChild({identifier(), selector()});
      break;
    }
    case TokenType::const_number: {
      node->addChild(number());
      break;
    }
    case TokenType::lparen: {
      node->addChild({decideToken(TokenType::lparen), expression(), decideToken(TokenType::rparen)});
      break;
    }
    case TokenType::op_not: {
      node->addChild({decideToken(TokenType::op_not), factor()});
      break;
    }
    default: {
      node = std::make_shared<ParserNode>(ParserNodeType::factor, word->getPosition());
      std::string s = std::string("Expected a factor");
      fail(s);
    }
  }
  return node;
}

const std::shared_ptr<ParserNode> Parser::type() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::factor, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  switch (type) {
  case TokenType::const_ident: {
    node->addChild(identifier());
    break;
  }
  case TokenType::kw_array: {
    node->addChild(array_type());
    break;
  }
  case TokenType::kw_record: {
    node->addChild(record_type());
    break;
  }
  default: {
    node = std::make_shared<ParserNode>(ParserNodeType::syntax_error, word->getPosition(), "Expected type name, "
                                                                                           "ARRAY or RECORD");
    std::string s = std::string("Expected type name, ARRAY or RECORD");
    fail(s);
    return node;
  }
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::array_type() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::array_type, word->getPosition());

  node->addChild({decideToken(TokenType::kw_array), expression(), decideToken(TokenType::kw_of), type()});

  return node;
}

const std::shared_ptr<ParserNode> Parser::record_type() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::record_type, word->getPosition());

  node->addChild({decideToken(TokenType::kw_record), field_list()});

  TokenType type = scanner_->peekToken()->getType();
  while (type == TokenType::semicolon) {
    node->addChild({decideToken(TokenType::semicolon), field_list()});

    type = scanner_->peekToken()->getType();
  }
  node->addChild(decideToken(TokenType::kw_end));

  return node;
}

const std::shared_ptr<ParserNode> Parser::field_list() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::field_list, word->getPosition());

  TokenType t_type = scanner_->peekToken()->getType();
  if (t_type == TokenType::const_ident) {
    node->addChild({ident_list(), decideToken(TokenType::colon), type()});
  }
  return node;
}

const std::shared_ptr<ParserNode> Parser::ident_list() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::ident_list, word->getPosition());

  node->addChild(identifier());

  while (scanner_->peekToken()->getType() == TokenType::comma) {
    node->addChild({decideToken(TokenType::comma), identifier()});
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::procedure_heading() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::procedure_heading, word->getPosition());

  node->addChild({decideToken(TokenType::kw_procedure), identifier()});

  if (scanner_->peekToken()->getType() == TokenType::lparen) {
    node->addChild(formal_parameters());
  }
  return node;
}

const std::shared_ptr<ParserNode> Parser::procedure_body() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::procedure_body, word->getPosition());

  node->addChild(declarations());

  if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
    node->addChild({decideToken(TokenType::kw_begin), statement_sequence()});
  }
  node->addChild({decideToken(TokenType::kw_end), identifier(), decideToken(TokenType::semicolon)});

  return node;
}

const std::shared_ptr<ParserNode> Parser::formal_parameters() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::formal_parameters, word->getPosition());

  node->addChild(decideToken(TokenType::lparen));

  TokenType type = scanner_->peekToken()->getType();
  if (type == TokenType::kw_var || type == TokenType::const_ident) {
    node->addChild(fp_section());
    while (scanner_->peekToken()->getType() == TokenType::semicolon) {
      node->addChild({decideToken(TokenType::semicolon), fp_section()});
    }
  }
  node->addChild(decideToken(TokenType::rparen));

  return node;
}

const std::shared_ptr<ParserNode> Parser::fp_section() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::fp_section, word->getPosition());

  if (scanner_->peekToken()->getType() == TokenType::kw_var) {
    node->addChild(decideToken(TokenType::kw_var));
  }
  node->addChild({ident_list(), decideToken(TokenType::colon), type()});

  return node;
}

const std::shared_ptr<ParserNode> Parser::statement_sequence() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::statement_sequence, word->getPosition());

  node->addChild(statement());
  while (scanner_->peekToken()->getType() == TokenType::semicolon) {
    node->addChild({decideToken(TokenType::semicolon), statement()});
  }
  return node;
}

const std::shared_ptr<ParserNode> Parser::statement() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::statement, word->getPosition());

  TokenType type = scanner_->peekToken()->getType();
  switch (type) {
    case TokenType::const_ident: {
      node->addChild(id_sel());
      break;
    } case TokenType::kw_if: {
      node->addChild(if_statement());
      break;
    } case TokenType::kw_while: {
      node->addChild(while_statement());
    break;
    } default: {
      break;
    }
  }
  return node;
}

// Implements identifier selector before Assignment or ProcedureCall
const std::shared_ptr<ParserNode> Parser::id_sel() {
  std::shared_ptr<ParserNode> node;

  const auto ident = identifier();
  const auto select = selector();

  if (scanner_->peekToken()->getType() == TokenType::op_becomes) {
    node = std::make_shared<ParserNode>(ParserNodeType::assignment, word->getPosition());
    node->addChild({ident, select, decideToken(TokenType::op_becomes), expression()});
  } else {
    node = std::make_shared<ParserNode>(ParserNodeType::procedure_call, word->getPosition());
    node->addChild({ident, select});

    if (scanner_->peekToken()->getType() == TokenType::lparen) {
      node->addChild(actual_parameters());
    }
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::if_statement() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::if_statement, word->getPosition());

  node->addChild({decideToken(TokenType::kw_if), expression(), decideToken(TokenType::kw_then), statement_sequence()});

  while (scanner_->peekToken()->getType() == TokenType::kw_elsif) {
    node->addChild({decideToken(TokenType::kw_elsif), expression(), decideToken(TokenType::kw_then),
                    statement_sequence()});
  }
  if (scanner_->peekToken()->getType() == TokenType::kw_else) {
    node->addChild({decideToken(TokenType::kw_else), statement_sequence()});
  }
  node->addChild(decideToken(TokenType::kw_end));

  return node;
}

const std::shared_ptr<ParserNode> Parser::while_statement() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::while_statement, word->getPosition());

  node->addChild({decideToken(TokenType::kw_while), expression(), decideToken(TokenType::kw_do), statement_sequence(),
                  decideToken(TokenType::kw_end)});

  return node;
}

const std::shared_ptr<ParserNode> Parser::actual_parameters() {
  auto node = std::make_shared<ParserNode>(ParserNodeType::acutal_parameters, word->getPosition());

  node->addChild(decideToken(TokenType::lparen));
  if (scanner_->peekToken()->getType() != TokenType::rparen) {
    node->addChild(expression());
    while (scanner_->peekToken()->getType() == TokenType::comma) {
      node->addChild({decideToken(TokenType::comma), expression()});
    }
  }
  node->addChild(decideToken(TokenType::rparen));

  return node;
}

const std::shared_ptr<ParserNode> Parser::selector() {
  std::shared_ptr<ParserNode> node;

  switch (scanner_->peekToken()->getType()) {
    case TokenType::period: {
      node = std::make_shared<ParserNode>(ParserNodeType::selector, word->getPosition());
      node->addChild({decideToken(TokenType::period), identifier()});
      break;
    }
    case TokenType::lbrack: {
      node = std::make_shared<ParserNode>(ParserNodeType::selector, word->getPosition());
      node->addChild({decideToken(TokenType::lbrack), expression(), decideToken(TokenType::rbrack)});
      break;
    }
    default: {
      node = std::make_shared<ParserNode>(ParserNodeType::selector, word->getPosition());
    }
  }

  if (scanner_->peekToken()->getType() == TokenType::period || scanner_->peekToken()->getType() == TokenType::lbrack) {
    node->addChild(selector());
  }

  return node;
}

const std::shared_ptr<ParserNode> Parser::binary_op() {
  word = scanner_->nextToken();

  TokenType type = word->getType();
  switch (type) {
    // Boolean Ops
  case TokenType::op_and:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "AND");
  case TokenType::op_or:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "OR");
    // Comparators
  case TokenType::op_eq:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "=");
  case TokenType::op_neq:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "!=");
  case TokenType::op_lt:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "<");
  case TokenType::op_leq:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "<=");
  case TokenType::op_gt:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), ">");
  case TokenType::op_geq:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), ">=");
    // Arithmetic Ops
  case TokenType::op_plus:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "+");
  case TokenType::op_minus:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "-");
  case TokenType::op_times:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "*");
  case TokenType::op_div:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "DIV");
  case TokenType::op_mod:return std::make_shared<ParserNode>(ParserNodeType::binary_op, word->getPosition(), "MOD");
    // No match, degrade
  default:
    std::string s = std::string("Expected binary operator in method binary op");
    fail(s);
    return std::make_shared<ParserNode>(ParserNodeType::syntax_error, word->getPosition(), "Expected binary operator");
  }
}

void Parser::fail(std::string &msg) {
  // TODO degrade gently
  // TODO print all relevant tokens (all containing "" in the grammar that arent already stored as a value)
  std::stringstream ss;
  ss << "\"" << *word << "\"" << std::endl;
  msg = msg + " expected but got " + ss.str();

  logger_->error(word->getPosition(), msg);
}

const std::shared_ptr<ParserNode> Parser::decideToken(TokenType type) {
  std::shared_ptr<ParserNode> node;
  word = scanner_->nextToken();
  std::stringstream ss;
  ss << "Token " << type;
  std::string c = ss.str();

  if (word->getType() != type) {
    node = std::make_shared<ParserNode>(ParserNodeType::syntax_error, word->getPosition());
    fail(c);
  } else {
    node = std::make_shared<ParserNode>(ParserNodeType::terminal_token, word->getPosition(), c);
  }
  return node;
}