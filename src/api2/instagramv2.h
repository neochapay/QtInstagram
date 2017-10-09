#ifndef INSTAGRAMV2_H
#define INSTAGRAMV2_H

#include <QObject>
#include <QScopedPointer>
#include <QVariant>

class QFile;

class Instagramv2Private;
class Instagramv2 : public QObject
{
    Q_OBJECT
public:
    explicit Instagramv2(QObject *parent = 0);
    ~Instagramv2();

public Q_SLOTS:
    Q_INVOKABLE void login(bool forse = false);
    Q_INVOKABLE void logout();
//Maked there
    Q_INVOKABLE void setUsername(QString username);
    Q_INVOKABLE void setPassword(QString password);
    Q_INVOKABLE QString getUsernameId();
//End

    Q_INVOKABLE void postImage(QString path, QString caption, QString upload_id = "");
    Q_INVOKABLE void postVideo(QFile *video);

//Unnown source of funct
    Q_INVOKABLE void getPopularFeed();
    Q_INVOKABLE void searchUsername(QString username);

//Image manipulate
    Q_INVOKABLE void rotateImg(QString filename, qreal deg);
    Q_INVOKABLE void cropImg(QString filename, bool squared);
    Q_INVOKABLE void cropImg(QString in_filename, QString out_filename, int topSpace, bool squared);

//Account
    Q_INVOKABLE void setPrivateAccount();
    Q_INVOKABLE void setPublicAccount();
    Q_INVOKABLE void changeProfilePicture(QFile *photo);
    Q_INVOKABLE void removeProfilePicture();
    Q_INVOKABLE void getCurrentUser();
    Q_INVOKABLE void editProfile(QString url, QString phone, QString first_name, QString biography, QString email, bool gender);
    Q_INVOKABLE void checkUsername(QString username);
    Q_INVOKABLE void createAccount(QString username, QString password, QString email);

//Direct
    Q_INVOKABLE void getInbox(QString cursorId="");
    Q_INVOKABLE void getDirectThread(QString threadId, QString cursorId="");
    Q_INVOKABLE void getPendingInbox();
    Q_INVOKABLE void getRecentRecipients();

//Discover
    Q_INVOKABLE void getExploreFeed(QString max_id="", QString isPrefetch="false");

//Hashtag
    Q_INVOKABLE void getTagFeed(QString tag, QString max_id="");

//Media
    Q_INVOKABLE void getInfoMedia(QString mediaId);
    Q_INVOKABLE void editMedia(QString mediaId, QString captionText = "", QString mediaType = "PHOTO");
    Q_INVOKABLE void deleteMedia(QString mediaId, QString mediaType = "PHOTO");
    Q_INVOKABLE void like(QString mediaId, QString module="feed_contextual_post");
    Q_INVOKABLE void unLike(QString mediaId, QString module="feed_contextual_post");
    Q_INVOKABLE void getLikedFeed(QString max_id="");
    Q_INVOKABLE void comment(QString mediaId, QString commentText, QString replyCommentId = "", QString module="coments_feed_timeline");
    Q_INVOKABLE void deleteComment(QString mediaId, QString commentId);
    Q_INVOKABLE void likeComment(QString commentId);
    Q_INVOKABLE void unlikeComment(QString commentId);
    Q_INVOKABLE void getComments(QString mediaId, QString max_id="");
    Q_INVOKABLE void getLikedMedia(QString max_id = "");
    Q_INVOKABLE void getMediaLikers(QString mediaId);

//People
    Q_INVOKABLE void getInfoById(QString userId);
    Q_INVOKABLE void getInfoByName(QString username);
    Q_INVOKABLE void getRecentActivityInbox();
    Q_INVOKABLE void getFollowingRecentActivity();
    Q_INVOKABLE void getFollowing(QString userId, QString max_id = "", QString searchQuery="");
    Q_INVOKABLE void getFollowers(QString userId, QString max_id = "", QString searchQuery="");
    Q_INVOKABLE void getFriendship(QString userId);
    Q_INVOKABLE void getSugestedUser(QString userId);

