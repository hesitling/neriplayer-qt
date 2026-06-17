/// @file BilibiliTypes.h
/// @brief Bilibili-specific data types

#ifndef NERIPLAYERQT_BILIBILITYPES_H
#define NERIPLAYERQT_BILIBILITYPES_H

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QString>

namespace NeriPlayerQt {

/// Bilibili video quality levels
enum class BiliVideoQuality : int {
    Q240P = 6,
    Q360P = 16,
    Q480P = 32,
    Q720P = 64,
    Q720P60 = 74,
    Q1080P = 80,
    Q1080P60 = 112,
    Q4K = 120
};

/// Bilibili audio quality levels
enum class BiliAudioQuality : std::uint8_t { Low = 0, Medium = 1, High = 2, Lossless = 3, HiRes = 4, Dolby = 5 };

/// QR code scan status
enum class BiliQrCodeStatus : std::uint8_t { Expired = 0, Scanned = 1, Confirmed = 2 };

/// QR code polling result
struct BiliLoginPollResult {
    BiliQrCodeStatus status = BiliQrCodeStatus::Expired;
    QString redirectUrl;
    QString message;
};

/// Bilibili user profile
struct BiliUserProfile {
    QString mid;
    QString name;
    QString avatarUrl;
    int level = 0;
    int vipType = 0;
    bool isLogin = false;
};

/// Video page (part of a multi-part video)
struct BiliVideoPage {
    int cid = 0;
    int page = 0;
    QString title;
    int duration = 0;
};

/// Video detail
struct BiliVideoDetail {
    int avid = 0;
    QString bvid;
    QString title;
    QString description;
    QString coverUrl;
    QString creatorName;
    QString creatorMid;
    int duration = 0;
    QDateTime createdAt;
    int viewCount = 0;
    int danmakuCount = 0;
    int likeCount = 0;
    int coinCount = 0;
    int favoriteCount = 0;
    int shareCount = 0;
    QList<BiliVideoPage> pages;
};

/// DASH video stream
struct BiliDashStream {
    int id = 0;
    QString baseUrl;
    QStringList backupUrls;
    int bandwidth = 0;
    QString mimeType;
    QString codecs;
    int width = 0;
    int height = 0;
    QString frameRate;
    int codecid = 0;
};

/// DASH audio stream
struct BiliAudioStream {
    int id = 0;
    QString baseUrl;
    QStringList backupUrls;
    int bandwidth = 0;
    QString mimeType;
    QString codecs;
    BiliAudioQuality quality = BiliAudioQuality::Medium;
};

/// Video stream info (DASH or progressive)
struct BiliVideoStream {
    QList<BiliDashStream> videos;
    QList<BiliAudioStream> audios;
    QStringList durl; // Progressive MP4 fallback
    bool isDash = false;
};

/// Search video result item
struct BiliSearchVideoItem {
    int avid = 0;
    QString bvid;
    QString title;
    QString author;
    QString authorMid;
    QString coverUrl;
    int durationSec = 0;
    int playCount = 0;
    QDateTime pubDate;
};

/// Paginated search result
struct BiliSearchVideoPage {
    QList<BiliSearchVideoItem> items;
    int totalCount = 0;
    int currentPage = 0;
    int numPages = 0;
    bool hasMore = false;
};

/// Favorite folder
struct BiliFavoriteList {
    int id = 0; // media_id
    int fid = 0;
    int mid = 0;
    QString title;
    QString description;
    QString coverUrl;
    int mediaCount = 0;
    int itemType = 0; // 11=favorite, 21=collection
};

/// Favorite resource (video in a folder)
struct BiliFavoriteResource {
    int type = 0; // 2=video, 12=audio
    int id = 0;
    QString bvid;
    QString title;
    QString coverUrl;
    QString intro;
    int durationSec = 0;
    QString upperMid;
    QString upperName;
    int playCount = 0;
    int danmakuCount = 0;
    qint64 favTime = 0;
};

/// Favorite folder detail with contents
struct BiliFavoriteDetail {
    BiliFavoriteList info;
    QList<BiliFavoriteResource> items;
    int totalCount = 0;
    bool hasMore = false;
};

/// User stats
struct BiliUserStat {
    int following = 0;
    int follower = 0;
    int like = 0;
};

} // namespace NeriPlayerQt

Q_DECLARE_METATYPE(NeriPlayerQt::BiliUserProfile)
Q_DECLARE_METATYPE(NeriPlayerQt::BiliVideoDetail)
Q_DECLARE_METATYPE(NeriPlayerQt::BiliVideoPage)
Q_DECLARE_METATYPE(NeriPlayerQt::BiliSearchVideoItem)
Q_DECLARE_METATYPE(NeriPlayerQt::BiliSearchVideoPage)
Q_DECLARE_METATYPE(NeriPlayerQt::BiliFavoriteList)
Q_DECLARE_METATYPE(NeriPlayerQt::BiliFavoriteDetail)

#endif // NERIPLAYERQT_BILIBILITYPES_H
