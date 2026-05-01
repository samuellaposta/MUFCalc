#include "HistoryRepository.h"

namespace MUFCalc {

void HistoryRepository::add(const MufInput& input, const MufResult& result) {
    m_entries.prepend({ input, result });
    while (m_entries.size() > MAX_ENTRIES)
        m_entries.removeLast();
}

}  // namespace MUFCalc
