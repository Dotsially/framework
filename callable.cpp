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

CallableManager* gCallableManager = nullptr;

void CallableRegisterName(const std::string& name){
    if(!gCallableManager) return;
    if (gCallableManager->callableIndex.find(name) == gCallableManager->callableIndex.end())
    {
        Callable callable = {};
        callable.name = name;
        gCallableManager->callableIndex[name] = gCallableManager->callables.size();
        gCallableManager->callables.push_back(callable);
    }
}

void CallableAddFunction(const std::string& name, GenericFunction function)
{
    if(!gCallableManager) return;
    auto it = gCallableManager->callableIndex.find(name);
    if (it != gCallableManager->callableIndex.end())
    {
        gCallableManager->callables[it->second].function = function;
    }
}

void CallableAdd(const std::string& name, GenericFunction function){
    if(!gCallableManager) return;
    if (gCallableManager->callableIndex.find(name) == gCallableManager->callableIndex.end())
    {
        Callable callable = {};
        callable.name = name;
        callable.function = function;
        gCallableManager->callableIndex[name] = gCallableManager->callables.size();
        gCallableManager->callables.push_back(callable);
    }
}

void CallableInvoke(const std::string& callableName, std::vector<std::any> args){
    if(!gCallableManager) return;
    auto it = gCallableManager->callableIndex.find(callableName);
    if (it == gCallableManager->callableIndex.end()) return;

    size_t index = it->second;
    if (index >= gCallableManager->callables.size()) return;

    Callable* callable = &gCallableManager->callables[index];
    callable->function(args);
}

//Only call after all callables have been registered
int CallableGetIndex(const std::string& callableName){
    if(!gCallableManager) return -1;

    int index = -1;
    auto it = gCallableManager->callableIndex.find(callableName);
    if (it == gCallableManager->callableIndex.end()) return index;

    index = it->second;
    return index;
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