## 1. Types & Parser

- [ ] 1.1 Create `src/api/bilibili/BilibiliTypes.h` with VideoDetail, VideoPage, AudioStream, SearchVideoItem, FavoriteList, etc.
- [ ] 1.2 Create `src/api/bilibili/BilibiliParser.h/.cpp` with JSON parsing for all response types
- [ ] 1.3 Create `tests/api/TestBilibiliParser.cpp` with fixture JSON files

## 2. WBI Signing

- [ ] 2.1 Implement WBI mixin key derivation (permutation table + MD5)
- [ ] 2.2 Implement `signWbiParams()` for request signing
- [ ] 2.3 Implement mixin key caching (10 min TTL)

## 3. Authentication

- [ ] 3.1 Implement `generateQrCode()` calling `/x/passport-login/web/qrcode/generate`
- [ ] 3.2 Implement `checkQrCodeStatus()` calling `/x/passport-login/web/qrcode/poll`
- [ ] 3.3 Implement cookie persistence via `SecureStorage`
- [ ] 3.4 Implement `isAuthenticated()`, `logout()`

## 4. Search & Videos

- [ ] 4.1 Implement `searchVideos()` via WBI-signed `/x/web-interface/wbi/search/type`
- [ ] 4.2 Implement `getVideoDetail()` via WBI-signed `/x/web-interface/wbi/view`
- [ ] 4.3 Implement `getVideoPages()` via `/x/player/pagelist`
- [ ] 4.4 Implement `getVideoStream()` / `getAudioStream()` via `/x/player/wbi/playurl`
- [ ] 4.5 Implement `getHotSearches()` via `/search/hot`

## 5. Favorites

- [ ] 5.1 Implement `getUserFavorites()` via `/x/v3/fav/folder/created/list-all`
- [ ] 5.2 Implement `getFavoriteDetail()` via `/x/v3/fav/resource/list`
- [ ] 5.3 Implement `addVideoToFavorite()` / `removeVideoFromFavorite()` via `/x/v3/fav/resource/deal`

## 6. IMusicPlatformPlugin

- [ ] 6.1 Implement `IMusicPlatformPlugin` interface methods: `platform()`, `search()`, `getSongUrl()`, `login()`, `logout()`, `isAuthenticated()`
- [ ] 6.2 Map Bilibili-specific types to domain `Song`/`SearchResult`

## 7. Integration

- [ ] 7.1 Create `src/api/bilibili/BilibiliClient.h/.cpp` with all methods
- [ ] 7.2 Register `BilibiliClient` in `ServiceLocator`
- [ ] 7.3 Update `CMakeLists.txt` with new files
- [ ] 7.4 Create test fixtures in `tests/fixtures/bilibili/`
- [ ] 7.5 Update `TODO.md` to mark Bilibili API as done
