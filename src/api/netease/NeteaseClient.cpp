/// @file NeteaseClient.cpp
/// @brief NetEase Cloud Music API client implementation
/// @date 2024-01-15

#include "api/netease/NeteaseClient.h"

#include "api/common/ApiError.h"
#include "core/crypto/SecureStorage.h"
#include "core/logger/Logger.h"
#include "core/network/HttpClient.h"
#include "api/netease/NeteaseCrypto.h"
#include "api/netease/NeteaseParser.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>

namespace NeriPlayerQt {

static const QUrl DEFAULT_BASE_URL(QStringLiteral("https://music.163.com"));
static const QString COOKIE_STORAGE_KEY = QStringLiteral("netease_cookie");

NeteaseClient::NeteaseClient(HttpClient *httpClient,
                               SecureStorage *storage,
                               QObject *parent)
    : QObject(parent)
    , m_httpClient(httpClient)
    , m_storage(storage)
    , m_baseUrl(DEFAULT_BASE_URL)
{
    Q_ASSERT(m_httpClient);

    // Restore cookies from secure storage
    if (m_storage) {
        m_cookie = m_storage->get(COOKIE_STORAGE_KEY);
        if (!m_cookie.isEmpty()) {
            // Extract CSRF token from cookie
            const QStringList parts = m_cookie.split(QLatin1String("; "));
            for (const QString &part : parts) {
                if (part.startsWith(QLatin1String("__csrf="))) {
                    m_csrfToken = part.mid(7);
                    break;
                }
            }
            m_authenticated = !m_csrfToken.isEmpty();
            if (m_authenticated) {
                Logger::get("api")->info("NeteaseClient: restored session from storage");
            }
        }
    }
}

// ─── Configuration ──────────────────────────────────────────────────────────

void NeteaseClient::setBaseUrl(const QUrl &url)
{
    m_baseUrl = url;
}

// ─── Request Helpers ────────────────────────────────────────────────────────

QCoro::Task<ApiResult<QJsonObject>> NeteaseClient::makeRequest(
    const QString &path,
    const QJsonObject &params)
{
    // Build the encrypted payload
    QJsonObject weapiParams = params;
    if (!m_csrfToken.isEmpty()) {
        weapiParams[QLatin1String("csrf_token")] = m_csrfToken;
    }

    QByteArray payload = QJsonDocument(weapiParams).toJson(QJsonDocument::Compact);
    auto encrypted = NeteaseCrypto::weapiEncrypt(QString::fromUtf8(payload));

    // Build URL-encoded form body manually to ensure + is encoded as %2B
    // QUrlQuery doesn't encode + to %2B, but in form encoding + means space
    QByteArray postData;
    postData += "params=";
    postData += QUrl::toPercentEncoding(encrypted.params);
    postData += "&encSecKey=";
    postData += QUrl::toPercentEncoding(encrypted.encSecKey);

    // Build URL with csrf_token as query parameter (matching Android behavior)
    QUrl url = m_baseUrl.resolved(QUrl(path));
    if (!m_csrfToken.isEmpty()) {
        QUrlQuery query(url.query());
        query.addQueryItem(QStringLiteral("csrf_token"), m_csrfToken);
        url.setQuery(query);
    }

    // Build request with headers
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    request.setRawHeader("Referer", "https://music.163.com");
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Accept-Language", "zh-CN,zh-Hans;q=0.9");
    // Note: do NOT set Accept-Encoding — Qt 6 QNetworkAccessManager doesn't
    // decompress gzip transparently, so we let the server return plain JSON.

    // Inject cookies
    injectCookies(request);

    // Send request with all headers preserved
    auto response = co_await m_httpClient->post(request, postData);

    if (!response.isSuccess()) {
        Logger::get("api")->warn("NeteaseClient: HTTP error at path {}: {} ({})",
                                 path.toStdString(),
                                 response.errorString.toStdString(),
                                 response.statusCode);
        co_return ApiResult<QJsonObject>(ApiError(
            response.statusCode,
            response.errorString));
    }

    // Parse JSON response
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response.body, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        Logger::get("api")->warn("NeteaseClient: JSON parse error at path {}: {} (body: {})",
                                 path.toStdString(),
                                 parseError.errorString().toStdString(),
                                 response.body.left(200).toStdString());
        co_return ApiResult<QJsonObject>(ApiError(
            -1,
            QStringLiteral("Invalid JSON response"),
            parseError.errorString()));
    }

