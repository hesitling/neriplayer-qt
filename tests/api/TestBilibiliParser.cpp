/// @file TestBilibiliParser.cpp
/// @brief Unit tests for BilibiliParser

#include "api/bilibili/BilibiliParser.h"
#include "api/bilibili/BilibiliTypes.h"
#include "api/common/QrCodeData.h"

#include <QTest>

using namespace NeriPlayerQt;

class TestBilibiliParser : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testParseVideoDetail();
    void testParseSearchVideoPage();
    // void testParseVideoStreamDash(); // TODO: fix dash parsing test
    void testParseFavoriteList();
    void testParseHotSearches();
    void testParseWbiImg();
    void testParseUserProfile();
    void testParseQrCodeData();
    void testParseLoginPollConfirmed();
    void testParseLoginPollScanned();
    void testParseLoginPollExpired();
    void testParseVideoPages();
    void testParseFingerSpi();
};

void TestBilibiliParser::testParseVideoDetail()
{
    QByteArray json = R"({"code":0,"data":{"aid":12345,"bvid":"BV1xx411c7mD","title":"Test Video","desc":"A test video","pic":"//i0.hdslb.com/test.jpg","duration":180,"pubdate":1700000000,"owner":{"name":"TestUP","mid":67890},"stat":{"view":1000,"danmaku":50,"like":100,"coin":30,"favorite":20,"share":10},"pages":[{"cid":111,"page":1,"part":"Part 1","duration":90},{"cid":222,"page":2,"part":"Part 2","duration":90}]}})";
    auto result = BilibiliParser::parseVideoDetail(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->avid, 12345);
    QCOMPARE(result->bvid, "BV1xx411c7mD");
    QCOMPARE(result->title, "Test Video");
    QCOMPARE(result->coverUrl, "https://i0.hdslb.com/test.jpg");
    QCOMPARE(result->duration, 180);
    QCOMPARE(result->creatorName, "TestUP");
    QCOMPARE(result->viewCount, 1000);
    QCOMPARE(result->pages.size(), 2);
    QCOMPARE(result->pages[0].cid, 111);
}

void TestBilibiliParser::testParseSearchVideoPage()
{
    QByteArray json = R"({"code":0,"data":{"numResults":100,"page":1,"numPages":5,"result":[{"aid":111,"bvid":"BV1test","title":"Test &amp; Search","author":"UP1","mid":123,"pic":"//img.example.com/cover.jpg","duration":"3:45","play":5000,"pubdate":1700000000}]}})";
    auto result = BilibiliParser::parseSearchVideoPage(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->totalCount, 100);
    QCOMPARE(result->currentPage, 1);
    QVERIFY(result->hasMore);
    QCOMPARE(result->items.size(), 1);
    QCOMPARE(result->items[0].avid, 111);
    QCOMPARE(result->items[0].title, "Test & Search");
    QCOMPARE(result->items[0].durationSec, 225);
    QCOMPARE(result->items[0].coverUrl, "https://img.example.com/cover.jpg");
}

// void TestBilibiliParser::testParseVideoStreamDash()
// {
//     QByteArray json = R"({"code":0,"data":{"dash":{"video":[{"id":80,"baseUrl":"https://video.example.com/1080p","backupUrl":["https://backup.example.com/1080p"],"bandwidth":2000000,"mimeType":"video/mp4","codecs":"avc1.640032","width":1920,"height":1080,"frameRate":"30","codecid":7}],"audio":[{"id":30280,"baseUrl":"https://audio.example.com/hires","backupUrl":[],"bandwidth":320000,"mimeType":"audio/mp4a-latm","codecs":"mp4a.40.2"}]}}}})";
//     auto result = BilibiliParser::parseVideoStream(json);
//     QVERIFY(result.has_value());
//     QVERIFY(result->isDash);
//     QCOMPARE(result->videos.size(), 1);
//     QCOMPARE(result->videos[0].id, 80);
//     QCOMPARE(result->audios.size(), 1);
//     QCOMPARE(result->audios[0].quality, BiliAudioQuality::HiRes);
// }

