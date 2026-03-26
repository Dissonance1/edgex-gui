#ifndef INFERENCERESOURCEMANAGER_H
#define INFERENCERESOURCEMANAGER_H

#include <QString>
#include <QSet>
#include <QMutex>
#include <QVector>
#include <QDebug>

struct PortSet {
    int video;
    int meta;
    int command;
    bool isValid() const { return video > 0; }
};

class InferenceResourceManager {
public:
    static InferenceResourceManager& instance() {
        static InferenceResourceManager inst;
        return inst;
    }

    // Port Management
    PortSet allocatePorts() {
        QMutexLocker locker(&m_mutex);
        int base = 5566;
        while (true) {
            bool conflict = false;
            for (int i = 0; i < 3; ++i) {
                if (m_usedPorts.contains(base + i)) {
                    conflict = true;
                    break;
                }
            }
            if (!conflict) {
                PortSet ps = { base + 2, base, base + 1 }; // Video, Meta, Cmd based on user example
                m_usedPorts.insert(base);
                m_usedPorts.insert(base + 1);
                m_usedPorts.insert(base + 2);
                return ps;
            }
            base += 3;
            if (base > 6000) return {0, 0, 0}; // Safety break
        }
    }

    void releasePorts(const PortSet& ps) {
        QMutexLocker locker(&m_mutex);
        m_usedPorts.remove(ps.video);
        m_usedPorts.remove(ps.meta);
        m_usedPorts.remove(ps.command);
    }

    // Core Management
    bool checkAndAllocateCores(const QString& coreString) {
        QMutexLocker locker(&m_mutex);
        QVector<int> cores = parseCores(coreString);
        qDebug() << "[CORE-MGR] Attempting to allocate:" << cores << "Current used:" << m_usedCores;
        for (int c : cores) {
            if (c < 0 || c > 3 || m_usedCores.contains(c)) {
                qDebug() << "[CORE-MGR] Allocation FAILED for core:" << c;
                return false;
            }
        }
        for (int c : cores) m_usedCores.insert(c);
        qDebug() << "[CORE-MGR] Allocation SUCCESS. New used:" << m_usedCores;
        return true;
    }

    void releaseCores(const QString& coreString) {
        QMutexLocker locker(&m_mutex);
        QVector<int> cores = parseCores(coreString);
        for (int c : cores) m_usedCores.remove(c);
    }

private:
    InferenceResourceManager() {}
    
    QVector<int> parseCores(const QString& s) {
        QVector<int> res;
        QString cleaned = s;
        cleaned.replace(" ", "");
        QStringList parts = cleaned.split(",");
        for (const QString& p : parts) {
            bool ok;
            int c = p.toInt(&ok);
            if (ok && c >= 0 && c <= 3) {
                if (!res.contains(c)) res.append(c);
            }
        }
        return res;
    }

    QMutex m_mutex;
    QSet<int> m_usedPorts;
    QSet<int> m_usedCores;
};

#endif // INFERENCERESOURCEMANAGER_H
