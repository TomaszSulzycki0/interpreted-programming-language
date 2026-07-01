#include<vector>
#include<memory>

class Scope
{
private:
    Scope* parent_scope = nullptr;

    // Scope defined by { }
    // Can have nested children { function(){ for(){ } }, { }}
    // will own variables, functions, etc.

public:

    Scope(); 
    Scope(Scope& parent);

};

