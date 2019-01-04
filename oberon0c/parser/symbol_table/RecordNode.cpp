#include "RecordNode.h"
#include "SymbolScopeNode.h"

RecordNode::RecordNode(std::string name, DeclarationType decl_type, std::shared_ptr<SymbolScopeNode> scope)
  : DeclarationNode(std::move(name), decl_type, "RecordNode", {scope}) {
}

const std::shared_ptr<DeclarationNode> RecordNode::getField(const std::string &name) {
	std::vector<std::shared_ptr<Node>> fields = this->getChildren()[0]->getChildren();
	for (const auto &node : fields) {
		auto decl_node = dynamic_cast<DeclarationNode &>(*node);
		if (decl_node.getName() == name)
          return std::shared_ptr<DeclarationNode>(&decl_node);
	}
	throw "Field not present in the record. Invalid field name!";
}

const std::shared_ptr<SymbolScopeNode> RecordNode::getScope() const {
  return std::make_shared<SymbolScopeNode>(dynamic_cast<SymbolScopeNode &>(*this->getChildren()[0]));
}

const int RecordNode::getFieldPos(const std::string &name) {
	std::vector<std::shared_ptr<Node>> fields = this->getChildren()[0]->getChildren();
	for (int i = 0; i < fields.size(); ++i) {
		auto decl_node = dynamic_cast<DeclarationNode &>(*fields[i]);
		if (decl_node.getName() == name)
			return i;
	}
	throw "Field not present in the record. Invalid field name!";
}

void RecordNode::setValue(const std::string &name, std::shared_ptr<DeclarationNode> node) {
	if (node->getType() == this->getField(name)->getType()) {
		this->getChildren()[0]->setChild(this->getFieldPos(name), node);
	} else {
		throw "Nope, invalid types";
	}
}


