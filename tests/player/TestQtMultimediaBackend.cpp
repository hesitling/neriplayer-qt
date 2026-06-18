/// @file TestQtMultimediaBackend.cpp
/// @brief Integration tests for QtMultimediaBackend

#include "player/backends/QtMultimediaBackend.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <memory>

using namespace QeriPlayerQt;

class TestQtMultimediaBackend : public QObject {
    Q_OBJECT

private:
    std::unique_ptr<QtMultimediaBackend> m_backend;

private Q_SLOTS:
    void init()
    {
        // Each test gets a fresh backend
        m_backend = std::make_unique<QtMultimediaBackend>();
    }

    void cleanup()
    {
        // Explicitly destroy before next test to avoid PipeWire thread races
        m_backend.reset();
    }

    // --- Basic construction ---

    void construction_initialStateIsStopped()
    {
        QCOMPARE(m_backend->state(), PlaybackState::Stopped);
    }

    void construction_backendName()
    {
        QCOMPARE(m_backend->backendName(), QStringLiteral("Qt Multimedia"));
    }

    // --- Volume ---

    void setVolume_setsVolume()
    {
        m_backend->setVolume(0.5);
        QCOMPARE(m_backend->volume(), 0.5);
    }

    void setVolume_clampsToRange()
    {
        m_backend->setVolume(1.5);
        QCOMPARE(m_backend->volume(), 1.0);

        m_backend->setVolume(-0.5);
        QCOMPARE(m_backend->volume(), 0.0);
    }

    // --- Mute ---

    void setMuted_toggles()
    {
        QVERIFY(!m_backend->isMuted());
        m_backend->setMuted(true);
        QVERIFY(m_backend->isMuted());
        m_backend->setMuted(false);
        QVERIFY(!m_backend->isMuted());
    }

    // --- Signals ---

    void stateChanged_onStop_doesNotCrash()
    {
        QSignalSpy spy(m_backend.get(), &IPlayerBackend::stateChanged);
        m_backend->stop();
        // Stop on already-stopped player may or may not emit signal
        // Just verify it doesn't crash
        Q_UNUSED(spy);
    }

    // --- Seek ---

    void seek_doesNotCrash()
    {
        m_backend->seek(5000);
        // Position may not update immediately without media loaded
        // Just verify it doesn't crash
    }
};

QTEST_MAIN(TestQtMultimediaBackend)
#include "TestQtMultimediaBackend.moc"
