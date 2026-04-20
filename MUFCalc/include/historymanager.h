#pragma once
#include "datatypes.h"
#include <QVector>
#include <QPair>

class HistoryManager {
public:
    void add(const MBPInput& input, const MUFResult& result);
    const QVector<QPair<MBPInput,MUFResult>>& all() const { return m_history; }
    bool  isEmpty() const { return m_history.isEmpty(); }
    int   size()    const { return m_history.size(); }
    const QPair<MBPInput,MUFResult>& at(int i) const { return m_history.at(i); }
    void  clear() { m_history.clear(); }
    static const int MAX_ENTRIES = 20;
private:
    QVector<QPair<MBPInput,MUFResult>> m_history;
};
