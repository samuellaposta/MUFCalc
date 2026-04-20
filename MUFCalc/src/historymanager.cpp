#include "historymanager.h"

void HistoryManager::add(const MBPInput& input, const MUFResult& result) {
    m_history.prepend({ input, result });
    if (m_history.size() > MAX_ENTRIES)
        m_history.removeLast();
}
