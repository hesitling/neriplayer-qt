/// @file PlaylistViewModel.cpp
/// @brief Implementation of PlaylistViewModel

#include "viewmodel/PlaylistViewModel.h"

namespace NeriPlayerQt {

PlaylistViewModel::PlaylistViewModel(IPlaylistRepository *playlistRepo, NeteaseClient *neteaseClient,
                                     QObject *parent)
    : QObject(parent)
    , m_playlistRepo(playlistRepo)
    , m_neteaseClient(neteaseClient)
{
}

PlaylistViewModel::~PlaylistViewModel() = default;

// --- Getters ---

QVariantList PlaylistViewModel::localPlaylists() const
{
    return m_localPlaylists;
}

QVariantList PlaylistViewModel::neteasePlaylists() const
{
    return m_neteasePlaylists;
}

QVariantList PlaylistViewModel::neteaseAlbums() const
{
    return m_neteaseAlbums;
}

bool PlaylistViewModel::isLoading() const
{
    return m_isLoading;
}

bool PlaylistViewModel::hasError() const
{
    return m_hasError;
}

ViewModelError PlaylistViewModel::error() const
{
    return m_error;
}

// --- Loading ---

void PlaylistViewModel::loadLocalPlaylists()
{
    auto summaries = m_playlistRepo->findAll();

    QVariantList list;
    for (const auto &summary : summaries) {
        list.append(QVariant::fromValue(summary));
    }

    m_localPlaylists = list;
    Q_EMIT localPlaylistsChanged();
}

void PlaylistViewModel::loadNeteasePlaylists()
{
    loadNeteasePlaylistsImpl();
}

void PlaylistViewModel::loadNeteaseAlbums()
{
    loadNeteaseAlbumsImpl();
}

// --- Local playlist CRUD ---

void PlaylistViewModel::createLocalPlaylist(const QString &name)
{
    m_playlistRepo->create(name);
    loadLocalPlaylists();
}

void PlaylistViewModel::deleteLocalPlaylist(const QString &id)
{
    m_playlistRepo->remove(id);
    loadLocalPlaylists();
}

void PlaylistViewModel::renameLocalPlaylist(const QString &id, const QString &name)
{
    // Get existing playlist to preserve other metadata
    auto playlist = m_playlistRepo->findById(id);
    if (playlist.has_value()) {
        m_playlistRepo->updateMetadata(id, name, playlist->description, playlist->coverUrl.toString());
        loadLocalPlaylists();
    }
}

// --- Error ---

void PlaylistViewModel::clearError()
{
    m_hasError = false;
    m_error = ViewModelError();
    Q_EMIT errorChanged();
}

// --- Private ---

QCoro::Task<void> PlaylistViewModel::loadNeteasePlaylistsImpl()
{
    m_isLoading = true;
    Q_EMIT isLoadingChanged();
    clearError();

    auto result = co_await m_neteaseClient->getUserPlaylists(QString()); // Empty = current user

    m_isLoading = false;
    Q_EMIT isLoadingChanged();

    if (result.isError()) {
        m_error = ViewModelError::fromApiError(result.error());
        m_hasError = true;
        Q_EMIT errorChanged();
        co_return;
    }

    QVariantList list;
    for (const auto &playlist : result.data()) {
        PlaylistSummary summary;
        summary.id = playlist.id;
        summary.name = playlist.name;
        summary.coverUrl = playlist.coverUrl;
        summary.trackCount = playlist.songCount;
        list.append(QVariant::fromValue(summary));
    }

    m_neteasePlaylists = list;
    Q_EMIT neteasePlaylistsChanged();
}

QCoro::Task<void> PlaylistViewModel::loadNeteaseAlbumsImpl()
{
    m_isLoading = true;
    Q_EMIT isLoadingChanged();
    clearError();

    auto result = co_await m_neteaseClient->getUserStarredAlbums(QString()); // Empty = current user

    m_isLoading = false;
    Q_EMIT isLoadingChanged();

    if (result.isError()) {
        m_error = ViewModelError::fromApiError(result.error());
        m_hasError = true;
        Q_EMIT errorChanged();
        co_return;
    }

    // getUserStarredAlbums returns QJsonObject, need to parse
    // For now, emit empty list — will be implemented when API is ready
    m_neteaseAlbums.clear();
    Q_EMIT neteaseAlbumsChanged();
}

} // namespace NeriPlayerQt
