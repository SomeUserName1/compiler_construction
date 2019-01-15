# Oberon-0-cpp
Oberon-0 compiler in C++

TODO: 
ReferenceNode; 
TypeNode vs. TypeDeclaration
Expression eval mittels fold

Class-Structure
- Node
    - DeclarationNode
        - ConstDeclarationNode
        - VarDeclNode
        - TypeDeclNode
        - ParameterNode
        - RecordField(?)
    - StatementNode
        - AssignmentNode
        - WhileNode
        - IfNode
        - ProcedureCallNode
    - ExpressionNode
        - ReferenceNode
        - BinExprNode
        - UnaryExprNode
        - Values 
            - string
            - Bool
            - Number
    - BlockNode 
        - ModuleNode
        - ProcedureNode
    - TypeNode
        - NumberNode
        - StringNode
        - BoolNode


