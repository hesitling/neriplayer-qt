/// @file Song.h
/// @brief Song domain model

#ifndef QERIPLAYERQT_SONG_H
#define QERIPLAYERQT_SONG_H

#include "domain/Enums.h"
#include "domain/Lyrics.h"

#include <QMetaType>
#include <QString>
#include <QUrl>
#include <QVariantMap>

namespace QeriPlayerQt {

/**
 * @brief Represents a song across all platforms
 *
 * Aligned with Android QeriPlayer's SongItem model.
 * Includes customization fields (custom/original) for user overrides,
 * lyric matching fields, local file support, and platform-specific identifiers.
 *
 * Q_GADGET enables QML property access via Q_PROPERTY(READ).
 * Only fields commonly needed by QML UI are exposed as properties.
 */
struct Song {
    Q_GADGET
    Q_PROPERTY(QString id READ getId CONSTANT)
    Q_PROPERTY(QString name READ getName CONSTANT)
    Q_PROPERTY(QString artist READ getArtist CONSTANT)
    Q_PROPERTY(QString album READ getAlbum CONSTANT)
    Q_PROPERTY(qint64 durationMs READ getDurationMs CONSTANT)
    Q_PROPERTY(QUrl coverUrl READ getCoverUrl CONSTANT)
    Q_PROPERTY(MusicPlatform platform READ getPlatform CONSTANT)

    // QML property getters
    QString getId() const
    {
        return id;
    }
    QString getName() const
    {
        return name;
    }
    QString getArtist() const
    {
        return artist;
    }
    QString getAlbum() const
    {
        return album;
    }
    qint64 getDurationMs() const
    {
        return durationMs;
    }
    QUrl getCoverUrl() const
    {
        return coverUrl;
    }
    MusicPlatform getPlatform() const
    {
        return platform;
    }

public:
    // Core identity
    QString id;
    QString name;
    QString artist;
    QString album;
    QString albumId;
    qint64 durationMs = 0; ///< Duration in milliseconds
    QUrl coverUrl;
    QUrl mediaUri;
    MusicPlatform platform = MusicPlatform::Unknown;

    // Lyric matching
    QString matchedLyric;
    QString matchedTranslatedLyric;
    MusicPlatform matchedLyricSource = MusicPlatform::Unknown;
    QString matchedSongId;
    qint64 userLyricOffsetMs = 0;

    Lyrics lyrics; ///< Parsed lyrics with word-level timing

    // User customizations (overrides)
    QString customCoverUrl;
    QString customName;
    QString customArtist;

    // Original values (before user edits)
    QString originalName;
    QString originalArtist;
    QString originalCoverUrl;
    QString originalLyric;
    QString originalTranslatedLyric;

    // Local file support
    QString localFileName;
    QString localFilePath;

    // Platform-specific identifiers
    QString channelId;
    QString audioId;
    QString subAudioId;
    QString playlistContextId;
    QString streamUrl;

    // Arbitrary platform-specific data
    QVariantMap extra;
};

} // namespace QeriPlayerQt

Q_DECLARE_METATYPE(QeriPlayerQt::Song)

#endif // QERIPLAYERQT_SONG_H