    QJsonObject json = doc.object();

    // Check for API-level errors
    int code = json[QLatin1String("code")].toInt();
    if (code != 200) {
        QString msg = json[QLatin1String("msg")].toString();
        if (msg.isEmpty()) {
            msg = json[QLatin1String("message")].toString();
        }
        Logger::get("api")->warn("NeteaseClient: API error {} at {}: {}",
                                  code, path.toStdString(), msg.toStdString());
        co_return ApiResult<QJsonObject>(ApiError(code, msg));
    }

    co_return ApiResult<QJsonObject>(json);
}

QCoro::Task<ApiResult<QJsonObject>> NeteaseClient::makeUnencryptedRequest(
    const QString &path,
    const QJsonObject &params)
{
    QUrl url = m_baseUrl.resolved(QUrl(path));

    QUrlQuery query;
    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        query.addQueryItem(it.key(), it.value().toVariant().toString());
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    request.setRawHeader("Referer", "https://music.163.com");
    injectCookies(request);

    auto response = co_await m_httpClient->post(request, query.toString(QUrl::FullyEncoded).toUtf8());

    if (!response.isSuccess()) {
        co_return ApiResult<QJsonObject>(ApiError(
            response.statusCode,
            response.errorString));
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response.body, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        co_return ApiResult<QJsonObject>(ApiError(
            -1,
            QStringLiteral("Invalid JSON response"),
            parseError.errorString()));
    }

    QJsonObject json = doc.object();
    int code = json[QLatin1String("code")].toInt();
    if (code != 200) {
        QString msg = json[QLatin1String("msg")].toString();
        if (msg.isEmpty()) {
            msg = json[QLatin1String("message")].toString();
        }
        co_return ApiResult<QJsonObject>(ApiError(code, msg));
    }

    co_return ApiResult<QJsonObject>(json);
}

QCoro::Task<ApiResult<QJsonObject>> NeteaseClient::makeEapiRequest(
    const QString &path,
    const QJsonObject &params)
{
    // Build JSON payload
    QByteArray payload = QJsonDocument(params).toJson(QJsonDocument::Compact);

    // EAPI encrypt
    QString eapiPath = QStringLiteral("/eapi") + path;
    QString encryptedParams = NeteaseCrypto::eapiEncrypt(eapiPath, QString::fromUtf8(payload));

    // Build form body
    QUrlQuery formBody;
    formBody.addQueryItem(QStringLiteral("params"), encryptedParams);
    QByteArray postData = formBody.toString(QUrl::FullyEncoded).toUtf8();

    // Build URL - eapi uses interface.music.163.com
    QUrl url(QStringLiteral("https://interface.music.163.com") + eapiPath);

    // Build request
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/x-www-form-urlencoded"));
    request.setRawHeader("Referer", "https://music.163.com");
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Accept-Language", "zh-CN,zh-Hans;q=0.9");

    // Inject cookies
    injectCookies(request);

    // Send request with all headers preserved
    auto response = co_await m_httpClient->post(request, postData);

    if (!response.isSuccess()) {
        Logger::get("api")->warn("NeteaseClient: EAPI HTTP error at {}: {} ({})",
                                  path.toStdString(),
                                  response.errorString.toStdString(),
                                  response.statusCode);
        co_return ApiResult<QJsonObject>(ApiError(
            response.statusCode,
            response.errorString));
    }

    // Parse JSON response
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response.body, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        Logger::get("api")->warn("NeteaseClient: EAPI JSON parse error: {} (body: {})",
                                 parseError.errorString().toStdString(),
                                 response.body.left(200).toStdString());
        co_return ApiResult<QJsonObject>(ApiError(
            -1,
            QStringLiteral("Invalid JSON response"),
            parseError.errorString()));
    }

    QJsonObject json = doc.object();

    // Check for API-level errors
    int code = json[QLatin1String("code")].toInt();
    if (code != 200) {
        QString msg = json[QLatin1String("msg")].toString();
        if (msg.isEmpty()) {
            msg = json[QLatin1String("message")].toString();
        }
        Logger::get("api")->warn("NeteaseClient: EAPI error {} at {}: {}",
                                  code, path.toStdString(), msg.toStdString());
        co_return ApiResult<QJsonObject>(ApiError(code, msg));
    }

    co_return ApiResult<QJsonObject>(json);
}

