/// @file Album.h
/// @brief Album domain model
/// @date 2024-01-15

#ifndef NERIPLAYERQT_ALBUM_H
#define NERIPLAYERQT_ALBUM_H

#include "domain/Enums.h"

#include <QDate>
#include <QMetaType>
#include <QString>
#include <QUrl>

namespace NeriPlayerQt {

/**
 * @brief Represents an album
 */
struct Album {
    QString id;
    QString title;
    QString artist;
    QString artistId;
    QUrl coverUrl;
    int songCount = 0;
    QDate publishDate;
    MusicPlatform platform = MusicPlatform::Unknown;
};

} // namespace NeriPlayerQt

Q_DECLARE_METATYPE(NeriPlayerQt::Album)

#endif // NERIPLAYERQT_ALBUM_H
