/// @file Song.h
/// @brief Song domain model
/// @date 2024-01-15

#ifndef NERIPLAYERQT_SONG_H
#define NERIPLAYERQT_SONG_H

#include "domain/Enums.h"

#include <QMetaType>
#include <QString>
#include <QUrl>
#include <QVariantMap>

namespace NeriPlayerQt {

/**
 * @brief Represents a song across all platforms
 */
struct Song {
    QString id;
    QString title;
    QString artist;
    QString album;
    QString albumId;
    qint64 duration = 0; ///< Duration in milliseconds
    QUrl coverUrl;
    QUrl playbackUrl;
    MusicPlatform platform = MusicPlatform::Unknown;
    QVariantMap extra; ///< Platform-specific fields
};

} // namespace NeriPlayerQt

Q_DECLARE_METATYPE(NeriPlayerQt::Song)

#endif // NERIPLAYERQT_SONG_H