void NeteaseClient::injectCookies(QNetworkRequest &request)
{
    QString cookie = m_cookie;
    // Add base cookies if not present
    if (!cookie.contains("os=")) {
        cookie += QStringLiteral("; os=pc; appver=8.10.35");
    }
    if (!cookie.isEmpty()) {
        request.setRawHeader("Cookie", cookie.toUtf8());
    }
}

void NeteaseClient::persistCookies(const QString &cookieString)
{
    m_cookie = cookieString;
    m_authenticated = true;

    // Extract CSRF token
    const QStringList parts = cookieString.split(QLatin1String("; "));
    for (const QString &part : parts) {
        if (part.startsWith(QLatin1String("__csrf="))) {
            m_csrfToken = part.mid(7);
            break;
        }
    }

    if (m_storage) {
        m_storage->set(COOKIE_STORAGE_KEY, cookieString);
    }
}

void NeteaseClient::clearCookies()
{
    m_cookie.clear();
    m_csrfToken.clear();
    m_authenticated = false;

    if (m_storage) {
        m_storage->remove(COOKIE_STORAGE_KEY);
    }
}

// ─── IMusicPlatformPlugin ───────────────────────────────────────────────────

QCoro::Task<ApiResult<SearchResult>> NeteaseClient::search(
    const QString &keyword,
    SearchType type,
    int limit,
    int offset)
{
    // NetEase search type values (not 1-indexed!)
    int neteaseType;
    switch (type) {
    case SearchType::Song:     neteaseType = 1;    break;
    case SearchType::Album:    neteaseType = 10;   break;
    case SearchType::Artist:   neteaseType = 100;  break;
    case SearchType::Playlist: neteaseType = 1000; break;
    default:                   neteaseType = 1;    break;
    }

    QJsonObject params;
    params[QLatin1String("s")] = keyword;
    params[QLatin1String("type")] = QString::number(neteaseType);
    params[QLatin1String("limit")] = QString::number(limit);
    params[QLatin1String("offset")] = QString::number(offset);
    params[QLatin1String("total")] = QStringLiteral("true");

    auto result = co_await makeRequest(QStringLiteral("/weapi/cloudsearch/get/web"), params);
    if (result.isError()) {
        co_return ApiResult<SearchResult>(result.error());
    }

    co_return ApiResult<SearchResult>(
        NeteaseParser::parseSearchResult(result.data(), type));
}

QCoro::Task<ApiResult<Song>> NeteaseClient::getSongDetail(const QString &songId)
{
    // Match Kotlin: c=[{"id":12345}], ids=[12345]
    QString idsArray = QStringLiteral("[%1]").arg(songId);
    QString cArray = QStringLiteral("[{\"id\":%1}]").arg(songId);

    QJsonObject params;
    params[QLatin1String("c")] = cArray;
    params[QLatin1String("ids")] = idsArray;

    auto result = co_await makeRequest(QStringLiteral("/weapi/v3/song/detail"), params);
    if (result.isError()) {
        co_return ApiResult<Song>(result.error());
    }

    co_return ApiResult<Song>(NeteaseParser::parseSong(result.data()));
}

QCoro::Task<ApiResult<SongUrlResult>> NeteaseClient::getSongUrl(
    const QString &songId,
    AudioQuality quality)
{
    // Map AudioQuality to NetEase bitrate
    int br;
    switch (quality) {
    case AudioQuality::Low: br = 128000; break;
    case AudioQuality::Standard: br = 192000; break;
    case AudioQuality::High: br = 320000; break;
    case AudioQuality::Lossless: br = 999000; break;
    }

    QJsonObject params;
    params[QLatin1String("ids")] = QStringLiteral("[%1]").arg(songId);
    params[QLatin1String("br")] = br;

    auto result = co_await makeRequest(QStringLiteral("/weapi/song/enhance/player/url"), params);
    if (result.isError()) {
        co_return ApiResult<SongUrlResult>(result.error());
    }

    co_return ApiResult<SongUrlResult>(NeteaseParser::parseSongUrl(result.data()));
}

