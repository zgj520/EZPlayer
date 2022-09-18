#include "ScopeGuard.h"

ScopeGuard::ScopeGuard(EXIT_FUNC func) {
    m_onExitFunc = func;
}

ScopeGuard::~ScopeGuard() {
    if (m_onExitFunc != nullptr) {
        m_onExitFunc();
    }
}