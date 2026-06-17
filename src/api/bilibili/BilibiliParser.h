/// @file BilibiliParser.h
/// @brief JSON parsers for Bilibili API responses

#ifndef NERIPLAYERQT_BILIBILIPARSER_H
#define NERIPLAYERQT_BILIBILIPARSER_H

#include "api/bilibili/BilibiliTypes.h"
#include "api/common/QrCodeData.h"

#include <QByteArray>
#include <optional>

namespace NeriPlayerQt {

class BilibiliParser {
public:
    /// Parse user profile from /x/web-interface/nav
    static std::optional<BiliUserProfile> parseUserProfile(const QByteArray &json);

    /// Parse QR code data from /x/passport-login/web/qrcode/generate
    static std::optional<QrCodeData> parseQrCodeData(const QByteArray &json);

    /// Parse QR code polling result from /x/passport-login/web/qrcode/poll
    static std::optional<BiliLoginPollResult> parseLoginPollResult(const QByteArray &json);

    /// Parse video search from /x/web-interface/wbi/search/type
    static std::optional<BiliSearchVideoPage> parseSearchVideoPage(const QByteArray &json);

    /// Parse video detail from /x/web-interface/wbi/view
    static std::optional<BiliVideoDetail> parseVideoDetail(const QByteArray &json);

    /// Parse video pages from /x/player/pagelist
    static std::optional<QList<BiliVideoPage>> parseVideoPages(const QByteArray &json);

    /// Parse streams from /x/player/wbi/playurl
    static std::optional<BiliVideoStream> parseVideoStream(const QByteArray &json);

    /// Parse favorite folder list from /x/v3/fav/folder/created/list-all
    static std::optional<QList<BiliFavoriteList>> parseFavoriteList(const QByteArray &json);

    /// Parse favorite detail from /x/v3/fav/resource/list
    static std::optional<BiliFavoriteDetail> parseFavoriteDetail(
        const QByteArray &json, const BiliFavoriteList &folderInfo);

    /// Parse hot searches from /search/hot
    static std::optional<QStringList> parseHotSearches(const QByteArray &json);

    /// Parse WBI img URLs from nav response
    struct WbiImgInfo {
        QString imgUrl;
        QString subUrl;
    };
    static std::optional<WbiImgInfo> parseWbiImg(const QByteArray &json);

    /// Parse anonymous fingerprint from /x/frontend/finger/spi
    struct FingerSpi {
        QString buvid3;
        QString buvid4;
    };
    static std::optional<FingerSpi> parseFingerSpi(const QByteArray &json);

private:
    static QString stripHtmlTags(const QString &html);
};

} // namespace NeriPlayerQt

#endif // NERIPLAYERQT_BILIBILIPARSER_H