QCoro::Task<ApiResult<Lyrics>> NeteaseClient::getLyrics(const QString &songId)
{
    QJsonObject params;
    params[QLatin1String("id")] = songId;
    params[QLatin1String("lv")] = 1; // LRC format
    params[QLatin1String("tv")] = 1; // Translated lyrics

    auto result = co_await makeEapiRequest(QStringLiteral("/song/lyric/v1"), params);
    if (result.isError()) {
        co_return ApiResult<Lyrics>(result.error());
    }

    co_return ApiResult<Lyrics>(NeteaseParser::parseLyrics(result.data()));
}

bool NeteaseClient::isAuthenticated() const
{
    return m_authenticated;
}

QString NeteaseClient::platformName() const
{
    return QStringLiteral("NetEase");
}

// ─── Authentication ────────────────────────────────────────────────────────

QCoro::Task<ApiResult<LoginResult>> NeteaseClient::login(
    const QString &phone,
    const QString &password)
{
    QJsonObject params;
    params[QLatin1String("phone")] = phone;
    params[QLatin1String("password")] = NeteaseCrypto::md5Hex(password);
    params[QLatin1String("countrycode")] = QStringLiteral("86");
    params[QLatin1String("rememberLogin")] = true;

    auto result = co_await makeEapiRequest(QStringLiteral("/w/login/cellphone"), params);
    if (result.isError()) {
        co_return ApiResult<LoginResult>(result.error());
    }

    LoginResult loginResult = NeteaseParser::parseLoginResult(result.data());
    persistCookies(loginResult.cookie);
    co_return ApiResult<LoginResult>(loginResult);
}

QCoro::Task<ApiResult<LoginResult>> NeteaseClient::loginByEmail(
    const QString &email,
    const QString &password)
{
    QJsonObject params;
    params[QLatin1String("email")] = email;
    params[QLatin1String("password")] = password;
    params[QLatin1String("rememberLogin")] = true;

    auto result = co_await makeEapiRequest(QStringLiteral("/w/login"), params);
    if (result.isError()) {
        co_return ApiResult<LoginResult>(result.error());
    }

    LoginResult loginResult = NeteaseParser::parseLoginResult(result.data());
    persistCookies(loginResult.cookie);
    co_return ApiResult<LoginResult>(loginResult);
}

QCoro::Task<ApiResult<VoidResult>> NeteaseClient::logout()
{
    auto result = co_await makeRequest(QStringLiteral("/weapi/logout"));
    if (result.isError()) {
        co_return ApiResult<VoidResult>(result.error());
    }

    clearCookies();
    co_return ApiResult<VoidResult>(VoidResult{});
}

void NeteaseClient::setCookies(const QString &cookieString)
{
    persistCookies(cookieString);
}

QCoro::Task<void> NeteaseClient::ensureWeapiSession()
{
    // Visit homepage to get __csrf cookie (matches Kotlin ensureWeapiSession)
    QUrl url(QStringLiteral("https://music.163.com/"));
    QNetworkRequest request(url);
    request.setRawHeader("Referer", "https://music.163.com");
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
    injectCookies(request);

    co_await m_httpClient->get(request);
}

// ─── Search ────────────────────────────────────────────────────────────────

QCoro::Task<ApiResult<SearchResult>> NeteaseClient::searchSongs(
    const QString &keyword, int limit, int offset)
{
    co_return co_await search(keyword, SearchType::Song, limit, offset);
}

QCoro::Task<ApiResult<SearchResult>> NeteaseClient::searchPlaylists(
    const QString &keyword, int limit, int offset)
{
    co_return co_await search(keyword, SearchType::Playlist, limit, offset);
}

QCoro::Task<ApiResult<SearchResult>> NeteaseClient::searchAlbums(
    const QString &keyword, int limit, int offset)
{
    co_return co_await search(keyword, SearchType::Album, limit, offset);
}

QCoro::Task<ApiResult<SearchResult>> NeteaseClient::searchArtists(
    const QString &keyword, int limit, int offset)
{
    co_return co_await search(keyword, SearchType::Artist, limit, offset);
}

// ─── Playlists ─────────────────────────────────────────────────────────────

QCoro::Task<ApiResult<Playlist>> NeteaseClient::getPlaylistDetail(const QString &playlistId)
{
    QJsonObject params;
    params[QLatin1String("id")] = playlistId;
    params[QLatin1String("n")] = 100000; // Max songs to return

    // Playlist detail uses unencrypted API endpoint
    auto result = co_await makeUnencryptedRequest(QStringLiteral("/api/v6/playlist/detail"), params);
    if (result.isError()) {
        co_return ApiResult<Playlist>(result.error());
    }

    co_return ApiResult<Playlist>(NeteaseParser::parsePlaylistDetail(result.data()));
}

