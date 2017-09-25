#ifndef INSTAGRAMV2_H
#define INSTAGRAMV2_H

#include <QObject>
#include <QDir>
#include <QVariant>

class Instagramv2 : public QObject
{
    Q_OBJECT
public:
    explicit Instagramv2(QObject *parent = 0);

public slots:
    Q_INVOKABLE void login(bool forse = false);
    Q_INVOKABLE void logout();
//Maked there
    Q_INVOKABLE void setUsername(QString username){this->m_username = username;}
    Q_INVOKABLE void setPassword(QString password){this->m_password = password;}
    Q_INVOKABLE QString getUsernameId(){return this->m_username_id;}
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


private:
    QString EXPERIMENTS = "ig_promote_reach_objective_fix_universe,ig_android_universe_video_production,"
                          "ig_search_client_h1_2017_holdout,ig_android_live_follow_from_comments_universe,"
                          "ig_android_carousel_non_square_creation,ig_android_live_analytics,"
                          "ig_android_follow_all_dialog_confirmation_copy,ig_android_stories_server_coverframe,"
                          "ig_android_video_captions_universe,ig_android_offline_location_feed,"
                          "ig_android_direct_inbox_retry_seen_state,ig_android_ontact_invite_universe,"
                          "ig_android_live_broadcast_blacklist,ig_android_insta_video_reconnect_viewers,"
                          "ig_android_ad_async_ads_universe,ig_android_search_clear_layout_universe,"
                          "ig_android_shopping_reporting,ig_android_stories_surface_universe,"
                          "ig_android_verified_comments_universe,ig_android_preload_media_ahead_in_current_reel,"
                          "android_instagram_prefetch_suggestions_universe,ig_android_reel_viewer_fetch_missing_reels_universe,"
                          "ig_android_direct_search_share_sheet_universe,ig_android_business_promote_tooltip,"
                          "ig_android_direct_blue_tab,ig_android_async_network_tweak_universe,"
                          "ig_android_elevate_main_thread_priority_universe,ig_android_stories_gallery_nux,"
                          "ig_android_instavideo_remove_nux_comments,ig_video_copyright_whitelist,"
                          "ig_react_native_inline_insights_with_relay,ig_android_direct_thread_message_animation,"
                          "ig_android_draw_rainbow_client_universe,ig_android_direct_link_style,"
                          "ig_android_live_heart_enhancements_universe,ig_android_rtc_reshare,"
                          "ig_android_preload_item_count_in_reel_viewer_buffer,ig_android_users_bootstrap_service,"
                          "ig_android_auto_retry_post_mode,ig_android_shopping,"
                          "ig_android_main_feed_seen_state_dont_send_info_on_tail_load,ig_fbns_preload_default,"
                          "ig_android_gesture_dismiss_reel_viewer,ig_android_tool_tip,ig_android_ad_logger_funnel_logging_universe,"
                          "ig_android_gallery_grid_column_count_universe,ig_android_business_new_ads_payment_universe,"
                          "ig_android_direct_links,ig_android_audience_control,ig_android_live_encore_consumption_settings_universe,"
                          "ig_perf_android_holdout,ig_android_cache_contact_import_list,ig_android_links_receivers,"
                          "ig_android_ad_impression_backtest,ig_android_list_redesign,ig_android_stories_separate_overlay_creation,"
                          "ig_android_stop_video_recording_fix_universe,ig_android_render_video_segmentation,"
                          "ig_android_live_encore_reel_chaining_universe,ig_android_sync_on_background_enhanced_10_25,"
                          "ig_android_immersive_viewer,ig_android_mqtt_skywalker,ig_fbns_push,ig_android_ad_watchmore_overlay_universe,"
                          "ig_android_react_native_universe,ig_android_profile_tabs_redesign_universe,ig_android_live_consumption_abr,"
                          "ig_android_story_viewer_social_context,ig_android_hide_post_in_feed,ig_android_video_loopcount_int,"
                          "ig_android_enable_main_feed_reel_tray_preloading,ig_android_camera_upsell_dialog,"
                          "ig_android_ad_watchbrowse_universe,ig_android_internal_research_settings,"
                          "ig_android_search_people_tag_universe,ig_android_react_native_ota,ig_android_enable_concurrent_request,"
                          "ig_android_react_native_stories_grid_view,ig_android_business_stories_inline_insights,"
                          "ig_android_log_mediacodec_info,ig_android_direct_expiring_media_loading_errors,ig_video_use_sve_universe,"
                          "ig_android_cold_start_feed_request,ig_android_enable_zero_rating,ig_android_reverse_audio,ig_android_bran"
                          "ded_content_three_line_ui_universe,ig_android_live_encore_production_universe,ig_stories_music_sticker,ig"
                          "_android_stories_teach_gallery_location,ig_android_http_stack_experiment_2017,ig_android_stories_device_t"
                          "ilt,ig_android_pending_request_search_bar,ig_android_fb_topsearch_sgp_fork_request,ig_android_seen_state_"
                          "with_view_info,ig_android_animation_perf_reporter_timeout,ig_android_new_block_flow,ig_android_story_tray"
                          "_title_play_all_v2,ig_android_direct_address_links,ig_android_stories_archive_universe,ig_android_save_co"
                          "llections_cover_photo,ig_android_live_webrtc_livewith_production,ig_android_sign_video_url,ig_android_sto"
                          "ries_video_prefetch_kb,ig_android_stories_create_flow_favorites_tooltip,ig_android_live_stop_broadcast_on_"
                          "404,ig_android_live_viewer_invite_universe,ig_android_promotion_feedback_channel,ig_android_render_iframe_"
                          "interval,ig_android_accessibility_logging_universe,ig_android_camera_shortcut_universe,ig_android_use_one_"
                          "cookie_store_per_user_override,ig_profile_holdout_2017_universe,ig_android_stories_server_brushes,ig_andro"
                          "id_ad_media_url_logging_universe,ig_android_shopping_tag_nux_text_universe,ig_android_comments_single_repl"
                          "y_universe,ig_android_stories_video_loading_spinner_improvements,ig_android_collections_cache,ig_android_c"
                          "omment_api_spam_universe,ig_android_facebook_twitter_profile_photos,ig_android_shopping_tag_creation_unive"
                          "rse,ig_story_camera_reverse_video_experiment,ig_android_direct_bump_selected_recipients,ig_android_ad_cta_"
                          "haptic_feedback_universe,ig_android_vertical_share_sheet_experiment,ig_android_family_bridge_share,ig_andr"
                          "oid_search,ig_android_insta_video_consumption_titles,ig_android_stories_gallery_preview_button,ig_android_"
                          "fb_auth_education,ig_android_camera_universe,ig_android_me_only_universe,ig_android_instavideo_audio_only_"
                          "mode,ig_android_user_profile_chaining_icon,ig_android_live_video_reactions_consumption_universe,ig_android"
                          "_stories_hashtag_text,ig_android_post_live_badge_universe,ig_android_swipe_fragment_container,ig_android_s"
                          "earch_users_universe,ig_android_live_save_to_camera_roll_universe,ig_creation_growth_holdout,ig_android_st"
                          "icker_region_tracking,ig_android_unified_inbox,ig_android_live_new_watch_time,ig_android_offline_main_feed"
                          "_10_11,ig_import_biz_contact_to_page,ig_android_live_encore_consumption_universe,ig_android_experimental_f"
                          "ilters,ig_android_search_client_matching_2,ig_android_react_native_inline_insights_v2,ig_android_business_"
                          "conversion_value_prop_v2,ig_android_redirect_to_low_latency_universe,ig_android_ad_show_new_awr_universe,i"
                          "g_family_bridges_holdout_universe,ig_android_background_explore_fetch,ig_android_following_follower_social"
                          "_context,ig_android_video_keep_screen_on,ig_android_ad_leadgen_relay_modern,ig_android_profile_photo_as_me"
                          "dia,ig_android_insta_video_consumption_infra,ig_android_ad_watchlead_universe,ig_android_direct_prefetch_d"
                          "irect_story_json,ig_android_shopping_react_native,ig_android_top_live_profile_pics_universe,ig_android_dir"
                          "ect_phone_number_links,ig_android_stories_weblink_creation,ig_android_direct_search_new_thread_universe,ig"
                          "_android_histogram_reporter,ig_android_direct_on_profile_universe,ig_android_network_cancellation,ig_andro"
                          "id_background_reel_fetch,ig_android_react_native_insights,ig_android_insta_video_audio_encoder,ig_android_"
                          "family_bridge_bookmarks,ig_android_data_usage_network_layer,ig_android_universal_instagram_deep_links,ig_a"
                          "ndroid_dash_for_vod_universe,ig_android_modular_tab_discover_people_redesign,ig_android_mas_sticker_upsell"
                          "_dialog_universe,ig_android_ad_add_per_event_counter_to_logging_event,ig_android_sticky_header_top_chrome_"
                          "optimization,ig_android_rtl,ig_android_biz_conversion_page_pre_select,ig_android_promote_from_profile_butt"
                          "on,ig_android_live_broadcaster_invite_universe,ig_android_share_spinner,ig_android_text_action,ig_android_"
                          "own_reel_title_universe,ig_promotions_unit_in_insights_landing_page,ig_android_business_settings_header_un"
                          "iv,ig_android_save_longpress_tooltip,ig_android_constrain_image_size_universe,ig_android_business_new_grap"
                          "hql_endpoint_universe,ig_ranking_following,ig_android_stories_profile_camera_entry_point,ig_android_univer"
                          "se_reel_video_production,ig_android_power_metrics,ig_android_sfplt,ig_android_offline_hashtag_feed,ig_andr"
                          "oid_live_skin_smooth,ig_android_direct_inbox_search,ig_android_stories_posting_offline_ui,ig_android_sidec"
                          "ar_video_upload_universe,ig_android_promotion_manager_entry_point_universe,ig_android_direct_reply_audienc"
                          "e_upgrade,ig_android_swipe_navigation_x_angle_universe,ig_android_offline_mode_holdout,ig_android_live_sen"
                          "d_user_location,ig_android_direct_fetch_before_push_notif,ig_android_non_square_first,ig_android_insta_vid"
                          "eo_drawing,ig_android_swipeablefilters_universe,ig_android_live_notification_control_universe,ig_android_a"
                          "nalytics_logger_running_background_universe,ig_android_save_all,ig_android_reel_viewer_data_buffer_size,ig"
                          "_direct_quality_holdout_universe,ig_android_family_bridge_discover,ig_android_react_native_restart_after_e"
                          "rror_universe,ig_android_startup_manager,ig_story_tray_peek_content_universe,ig_android_profile,ig_android"
                          "_high_res_upload_2,ig_android_http_service_same_thread,ig_android_scroll_to_dismiss_keyboard,ig_android_re"
                          "move_followers_universe,ig_android_skip_video_render,ig_android_story_timestamps,ig_android_live_viewer_co"
                          "mment_prompt_universe,ig_profile_holdout_universe,ig_android_react_native_insights_grid_view,ig_stories_se"
                          "lfie_sticker,ig_android_stories_reply_composer_redesign,ig_android_streamline_page_creation,ig_explore_net"
                          "ego,ig_android_ig4b_connect_fb_button_universe,ig_android_feed_util_rect_optimization,ig_android_rendering"
                          "_controls,ig_android_os_version_blocking,ig_android_encoder_width_safe_multiple_16,ig_search_new_bootstrap"
                          "_holdout_universe,ig_android_snippets_profile_nux,ig_android_e2e_optimization_universe,ig_android_comments"
                          "_logging_universe,ig_shopping_insights,ig_android_save_collections,ig_android_live_see_fewer_videos_like_t"
                          "his_universe,ig_android_show_new_contact_import_dialog,ig_android_live_view_profile_from_comments_universe"
                          ",ig_fbns_blocked,ig_formats_and_feedbacks_holdout_universe,ig_android_reduce_view_pager_buffer,ig_android_"
                          "instavideo_periodic_notif,ig_search_user_auto_complete_cache_sync_ttl,ig_android_marauder_update_frequency"
                          ",ig_android_suggest_password_reset_on_oneclick_login,ig_android_promotion_entry_from_ads_manager_universe,"
                          "ig_android_live_special_codec_size_list,ig_android_enable_share_to_messenger,ig_android_background_main_fe"
                          "ed_fetch,ig_android_live_video_reactions_creation_universe,ig_android_channels_home,ig_android_sidecar_gal"
                          "lery_universe,ig_android_upload_reliability_universe,ig_migrate_mediav2_universe,ig_android_insta_video_br"
                          "oadcaster_infra_perf,ig_android_business_conversion_social_context,android_ig_fbns_kill_switch,ig_android_"
                          "live_webrtc_livewith_consumption,ig_android_destroy_swipe_fragment,ig_android_react_native_universe_kill_s"
                          "witch,ig_android_stories_book_universe,ig_android_all_videoplayback_persisting_sound,ig_android_draw_erase"
                          "r_universe,ig_direct_search_new_bootstrap_holdout_universe,ig_android_cache_layer_bytes_threshold,ig_andro"
                          "id_search_hash_tag_and_username_universe,ig_android_business_promotion,ig_android_direct_search_recipients"
                          "_controller_universe,ig_android_ad_show_full_name_universe,ig_android_anrwatchdog,ig_android_qp_kill_switc"
                          "h,ig_android_2fac,ig_direct_bypass_group_size_limit_universe,ig_android_promote_simplified_flow,ig_android"
                          "_share_to_whatsapp,ig_android_hide_bottom_nav_bar_on_discover_people,ig_fbns_dump_ids,ig_android_hands_fre"
                          "e_before_reverse,ig_android_skywalker_live_event_start_end,ig_android_live_join_comment_ui_change,ig_andro"
                          "id_direct_search_story_recipients_universe,ig_android_direct_full_size_gallery_upload,ig_android_ad_browse"
                          "r_gesture_control,ig_channel_server_experiments,ig_android_video_cover_frame_from_original_as_fallback,ig_"
                          "android_ad_watchinstall_universe,ig_android_ad_viewability_logging_universe,ig_android_new_optic,ig_androi"
                          "d_direct_visual_replies,ig_android_stories_search_reel_mentions_universe,ig_android_threaded_comments_univ"
                          "erse,ig_android_mark_reel_seen_on_Swipe_forward,ig_internal_ui_for_lazy_loaded_modules_experiment,ig_fbns_"
                          "shared,ig_android_capture_slowmo_mode,ig_android_live_viewers_list_search_bar,ig_android_video_single_surf"
                          "ace,ig_android_offline_reel_feed,ig_android_video_download_logging,ig_android_last_edits,ig_android_exopla"
                          "yer_4142,ig_android_post_live_viewer_count_privacy_universe,ig_android_activity_feed_click_state,ig_androi"
                          "d_snippets_haptic_feedback,ig_android_gl_drawing_marks_after_undo_backing,ig_android_mark_seen_state_on_vi"
                          "ewed_impression,ig_android_live_backgrounded_reminder_universe,ig_android_live_hide_viewer_nux_universe,ig"
                          "_android_live_monotonic_pts,ig_android_search_top_search_surface_universe,ig_android_user_detail_endpoint,"
                          "ig_android_location_media_count_exp_ig,ig_android_comment_tweaks_universe,ig_android_ad_watchmore_entry_po"
                          "int_universe,ig_android_top_live_notification_universe,ig_android_add_to_last_post,ig_save_insights,ig_and"
                          "roid_live_enhanced_end_screen_universe,ig_android_ad_add_counter_to_logging_event,ig_android_blue_token_co"
                          "nversion_universe,ig_android_exoplayer_settings,ig_android_progressive_jpeg,ig_android_offline_story_stick"
                          "ers,ig_android_gqls_typing_indicator,ig_android_chaining_button_tooltip,ig_android_video_prefetch_for_conn"
                          "ectivity_type,ig_android_use_exo_cache_for_progressive,ig_android_samsung_app_badging,ig_android_ad_holdou"
                          "t_watchandmore_universe,ig_android_offline_commenting,ig_direct_stories_recipient_picker_button,ig_insight"
                          "s_feedback_channel_universe,ig_android_insta_video_abr_resize,ig_android_insta_video_sound_always_on";
    QString SIG_KEY_VERSION = "4";

    QString m_username;
    QString m_password;
    QString m_userID;
    QString m_debug;
    QString m_username_id;
    QString m_uuid;
    QString m_device_id;
    QString m_token;
    QString m_csrftoken=m_token;
    QString m_rank_token;
    QString m_IGDataPath;

    QString m_caption;
    QString m_image_path;

    QDir m_data_path;

    bool m_isLoggedIn = false;

    QString generateDeviceId();

signals:
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
    void unFollowDataReady(QVariant answer);
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


private slots:
    void setUser();
    void doLogin();
    void syncFeatures();
    void profileConnect(QVariant profile);
    void configurePhoto(QVariant answer);
};

#endif // INSTAGRAMV2_H
