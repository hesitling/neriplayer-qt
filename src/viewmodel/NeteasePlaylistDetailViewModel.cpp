/// @file NeteasePlaylistDetailViewModel.cpp
/// @brief Implementation of NeteasePlaylistDetailViewModel

#include "viewmodel/NeteasePlaylistDetailViewModel.h"

#include "core/logger/Logger.h"

#include <QPointer>

namespace QeriPlayerQt {

NeteasePlaylistDetailViewModel::NeteasePlaylistDetailViewModel(NeteaseClient *neteaseClient, ISongRepository *songRepo,
                                                               IPlaylistRepository *playlistRepo, QObject *parent)
    : QObject(parent)
    , m_neteaseClient(neteaseClient)
    , m_songRepo(songRepo)
    , m_playlistRepo(playlistRepo)
    , m_songs(new SongListModel(this))
{
}

NeteasePlaylistDetailViewModel::~NeteasePlaylistDetailViewModel() = default;

QString NeteasePlaylistDetailViewModel::headerName() const
{
    return m_headerName;
}
QString NeteasePlaylistDetailViewModel::headerCoverUrl() const
{
    return m_headerCoverUrl;
}
int NeteasePlaylistDetailViewModel::headerTrackCount() const
{
    return m_headerTrackCount;
}
SongListModel *NeteasePlaylistDetailViewModel::songs() const
{
    return m_songs;
}
bool NeteasePlaylistDetailViewModel::isLoading() const
{
    return m_isLoading;
}
bool NeteasePlaylistDetailViewModel::hasError() const
{
    return m_hasError;
}
ViewModelError NeteasePlaylistDetailViewModel::error() const
{
    return m_error;
}

QCoro::QmlTask NeteasePlaylistDetailViewModel::loadPlaylist(const QString &playlistId)
{
    m_lastPlaylistId = playlistId;
    m_lastAlbumId.clear();
    m_isAlbum = false;
    m_pendingTask
        = QCoro::QmlTask([self = QPointer<NeteasePlaylistDetailViewModel>(this), playlistId]() -> QCoro::Task<void> {
              if (!self) {
                  co_return;
              }

              self->m_isLoading = true;
              Q_EMIT self->isLoadingChanged();
              self->m_hasError = false;
              Q_EMIT self->errorChanged();

              try {
                  if (self->m_neteaseClient == nullptr) {
                      self->m_isLoading = false;
                      Q_EMIT self->isLoadingChanged();
                      self->m_error = ViewModelError(ViewModelError::ErrorType::Api,
                                                     QStringLiteral("NetEase client not available"));
                      self->m_hasError = true;
                      Q_EMIT self->errorChanged();
                      co_return;
                  }

                  auto result = co_await self->m_neteaseClient->getPlaylistDetail(playlistId);
                  if (!self) {
                      co_return;
                  }

                  self->m_isLoading = false;
                  Q_EMIT self->isLoadingChanged();

                  if (result.isError()) {
                      self->m_error = ViewModelError::fromApiError(result.error());
                      self->m_hasError = true;
                      Q_EMIT self->errorChanged();
                      co_return;
                  }

                  const Playlist &playlist = result.data();
                  self->m_headerName = playlist.name;
                  self->m_headerCoverUrl = playlist.coverUrl.toString();
                  self->m_headerTrackCount = playlist.songCount;

                  Q_EMIT self->headerNameChanged();
                  Q_EMIT self->headerCoverUrlChanged();
                  Q_EMIT self->headerTrackCountChanged();

                  self->m_songs->setSongs(playlist.songs);
                  self->m_songRepo->saveBatch(playlist.songs);
              } catch (const std::exception &ex) {
                  if (!self) {
                      co_return;
                  }
                  self->m_isLoading = false;
                  Q_EMIT self->isLoadingChanged();
                  self->m_error = ViewModelError(ViewModelError::ErrorType::Unknown, QString::fromUtf8(ex.what()));
                  self->m_hasError = true;
                  Q_EMIT self->errorChanged();
                  Logger::get("viewmodel")->warn("Failed to load NetEase playlist: {}", ex.what());
              }
          }());
    return m_pendingTask;
}

QCoro::QmlTask NeteasePlaylistDetailViewModel::loadAlbum(const QString &albumId)
{
    m_lastAlbumId = albumId;
    m_lastPlaylistId.clear();
    m_isAlbum = true;
    m_pendingTask
        = QCoro::QmlTask([self = QPointer<NeteasePlaylistDetailViewModel>(this), albumId]() -> QCoro::Task<void> {
              if (!self) {
                  co_return;
              }

              self->m_isLoading = true;
              Q_EMIT self->isLoadingChanged();
              self->m_hasError = false;
              Q_EMIT self->errorChanged();

              try {
                  if (self->m_neteaseClient == nullptr) {
                      self->m_isLoading = false;
                      Q_EMIT self->isLoadingChanged();
                      self->m_error = ViewModelError(ViewModelError::ErrorType::Api,
                                                     QStringLiteral("NetEase client not available"));
                      self->m_hasError = true;
                      Q_EMIT self->errorChanged();
                      co_return;
                  }

                  auto result = co_await self->m_neteaseClient->getAlbumDetail(albumId);
                  if (!self) {
                      co_return;
                  }

                  self->m_isLoading = false;
                  Q_EMIT self->isLoadingChanged();

                  if (result.isError()) {
                      self->m_error = ViewModelError::fromApiError(result.error());
                      self->m_hasError = true;
                      Q_EMIT self->errorChanged();
                      co_return;
                  }

                  const QVector<Song> &songs = result.data();
                  if (!songs.isEmpty()) {
                      self->m_headerName = songs.first().album;
                      self->m_headerCoverUrl = songs.first().coverUrl.toString();
                      Q_EMIT self->headerNameChanged();
                      Q_EMIT self->headerCoverUrlChanged();
                  }

                  self->m_headerTrackCount = songs.size();
                  Q_EMIT self->headerTrackCountChanged();

                  self->m_songs->setSongs(songs);
                  self->m_songRepo->saveBatch(songs);
              } catch (const std::exception &ex) {
                  if (!self) {
                      co_return;
                  }
                  self->m_isLoading = false;
                  Q_EMIT self->isLoadingChanged();
                  self->m_error = ViewModelError(ViewModelError::ErrorType::Unknown, QString::fromUtf8(ex.what()));
                  self->m_hasError = true;
                  Q_EMIT self->errorChanged();
                  Logger::get("viewmodel")->warn("Failed to load NetEase album: {}", ex.what());
              }
          }());
    return m_pendingTask;
}

namespace {
QCoro::Task<void> completedTask()
{
    co_return;
}
} // namespace

QCoro::QmlTask NeteasePlaylistDetailViewModel::retry()
{
    if (m_isAlbum) {
        if (m_lastAlbumId.isEmpty()) {
            return QCoro::QmlTask(completedTask());
        }
        return loadAlbum(m_lastAlbumId);
    }

    if (m_lastPlaylistId.isEmpty()) {
        return QCoro::QmlTask(completedTask());
    }
    return loadPlaylist(m_lastPlaylistId);
}

QCoro::QmlTask NeteasePlaylistDetailViewModel::saveToLocal()
{
    m_pendingTask = QCoro::QmlTask([this]() -> QCoro::Task<void> {
        if (m_headerName.isEmpty())
            co_return;

        try {
            Playlist localPlaylist = m_playlistRepo->create(m_headerName);
            m_songRepo->saveBatch(m_songs->songs());

            for (const Song &song : m_songs->songs()) {
                m_playlistRepo->addSong(localPlaylist.id, song.id);
            }
        } catch (const std::exception &ex) {
            Logger::get("viewmodel")->warn("Failed to save playlist to local: {}", ex.what());
            m_error = ViewModelError(ViewModelError::ErrorType::Unknown, QString::fromUtf8(ex.what()));
            m_hasError = true;
            Q_EMIT errorChanged();
        }
    }());
    return m_pendingTask;
}

void NeteasePlaylistDetailViewModel::playSong(int index)
{
    Song song = m_songs->songAt(index);
    if (!song.id.isEmpty()) {
        Q_EMIT requestPlay(song);
    }
}

void NeteasePlaylistDetailViewModel::playAll()
{
    Q_EMIT requestPlayPlaylist(m_songs->songs(), 0);
}

} // namespace QeriPlayerQt