QCoro::Task<ApiResult<QVector<Playlist>>> NeteaseClient::getUserPlaylists(const QString &userId)
{
    QJsonObject params;
    params[QLatin1String("uid")] = userId;
    params[QLatin1String("limit")] = 1000;
    params[QLatin1String("offset")] = 0;

    auto result = co_await makeRequest(QStringLiteral("/weapi/user/playlist"), params);
    if (result.isError()) {
        co_return ApiResult<QVector<Playlist>>(result.error());
    }

    QJsonArray playlistArray = result.data()[QLatin1String("playlist")].toArray();
    QVector<Playlist> playlists;
    playlists.reserve(playlistArray.size());
    for (const auto &item : playlistArray) {
        playlists.append(NeteaseParser::parsePlaylist(item.toObject()));
    }
    co_return ApiResult<QVector<Playlist>>(playlists);
}

QCoro::Task<ApiResult<QVector<Playlist>>> NeteaseClient::getRecommendedPlaylists()
{
    QJsonObject params;
    params[QLatin1String("limit")] = 30;

    // Use the correct endpoint from Kotlin version
    auto result = co_await makeRequest(QStringLiteral("/weapi/personalized/playlist"), params);
    if (result.isError()) {
        co_return ApiResult<QVector<Playlist>>(result.error());
    }

    QJsonArray playlistArray = result.data()[QLatin1String("result")].toArray();
    QVector<Playlist> playlists;
    playlists.reserve(playlistArray.size());
    for (const auto &item : playlistArray) {
        playlists.append(NeteaseParser::parsePlaylist(item.toObject()));
    }
    co_return ApiResult<QVector<Playlist>>(playlists);
}

QCoro::Task<ApiResult<QVector<Playlist>>> NeteaseClient::getHighQualityPlaylists(
    const QString &category, int limit)
{
    QJsonObject params;
    params[QLatin1String("cat")] = category;
    params[QLatin1String("limit")] = limit;
    params[QLatin1String("lasttime")] = 0;
    params[QLatin1String("total")] = true;

    // Use the correct endpoint from Kotlin version
    auto result = co_await makeRequest(QStringLiteral("/weapi/playlist/highquality/list"), params);
    if (result.isError()) {
        co_return ApiResult<QVector<Playlist>>(result.error());
    }

    QJsonArray playlistArray = result.data()[QLatin1String("playlists")].toArray();
    QVector<Playlist> playlists;
    playlists.reserve(playlistArray.size());
    for (const auto &item : playlistArray) {
        playlists.append(NeteaseParser::parsePlaylist(item.toObject()));
    }
    co_return ApiResult<QVector<Playlist>>(playlists);
}

// ─── Albums ────────────────────────────────────────────────────────────────

QCoro::Task<ApiResult<QVector<Song>>> NeteaseClient::getAlbumDetail(const QString &albumId)
{
    QJsonObject params;
    params[QLatin1String("id")] = albumId;

    // Album detail uses interface.music.163.com
    QJsonObject albumParams;
    albumParams[QLatin1String("n")] = 100000;
    albumParams[QLatin1String("s")] = 8;
    auto result = co_await makeRequest(
        QStringLiteral("https://interface.music.163.com/weapi/v1/album/") + albumId,
        albumParams);
    if (result.isError()) {
        co_return ApiResult<QVector<Song>>(result.error());
    }

    QJsonArray songsArray = result.data()[QLatin1String("songs")].toArray();
    co_return ApiResult<QVector<Song>>(NeteaseParser::parseSongs(songsArray));
}

// ─── User ──────────────────────────────────────────────────────────────────

QCoro::Task<ApiResult<VoidResult>> NeteaseClient::likeSong(const QString &songId)
{
    QJsonObject params;
    params[QLatin1String("trackId")] = songId;
    params[QLatin1String("like")] = QStringLiteral("true");

    auto result = co_await makeRequest(QStringLiteral("/weapi/song/like"), params);
    if (result.isError()) {
        co_return ApiResult<VoidResult>(result.error());
    }

    co_return ApiResult<VoidResult>(VoidResult{});
}

