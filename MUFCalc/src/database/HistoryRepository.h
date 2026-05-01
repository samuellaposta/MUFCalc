#pragma once
// ─────────────────────────────────────────────────────────────────────────
//  MUFCalc :: HistoryRepository
//  In-memory store of recent calculations. Kept behind a repository
//  interface so that a SQLite-backed implementation can drop in later
//  without touching the GUI.
// ─────────────────────────────────────────────────────────────────────────

#include <QPair>
#include <QVector>
#include "../core/Types.h"

namespace MUFCalc {

class HistoryRepository {
public:
    using Entry = QPair<MufInput, MufResult>;

    void  add(const MufInput& input, const MufResult& result);
    void  clear()                          { m_entries.clear(); }
    bool  isEmpty() const                  { return m_entries.isEmpty(); }
    int   size()    const                  { return m_entries.size();    }
    const Entry& at(int i) const           { return m_entries.at(i);     }
    const QVector<Entry>& all() const      { return m_entries;           }

    static constexpr int MAX_ENTRIES = 20;

private:
    QVector<Entry> m_entries;
};

}  // namespace MUFCalc
