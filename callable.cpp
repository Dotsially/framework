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

void CallableRegisterName(CallableManager* manager, const std::string& name){
    if (manager->callableIndex.find(name) == manager->callableIndex.end())
    {
        Callable callable = {};
        callable.name = name;
        manager->callableIndex[name] = manager->callables.size();
        manager->callables.push_back(callable);
    }
}

void CallableAddFunction(CallableManager* manager, const std::string& name, GenericFunction function)
{
    auto it = manager->callableIndex.find(name);
    if (it != manager->callableIndex.end())
    {
        manager->callables[it->second].function = function;
    }
}

void CallableAdd(CallableManager* manager, const std::string& name, GenericFunction function){
    if (manager->callableIndex.find(name) == manager->callableIndex.end())
    {
        Callable callable = {};
        callable.name = name;
        callable.function = function;
        manager->callableIndex[name] = manager->callables.size();
        manager->callables.push_back(callable);
    }
}

void CallableInvoke(CallableManager *callableManager, const std::string& callableName, std::vector<std::any> args){
    auto it = callableManager->callableIndex.find(callableName);
    if (it == callableManager->callableIndex.end()) return;

    size_t index = it->second;
    if (index >= callableManager->callables.size()) return;

    Callable* callable = &callableManager->callables[index];
    callable->function(args);
}

//Only call after all callables have been registered
int CallableGetIndex(CallableManager *callableManager, const std::string& callableName){
    int index = -1;
    auto it = callableManager->callableIndex.find(callableName);
    if (it == callableManager->callableIndex.end()) return index;

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