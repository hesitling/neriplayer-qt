/// @file TestPlaylistViewModel.cpp
/// @brief Unit tests for PlaylistViewModel with mocked dependencies

#include "domain/Playlist.h"
#include "domain/PlaylistSummary.h"
#include "repo/IPlaylistRepository.h"
#include "viewmodel/PlaylistViewModel.h"

#include <QCoroTask>
#include <QSignalSpy>
#include <QTest>

using namespace QeriPlayerQt;

// --- Mock IPlaylistRepository ---

class MockPlaylistRepo : public IPlaylistRepository {
public:
    QVector<PlaylistSummary> findAll() override
    {
        if (m_throwOnFindAll)
            throw std::runtime_error("DB error");
        return m_summaries;
    }
    std::optional<Playlist> findById(const QString &id) override
    {
        auto it = m_playlists.find(id);
        if (it != m_playlists.end())
            return it.value();
        return std::nullopt;
    }
    Playlist create(const QString &name, MusicPlatform) override
    {
        if (m_throwOnCreate)
            throw std::runtime_error("create failed");
        Playlist p;
        p.id = QString::number(++m_nextId);
        p.name = name;
        m_createdIds.append(p.id);
        return p;
    }
    void updateMetadata(const QString &id, const QString &name, const QString &, const QString &) override
    {
        if (m_throwOnUpdateMetadata)
            throw std::runtime_error("updateMetadata failed");
        m_renamedIds[id] = name;
    }
    void remove(const QString &id) override
    {
        if (m_throwOnRemove)
            throw std::runtime_error("remove failed");
        m_removedIds.append(id);
    }
    bool addSong(const QString &, const QString &, int) override
    {
        return true;
    }
    void removeSong(const QString &, const QString &) override { }
    void reorderSongs(const QString &, const QStringList &) override { }
    int songCount(const QString &) override
    {
        return 0;
    }

    // Test data
    QVector<PlaylistSummary> m_summaries;
    QHash<QString, Playlist> m_playlists;
    QStringList m_createdIds;
    QStringList m_removedIds;
    QHash<QString, QString> m_renamedIds;
    int m_nextId = 0;
    bool m_throwOnFindAll = false;
    bool m_throwOnCreate = false;
    bool m_throwOnRemove = false;
    bool m_throwOnUpdateMetadata = false;
};

// --- Test class ---

class TestPlaylistViewModel : public QObject {
    Q_OBJECT

private:
    PlaylistSummary makeSummary(const QString &id, const QString &name)
    {
        PlaylistSummary s;
        s.id = id;
        s.name = name;
        return s;
    }

private Q_SLOTS:
    void initialState();
    void loadLocalPlaylists();
    void loadLocalPlaylists_empty();
    void createLocalPlaylist();
    void deleteLocalPlaylist();
    void renameLocalPlaylist();
    void clearError();
    void localPlaylistSelected_signal();
    void neteasePlaylistSelected_signal();
    void loadLocalPlaylists_repoException_doesNotCrash();
    void createLocalPlaylist_repoException_setsError();
    void deleteLocalPlaylist_repoException_setsError();
    void renameLocalPlaylist_repoException_setsError();
};

void TestPlaylistViewModel::initialState()
{
    MockPlaylistRepo playlistRepo;
    PlaylistViewModel vm(&playlistRepo, nullptr);

    QVERIFY(vm.localPlaylists().isEmpty());
    QVERIFY(vm.neteasePlaylists().isEmpty());
    QVERIFY(!vm.isLoading());
    QVERIFY(!vm.hasError());
}

void TestPlaylistViewModel::loadLocalPlaylists()
{
    MockPlaylistRepo playlistRepo;
    playlistRepo.m_summaries = {makeSummary("1", "Playlist A"), makeSummary("2", "Playlist B")};

    PlaylistViewModel vm(&playlistRepo, nullptr);
    QSignalSpy spy(&vm, &PlaylistViewModel::localPlaylistsChanged);

    vm.loadLocalPlaylists();

    QCOMPARE(vm.localPlaylists().size(), 2);
    QCOMPARE(spy.count(), 1);
}

void TestPlaylistViewModel::loadLocalPlaylists_empty()
{
    MockPlaylistRepo playlistRepo;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    vm.loadLocalPlaylists();

    QCOMPARE(vm.localPlaylists().size(), 0);
}