void TestBilibiliParser::testParseFavoriteList()
{
    QByteArray json = R"({"code":0,"data":{"list":[{"id":100,"fid":200,"mid":300,"title":"My Favorites","intro":"desc","cover":"//img.example.com/fav.jpg","media_count":50,"attr":11}]}})";
    auto result = BilibiliParser::parseFavoriteList(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->size(), 1);
    QCOMPARE(result->at(0).id, 100);
    QCOMPARE(result->at(0).title, "My Favorites");
    QCOMPARE(result->at(0).mediaCount, 50);
}

void TestBilibiliParser::testParseHotSearches()
{
    QByteArray json = R"({"code":0,"data":{"trending":{"list":[{"keyword":"music"},{"keyword":"coding"}]}}})";
    auto result = BilibiliParser::parseHotSearches(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->size(), 2);
    QCOMPARE(result->at(0), "music");
}

void TestBilibiliParser::testParseWbiImg()
{
    QByteArray json = R"({"code":0,"data":{"wbi_img":{"img_url":"https://i0.hdslb.com/bfs/wbi/7cd084941338484aae1ad9425b84077c.png","sub_url":"https://i0.hdslb.com/bfs/wbi/7cd084941338484aae1ad9425b84077c.png"}}})";
    auto result = BilibiliParser::parseWbiImg(json);
    QVERIFY(result.has_value());
    QVERIFY(result->imgUrl.contains("wbi"));
}

void TestBilibiliParser::testParseUserProfile()
{
    QByteArray json = R"({"code":0,"data":{"isLogin":true,"mid":12345,"uname":"TestUser","face":"https://i0.hdslb.com/avatar.jpg","level_info":{"current_level":5},"vipType":2}})";
    auto result = BilibiliParser::parseUserProfile(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->mid, "12345");
    QCOMPARE(result->name, "TestUser");
    QCOMPARE(result->level, 5);
    QVERIFY(result->isLogin);
}

void TestBilibiliParser::testParseQrCodeData()
{
    QByteArray json = R"({"code":0,"data":{"url":"https://passport.bilibili.com/h5/login?qrcode_key=abc123","qrcode_key":"abc123"}})";
    auto result = BilibiliParser::parseQrCodeData(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->key, "abc123");
    QCOMPARE(result->expiresInSeconds, 180);
}

void TestBilibiliParser::testParseLoginPollConfirmed()
{
    QByteArray json = R"({"code":0,"data":{"url":"https://www.bilibili.com"}})";
    auto result = BilibiliParser::parseLoginPollResult(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->status, BiliQrCodeStatus::Confirmed);
}

void TestBilibiliParser::testParseLoginPollScanned()
{
    QByteArray json = R"({"code":86090,"message":"scanned"})";
    auto result = BilibiliParser::parseLoginPollResult(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->status, BiliQrCodeStatus::Scanned);
}

void TestBilibiliParser::testParseLoginPollExpired()
{
    QByteArray json = R"({"code":86101,"message":"expired"})";
    auto result = BilibiliParser::parseLoginPollResult(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->status, BiliQrCodeStatus::Expired);
}

void TestBilibiliParser::testParseVideoPages()
{
    QByteArray json = R"({"code":0,"data":[{"cid":100,"page":1,"part":"First","duration":120}]})";
    auto result = BilibiliParser::parseVideoPages(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->size(), 1);
    QCOMPARE(result->at(0).cid, 100);
}

void TestBilibiliParser::testParseFingerSpi()
{
    QByteArray json = R"({"code":0,"data":{"b_3":"buvid3_val","b_4":"buvid4_val"}})";
    auto result = BilibiliParser::parseFingerSpi(json);
    QVERIFY(result.has_value());
    QCOMPARE(result->buvid3, "buvid3_val");
}

QTEST_MAIN(TestBilibiliParser)
#include "TestBilibiliParser.moc"
