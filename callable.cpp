#ifndef CALLABLE
#define CALLABLE

#include "common.cpp"

using GenericFunction = std::function<void(std::vector<std::any>)>;

struct Callable
{
    std::string name;
    GenericFunction function;
};

struct CallableManager
{
    std::vector<Callable> callables;
    std::unordered_map<std::string, uint32_t> callableIndex;
};

void CallableAdd(CallableManager *callableManager, std::string name, GenericFunction function)
{
    Callable callable = {};
    callable.name = name;
    callable.function = function;
    callableManager->callableIndex[name] = callableManager->callables.size();
    callableManager->callables.push_back(callable);
}

void CallableCall(CallableManager *callableManager, std::string callableName, std::vector<std::any> args){
    auto it = callableManager->callableIndex.find(callableName);
    if (it == callableManager->callableIndex.end()) return;

    size_t index = it->second;
    if (index >= callableManager->callables.size()) return;

    Callable* callable = &callableManager->callables[index];
    callable->function(args);
}

//Example:
/*
    void CallableFunction(std::vector<std::any> args){
        if(args.size() != 2) return;

        float* n1 = std::any_cast<float*>(args[0]);
        uint32_t n2 = std::any_cast<uint32_t>(args[1]);

        FunctionDoSomething(n1, n2);
    }
*/

#endif