void TestPlaylistViewModel::createLocalPlaylist()
{
    MockPlaylistRepo playlistRepo;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    vm.createLocalPlaylist("New Playlist");

    QCOMPARE(playlistRepo.m_createdIds.size(), 1);
    QCOMPARE(playlistRepo.m_createdIds.first(), QStringLiteral("1"));
}

void TestPlaylistViewModel::deleteLocalPlaylist()
{
    MockPlaylistRepo playlistRepo;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    vm.deleteLocalPlaylist("abc");

    QCOMPARE(playlistRepo.m_removedIds.size(), 1);
    QCOMPARE(playlistRepo.m_removedIds.first(), QStringLiteral("abc"));
}

void TestPlaylistViewModel::renameLocalPlaylist()
{
    MockPlaylistRepo playlistRepo;
    Playlist existing;
    existing.id = "abc";
    existing.name = "Old Name";
    playlistRepo.m_playlists["abc"] = existing;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    vm.renameLocalPlaylist("abc", "New Name");

    QCOMPARE(playlistRepo.m_renamedIds["abc"], QStringLiteral("New Name"));
}

void TestPlaylistViewModel::clearError()
{
    MockPlaylistRepo playlistRepo;

    PlaylistViewModel vm(&playlistRepo, nullptr);

    QSignalSpy spy(&vm, &PlaylistViewModel::errorChanged);
    vm.clearError();

    QVERIFY(!vm.hasError());
    QCOMPARE(spy.count(), 1);
}

void TestPlaylistViewModel::localPlaylistSelected_signal()
{
    MockPlaylistRepo playlistRepo;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    QSignalSpy spy(&vm, &PlaylistViewModel::localPlaylistSelected);

    Q_EMIT vm.localPlaylistSelected("abc");

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().first().toString(), QStringLiteral("abc"));
}

void TestPlaylistViewModel::neteasePlaylistSelected_signal()
{
    MockPlaylistRepo playlistRepo;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    QSignalSpy spy(&vm, &PlaylistViewModel::neteasePlaylistSelected);

    PlaylistSummary summary;
    summary.id = "100";
    summary.name = "Test";
    Q_EMIT vm.neteasePlaylistSelected(summary);

    QCOMPARE(spy.count(), 1);
}

void TestPlaylistViewModel::loadLocalPlaylists_repoException_doesNotCrash()
{
    MockPlaylistRepo playlistRepo;
    playlistRepo.m_throwOnFindAll = true;

    PlaylistViewModel vm(&playlistRepo, nullptr);

    // Should not crash — exception is caught internally
    vm.loadLocalPlaylists();

    // Local playlists should remain empty since the repo threw
    QVERIFY(vm.localPlaylists().isEmpty());
}

void TestPlaylistViewModel::createLocalPlaylist_repoException_setsError()
{
    MockPlaylistRepo playlistRepo;
    playlistRepo.m_throwOnCreate = true;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    QSignalSpy errorSpy(&vm, &PlaylistViewModel::errorChanged);

    vm.createLocalPlaylist("New Playlist");

    QVERIFY(vm.hasError());
    QCOMPARE(vm.error().type(), ViewModelError::ErrorType::Database);
    QVERIFY(errorSpy.count() >= 1);
}

void TestPlaylistViewModel::deleteLocalPlaylist_repoException_setsError()
{
    MockPlaylistRepo playlistRepo;
    playlistRepo.m_throwOnRemove = true;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    QSignalSpy errorSpy(&vm, &PlaylistViewModel::errorChanged);

    vm.deleteLocalPlaylist("abc");

    QVERIFY(vm.hasError());
    QCOMPARE(vm.error().type(), ViewModelError::ErrorType::Database);
    QVERIFY(errorSpy.count() >= 1);
}

void TestPlaylistViewModel::renameLocalPlaylist_repoException_setsError()
{
    MockPlaylistRepo playlistRepo;
    playlistRepo.m_throwOnUpdateMetadata = true;
    Playlist existing;
    existing.id = "abc";
    existing.name = "Old Name";
    playlistRepo.m_playlists["abc"] = existing;

    PlaylistViewModel vm(&playlistRepo, nullptr);
    QSignalSpy errorSpy(&vm, &PlaylistViewModel::errorChanged);

    vm.renameLocalPlaylist("abc", "New Name");

    QVERIFY(vm.hasError());
    QCOMPARE(vm.error().type(), ViewModelError::ErrorType::Database);
    QVERIFY(errorSpy.count() >= 1);
}

QTEST_MAIN(TestPlaylistViewModel)
#include "TestPlaylistViewModel.moc"
