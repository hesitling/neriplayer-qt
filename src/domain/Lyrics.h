/// @file Lyrics.h
/// @brief Lyrics domain model
/// @date 2024-01-15

#ifndef NERIPLAYERQT_LYRICS_H
#define NERIPLAYERQT_LYRICS_H

#include <QMetaType>
#include <QString>
#include <QVector>

namespace NeriPlayerQt {

/**
 * @brief A single timed lyric line
 */
struct LyricLine {
    qint64 timestamp = 0;  ///< Milliseconds from song start
    QString text;
};

/**
 * @brief Full lyrics for a song
 */
struct Lyrics {
    QString rawText;
    QVector<LyricLine> lines;  ///< Sorted by timestamp ascending
};

} // namespace NeriPlayerQt

Q_DECLARE_METATYPE(NeriPlayerQt::LyricLine)
Q_DECLARE_METATYPE(NeriPlayerQt::Lyrics)

#endif // NERIPLAYERQT_LYRICS_H
