#ifndef SCOPEGUARD_H_
#define SCOPEGUARD_H_

#include <functional>

#define EXIT_FUNC std::function<void()>
class ScopeGuard
{
public:
    explicit ScopeGuard(EXIT_FUNC);
    ~ScopeGuard();

private:
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator = (ScopeGuard const&) = delete;

private:
    EXIT_FUNC m_onExitFunc = nullptr;
};

#define STR_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) STR_CAT(name, line)
#define SCOPE_GUARD(onExit) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(onExit)

#endif