    Q_INVOKABLE void favorite(QString userId);
    Q_INVOKABLE void unFavorite(QString userId);
    Q_INVOKABLE void follow(QString userId);
    Q_INVOKABLE void unFollow(QString userId);
    Q_INVOKABLE void block(QString userId);
    Q_INVOKABLE void unBlock(QString userId);

    Q_INVOKABLE void searchUser(QString query);

//Story
    Q_INVOKABLE void getReelsTrayFeed();
    Q_INVOKABLE void getUserReelsMediaFeed(QString userId);

//Timeline
    Q_INVOKABLE void getTimelineFeed(QString max_id = "");
    Q_INVOKABLE void getUserFeed(QString userID, QString max_id = "", QString minTimestamp = "");

//Usertag
    Q_INVOKABLE void getUserTags(QString userId, QString max_id="", QString minTimestamp="");
    Q_INVOKABLE void removeSelftag(QString mediaId);


Q_SIGNALS:
    void profileConnected(QVariant answer);
    void profileConnectedFail();
    void doLogout(QVariant answer);
    void error(QString message);

    void imageConfigureDataReady(QVariant answer);

//Unnown source
    void popularFeedDataReady(QVariant answer);
    void searchUsernameDataReady(QVariant answer);

//Refactored

//Account
    void profilePictureDeleted(QVariant answer);
    void setProfilePrivate(QVariant answer);
    void setProfilePublic(QVariant answer);
    void currentUserDataReady(QVariant answer);
    void editDataReady(QVariant answer);
    void usernameCheckDataReady(QVariant answer);
    void createAccountDataReady(QVariant answer);

//Direct
    void inboxDataReady(QVariant answer);
    void directThreadDataReady(QVariant answer);
    void pendingInboxDataReady(QVariant answer);
    void recentRecipientsDataReady(QVariant answer);

//Discover
    void exploreFeedDataReady(QVariant answer);

//Hashtag
    void tagFeedDataReady(QVariant answer);

//Media
    void likeDataReady(QVariant answer);
    void unLikeDataReady(QVariant answer);
    void likedFeedDataReady(QVariant answer);
    void mediaInfoReady(QVariant answer);
    void mediaEdited(QVariant answer);
    void mediaDeleted(QVariant answer);
    void commentPosted(QVariant answer);
    void commentDeleted(QVariant answer);
    void commentLiked(QVariant answer);
    void commentUnliked(QVariant answer);
    void mediaCommentsDataReady(QVariant answer);
    void likedMediaDataReady(QVariant answer);
    void mediaLikersDataReady(QVariant answer);

//People
    void followingDataReady(QVariant answer);
    void followersDataReady(QVariant answer);
    void followDataReady(QVariant answer);
    void unfollowDataReady(QVariant answer);
    void favoriteDataReady(QVariant answer);
    void unFavoriteDataReady(QVariant answer);
    void blockDataReady(QVariant answer);
    void unBlockDataReady(QVariant answer);

    void infoByIdDataReady(QVariant answer);
    void infoByNameDataReady(QVariant answer);

    void recentActivityInboxDataReady(QVariant answer);
    void followingRecentActivityDataReady(QVariant answer);
    void friendshipDataReady(QVariant answer);
    void searchUserDataReady(QVariant answer);
    void suggestedUserDataReady(QVariant answer);

//Story
    void reelsTrayFeedDataReady(QVariant answer);
    void userReelsMediaFeedDataReady(QVariant answer);

//Timeline
    void userFeedDataReady(QVariant answer);
    void timelineFeedDataReady(QVariant answer);

//Usertags
    void userTagsDataReady(QVariant answer);
    void removeSelftagDone(QVariant answer);

private:
    Q_DECLARE_PRIVATE(Instagramv2)
    QScopedPointer<Instagramv2Private> d_ptr;
};

#endif // INSTAGRAMV2_H