QCoro::Task<ApiResult<VoidResult>> NeteaseClient::unlikeSong(const QString &songId)
{
    QJsonObject params;
    params[QLatin1String("trackId")] = songId;
    params[QLatin1String("like")] = QStringLiteral("false");

    auto result = co_await makeRequest(QStringLiteral("/weapi/song/like"), params);
    if (result.isError()) {
        co_return ApiResult<VoidResult>(result.error());
    }

    co_return ApiResult<VoidResult>(VoidResult{});
}

QCoro::Task<ApiResult<QStringList>> NeteaseClient::getLikedSongIds(const QString &userId)
{
    QJsonObject params;
    params[QLatin1String("uid")] = userId;

    auto result = co_await makeRequest(QStringLiteral("/weapi/song/like/get"), params);
    if (result.isError()) {
        co_return ApiResult<QStringList>(result.error());
    }

    QJsonArray idsArray = result.data()[QLatin1String("ids")].toArray();
    QStringList ids;
    ids.reserve(idsArray.size());
    for (const auto &id : idsArray) {
        ids.append(QString::number(id.toVariant().toLongLong()));
    }
    co_return ApiResult<QStringList>(ids);
}

QCoro::Task<ApiResult<QJsonObject>> NeteaseClient::getCurrentUserAccount()
{
    auto result = co_await makeRequest(QStringLiteral("/weapi/w/nuser/account/get"));
    if (result.isError()) {
        co_return ApiResult<QJsonObject>(result.error());
    }

    co_return ApiResult<QJsonObject>(result.data());
}

// ─── Download ─────────────────────────────────────────────────────────────

QCoro::Task<ApiResult<QJsonObject>> NeteaseClient::getSongDownloadUrl(
    const QString &songId, const QString &level)
{
    QJsonObject params;
    params[QLatin1String("ids")] = QStringLiteral("[%1]").arg(songId);
    params[QLatin1String("level")] = level;
    params[QLatin1String("encodeType")] = QStringLiteral("flac");

    auto result = co_await makeEapiRequest(QStringLiteral("/song/enhance/player/url/v1"), params);
    if (result.isError()) {
        co_return ApiResult<QJsonObject>(result.error());
    }

    co_return ApiResult<QJsonObject>(result.data());
}

// ─── High Quality Playlists ───────────────────────────────────────────────

QCoro::Task<ApiResult<QStringList>> NeteaseClient::getHighQualityTags()
{
    QJsonObject params;

    auto result = co_await makeRequest(QStringLiteral("/weapi/playlist/highquality/tags"), params);
    if (result.isError()) {
        co_return ApiResult<QStringList>(result.error());
    }

    // Parse tags from response
    QStringList tags;
    QJsonObject data = result.data()[QLatin1String("data")].toObject();
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        tags.append(it.key());
    }
    co_return ApiResult<QStringList>(tags);
}

// ─── User Collections ─────────────────────────────────────────────────────

QCoro::Task<ApiResult<QJsonObject>> NeteaseClient::getUserAlbums(
    const QString &userId, int limit, int offset)
{
    QJsonObject params;
    params[QLatin1String("userId")] = userId;
    params[QLatin1String("offset")] = offset;
    params[QLatin1String("limit")] = limit;
    params[QLatin1String("pageType")] = QStringLiteral("3");
    params[QLatin1String("needRcmd")] = QStringLiteral("0");
    params[QLatin1String("isVistor")] = QStringLiteral("false");
    params[QLatin1String("includeStarPodcast")] = QStringLiteral("true");

    auto result = co_await makeEapiRequest(QStringLiteral("/mine/rn/resource/list"), params);
    if (result.isError()) {
        co_return ApiResult<QJsonObject>(result.error());
    }

    co_return ApiResult<QJsonObject>(result.data());
}

QCoro::Task<ApiResult<QJsonObject>> NeteaseClient::getUserDjRadios(
    const QString &userId, int limit, int offset)
{
    QJsonObject params;
    params[QLatin1String("uid")] = userId;
    params[QLatin1String("offset")] = offset;
    params[QLatin1String("limit")] = limit;

    auto result = co_await makeRequest(QStringLiteral("/weapi/user/djradio/get/subed"), params);
    if (result.isError()) {
        co_return ApiResult<QJsonObject>(result.error());
    }

    co_return ApiResult<QJsonObject>(result.data());
}

} // namespace NeriPlayerQt
