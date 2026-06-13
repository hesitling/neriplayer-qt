/// @file Enums.h
/// @brief Domain enumerations shared across all layers
/// @date 2024-01-15

#ifndef NERIPLAYERQT_ENUMS_H
#define NERIPLAYERQT_ENUMS_H

#include <QMetaType>

#include <cstdint>

namespace NeriPlayerQt {

/**
 * @brief Supported music platforms
 */
enum class MusicPlatform : std::uint8_t { Unknown = 0, NetEase = 1, Bilibili = 2, YouTube = 3, QQMusic = 4 };

/**
 * @brief Search target types
 */
enum class SearchType : std::uint8_t { Song = 0, Album = 1, Artist = 2, Playlist = 3, All = 4 };

/**
 * @brief Playback state
 */
enum class PlaybackState : std::uint8_t { Stopped = 0, Playing = 1, Paused = 2, Loading = 3, Error = 4 };

/**
 * @brief Repeat mode
 */
enum class RepeatMode : std::uint8_t { Off = 0, One = 1, All = 2 };

/**
 * @brief Audio quality levels
 */
enum class AudioQuality : std::uint8_t { Low = 0, Standard = 1, High = 2, Lossless = 3 };

} // namespace NeriPlayerQt

Q_DECLARE_METATYPE(NeriPlayerQt::MusicPlatform)
Q_DECLARE_METATYPE(NeriPlayerQt::SearchType)
Q_DECLARE_METATYPE(NeriPlayerQt::PlaybackState)
Q_DECLARE_METATYPE(NeriPlayerQt::RepeatMode)
Q_DECLARE_METATYPE(NeriPlayerQt::AudioQuality)

#endif // NERIPLAYERQT_ENUMS_H
