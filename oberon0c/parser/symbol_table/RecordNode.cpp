#include "RecordNode.h"
#include <utility>

RecordNode::RecordNode(std::string name, int no_fields): DeclarationNode(std::move(name)), _no_fields(no_fields) {
  // TODO new Symbol Table
}

const int RecordNode::getNoFields() {
	return this->_no_fields;
}

const std::shared_ptr<DeclarationNode> RecordNode::getField(const std::string &name) {
	std::vector<std::shared_ptr<Node>> fields = this->getChildren();
	for (const auto &node : fields) {
		auto decl_node = dynamic_cast<DeclarationNode &>(*node);
		if (decl_node.getName() == name)
          return std::shared_ptr<DeclarationNode>(&decl_node);
	}
	throw "Field not present in the record. Invalid field name!";
}

const std::vector<std::shared_ptr<DeclarationNode>> RecordNode::getFields() {
  std::vector<std::shared_ptr<DeclarationNode>> res;
  res.reserve(this->getChildren().size());

  for (const auto &node : this->getChildren()) {
    res.push_back(std::shared_ptr<DeclarationNode>(&dynamic_cast<DeclarationNode &>(*node)));
  }
  return res;
}


