/// @file Playlist.h
/// @brief Playlist domain model
/// @date 2024-01-15

#ifndef NERIPLAYERQT_PLAYLIST_H
#define NERIPLAYERQT_PLAYLIST_H

#include "domain/Enums.h"
#include "domain/Song.h"

#include <QMetaType>
#include <QString>
#include <QUrl>
#include <QVector>

namespace NeriPlayerQt {

/**
 * @brief Represents a playlist with optional embedded songs
 */
struct Playlist {
    QString id;
    QString name;
    QString description;
    QUrl coverUrl;
    int songCount = 0;
    QString owner;
    MusicPlatform platform = MusicPlatform::Unknown;
    QVector<Song> songs; ///< Optionally populated
};

} // namespace NeriPlayerQt

Q_DECLARE_METATYPE(NeriPlayerQt::Playlist)

#endif // NERIPLAYERQT_PLAYLIST_H
