#ifndef INSTAGRAM_H
#define INSTAGRAM_H

#include <QObject>
#include <QDir>
#include <QVariant>

class Instagram : public QObject
{
    Q_OBJECT
public:
    explicit Instagram(QObject *parent = 0);

public slots:
    void login(bool forse = false);
    void logout();

    void setUsername(QString username){this->m_username = username;}
    void setPassword(QString password){this->m_password = password;}

    QString getUsernameId(){return this->m_username_id;}

    void postImage(QString path, QString caption, QString upload_id = "");
    void postVideo(QFile *video);

    void infoMedia(QString mediaId);
    void editMedia(QString mediaId, QString captionText = "");
    void deleteMedia(QString mediaId);
    void removeSelftag(QString mediaId);

    void postComment(QString mediaId, QString commentText);
    void deleteComment(QString mediaId, QString commentId, QString captionText);

    void setPrivateAccount();
    void setPublicAccount();
    void changeProfilePicture(QFile *photo);
    void removeProfilePicture();
    void getProfileData();
    void editProfile(QString url, QString phone, QString first_name, QString biography, QString email, bool gender);
    void getUsernameInfo(QString usernameId);

    void getRecentActivity();
    void getFollowingRecentActivity();

    void getUserTags(QString usernameId);
    void tagFeed(QString tag);
    void userFeed(QString user);
    void exploreFeed(QString max_id="");

    void getTimeLine(QString max_id = "");
    void getUsernameFeed(QString usernameID, QString maxid = "", QString minTimestamp = "");
    void getPopularFeed();

    void getMediaLikers(QString mediaId);
    void getMediaComments(QString mediaId);

    void like(QString mediaId);
    void unLike(QString mediaId);

    void likeComment(QString commentId);
    void unLikeComment(QString commentId);

    void follow(QString userId);
    void unFollow(QString userId);
    void block(QString userId);
    void unBlock(QString userId);
    void userFriendship(QString userId);
    void getLikedMedia(QString maxid = "");

    void getUserFollowings(QString userId, QString maxId = "");
    void getUserFollowers(QString userId, QString maxId = "");

    void checkUsername(QString username);
    void createAccount(QString username, QString password, QString email);

    void searchUsername(QString username);

    void getInbox();
    void getDirectThread(QString threadId);
    void getPendingInbox();
    void getRecentRecipients();

    void rotateImg(QString filename, qreal deg);
    void cropImg(QString filename, bool squared);
    void cropImg(QString in_filename, QString out_filename, int topSpace, bool squared);

private:
    //QString EXPERIMENTS = "ig_android_disk_cache_match_journal_size_to_cache_max_count,ig_android_ad_move_carousel_indicator_to_ufi_universe,ig_android_universe_video_production,ig_android_live_follow_from_comments_universe,ig_android_live_analytics,ig_android_video_captions_universe,ig_android_offline_location_feed,ig_android_ontact_invite_universe,ig_android_insta_video_reconnect_viewers,ig_android_live_broadcast_blacklist,ig_android_checkbox_instead_of_button_as_follow_affordance_universe,ig_android_shopping_reporting,ig_android_ufi_redesign_video_social_context,ig_android_stories_surface_universe,ig_android_verified_comments_universe,ig_android_preload_media_ahead_in_current_reel,android_instagram_prefetch_suggestions_universe,ig_android_direct_inbox_tray_suggested_user_universe,ig_android_direct_blue_tab,ig_android_asset_button_new_content_animation,ig_android_async_network_tweak_universe,ig_android_react_native_lazy_modules_killswitch,ig_android_instavideo_remove_nux_comments,ig_video_copyright_whitelist,ig_android_ad_sponsor_label_story_top_design_universe,ig_android_business_action,ig_android_direct_link_style,ig_android_live_heart_enhancements_universe,ig_android_preload_item_count_in_reel_viewer_buffer,ig_android_auto_retry_post_mode,ig_android_fix_render_thread_crash,ig_android_shopping,ig_fbns_preload_default,ig_android_gesture_dismiss_reel_viewer,ig_android_tool_tip,ig_android_ad_logger_funnel_logging_universe,ig_android_direct_links,ig_android_links_receivers,ig_android_ad_impression_backtest,ig_android_invites_without_token_universe,ig_android_immersive_viewer,ig_android_mqtt_skywalker,ig_fbns_push,ig_android_react_native_universe,ig_android_special_brush,ig_android_live_consumption_abr,ig_android_story_viewer_social_context,ig_android_explore_verified_badges_stories_universe,ig_android_video_loopcount_int,ig_android_enable_main_feed_reel_tray_preloading,ig_android_ad_watchbrowse_universe,ig_android_react_native_ota,ig_android_discover_people_icon_in_others_profile,ig_android_log_mediacodec_info,ig_android_enable_back_navigation_nux_universe,ig_android_cold_start_feed_request,ig_video_use_sve_universe,ig_android_offline_explore_10_14,ig_android_stories_teach_gallery_location,ig_android_http_stack_experiment_2017,ig_android_stories_device_tilt,ig_android_pending_request_search_bar,ig_android_fb_topsearch_sgp_fork_request,ig_android_animation_perf_reporter_timeout,ig_android_new_block_flow,ig_android_direct_address_links,ig_android_share_profile_photo_to_feed_universe,ig_android_stories_private_likes,ig_android_text_background,ig_android_stories_video_prefetch_kb,ig_android_su_activity_feed,ig_android_live_stop_broadcast_on_404,ig_android_promotion_feedback_channel,ig_android_render_iframe_interval,ig_android_boomerang_entry,ig_android_camera_shortcut_universe,ig_android_fetch_fresh_viewer_list,ig_android_ad_media_url_logging_universe,ig_android_phone_confirm_rate_limit_language_universe,ig_android_livewith_handle_invites_for_cobroadcast,ig_android_facebook_twitter_profile_photos,ig_android_full_user_detail_endpoint,ig_story_camera_reverse_video_experiment,ig_android_livewith_allow_invite_for_cobroadcast,ig_android_direct_sqlite_universe,ig_android_family_bridge_share,ig_android_search,ig_android_insta_video_consumption_titles,ig_android_camera_universe,ig_android_instavideo_audio_only_mode,ig_android_live_video_reactions_consumption_universe,ig_android_swipe_fragment_container,ig_creation_growth_holdout,ig_android_live_save_to_camera_roll_universe,ig_android_ad_cta_redesign_universe,ig_android_sticker_region_tracking,ig_android_unified_inbox,ig_android_live_new_watch_time,ig_android_offline_main_feed_10_11,ig_android_chaining_teaser_animation,ig_android_business_conversion_value_prop_v2,ig_android_redirect_to_low_latency_universe,ig_android_feed_header_profile_ring_universe,ig_family_bridges_holdout_universe,ig_android_following_follower_social_context,ig_android_video_keep_screen_on,ig_android_profile_photo_as_media,ig_android_ad_leadgen_relay_modern,ig_android_insta_video_consumption_infra,ig_android_ad_watchlead_universe,ig_android_infinite_scrolling_launch,ig_in_feed_commenting,ig_android_live_broadcast_enable_year_class_2011,ig_android_direct_phone_number_links,ig_android_direct_share_sheet_ring,ig_android_stories_weblink_creation,ig_android_histogram_reporter,ig_android_network_cancellation,ig_android_react_native_insights,ig_android_insta_video_audio_encoder,ig_android_family_bridge_bookmarks,ig_android_dash_for_vod_universe,ig_android_direct_mutually_exclusive_experiment_universe,ig_android_stories_selfie_sticker,ig_android_ad_add_per_event_counter_to_logging_event,ig_android_rtl,ig_android_sidecar_tagging_universe,ig_android_direct_send_auto_retry,ig_android_direct_video_autoplay_scroll,ig_android_promote_from_profile_button,ig_android_share_spinner,ig_android_text_action,ig_android_profile_share_username,ig_android_sidecar_edit_screen_universe,ig_promotions_unit_in_insights_landing_page,ig_android_save_longpress_tooltip,ig_android_constrain_image_size_universe,ig_android_business_new_graphql_endpoint_universe,ig_ranking_following,ig_android_universe_reel_video_production,ig_android_sfplt,ig_android_story_tray_title_play_all,ig_android_offline_hashtag_feed,ig_android_live_skin_smooth,ig_android_stories_posting_offline_ui,ig_android_direct_add_local_thread_in_inbox,ig_android_swipe_navigation_x_angle_universe,ig_android_offline_mode_holdout,ig_android_non_square_first,ig_android_insta_video_drawing,ig_android_react_native_usertag,ig_android_swipeablefilters_universe,ig_android_live_notification_control_universe,ig_android_analytics_logger_running_background_universe,ig_android_save_all,ig_android_reel_viewer_data_buffer_size,ig_direct_quality_holdout_universe,ig_android_family_bridge_discover,ig_android_react_native_restart_after_error_universe,ig_story_tray_peek_content_universe,ig_android_profile,ig_android_high_res_upload_2,ig_android_http_service_same_thread,ig_android_remove_followers_universe,ig_android_skip_video_render,ig_android_live_viewer_comment_prompt_universe,ig_android_react_native_insights_grid_view,ig_android_search_client_matching,ig_explore_netego,ig_android_boomerang_feed_attribution,ig_android_explore_story_sfslt_universe,ig_android_rendering_controls,ig_android_os_version_blocking,ig_android_encoder_width_safe_multiple_16,ig_android_direct_video_autoplay,ig_android_snippets_profile_nux,ig_android_e2e_optimization_universe,ig_android_disk_usage,ig_android_save_collections,ig_android_live_see_fewer_videos_like_this_universe,ig_android_live_view_profile_from_comments_universe,ig_formats_and_feedbacks_holdout_universe,ig_fbns_blocked,ig_android_instavideo_periodic_notif,ig_android_empty_feed_redesign,ig_android_marauder_update_frequency,ig_android_suggest_password_reset_on_oneclick_login,ig_android_live_special_codec_size_list,ig_android_enable_share_to_messenger,ig_android_live_video_reactions_creation_universe,ig_android_live_hide_viewer_nux,ig_android_channels_home,ig_android_sidecar_gallery_universe,ig_android_live_using_webrtc,ig_android_insta_video_broadcaster_infra_perf,ig_android_business_conversion_social_context,android_ig_fbns_kill_switch,ig_android_retry_story_seen_state,ig_android_react_native_universe_kill_switch,ig_android_stories_book_universe,ig_android_all_videoplayback_persisting_sound,ig_android_cache_layer_bytes_threshold,ig_android_comment_deep_linking_v1,ig_android_business_promotion,ig_android_anrwatchdog,ig_android_qp_kill_switch,ig_android_ad_always_send_ad_attribution_id_universe,ig_android_2fac,ig_direct_bypass_group_size_limit_universe,ig_android_promote_simplified_flow,ig_android_share_to_whatsapp,ig_fbns_dump_ids,ig_android_ad_show_mai_cta_loading_state_universe,ig_android_skywalker_live_event_start_end,ig_android_toplive_verified_badges_universe,ig_android_live_join_comment_ui_change,ig_android_draw_button_new_tool_animation,ig_video_max_duration_qe_preuniverse,ig_android_ad_watchinstall_universe,ig_android_http_stack_kz_debug,ig_request_cache_layer,ig_android_carousel_feed_indicators_universe,ig_android_new_optic,ig_android_mark_reel_seen_on_Swipe_forward,ig_fbns_shared,ig_android_capture_slowmo_mode,ig_android_save_multi_select,ig_android_mead,ig_android_video_single_surface,ig_android_offline_reel_feed,ig_android_video_download_logging,ig_android_last_edits,ig_android_exoplayer_4142,ig_android_snippets_haptic_feedback,ig_android_gl_drawing_marks_after_undo_backing,ig_android_mark_seen_state_on_viewed_impression,ig_android_live_backgrounded_reminder_universe,ig_android_disable_comment_public_test,ig_android_user_detail_endpoint,ig_android_location_media_count_exp_ig,ig_android_comment_tweaks_universe,ig_android_add_to_last_post,ig_save_insights,ig_android_live_enhanced_end_screen_universe,ig_android_ad_add_counter_to_logging_event,ig_android_sidecar,ig_android_direct_split_new_message_button,ig_android_grid_video_icon,ig_android_progressive_jpeg,ig_android_offline_story_stickers,ig_android_direct_inbox_unseen_hint,ig_android_top_live_titles_universe,ig_android_video_prefetch_for_connectivity_type,ig_android_sync_on_background_enhanced,ig_android_upload_reliability_use_fbupload_lib,ig_android_ad_holdout_watchandmore_universe,ig_android_samsung_app_badging,ig_android_offline_commenting,ig_android_insta_video_abr_resize,ig_android_insta_video_sound_always_on,ig_android_disable_comment";
    QString EXPERIMENTS = "ig_promote_reach_objective_fix_universe,ig_android_universe_video_production,ig_search_client_h1_2017_holdout,ig_android_live_follow_from_comments_universe,ig_android_carousel_non_square_creation,ig_android_live_analytics,ig_android_follow_all_dialog_confirmation_copy,ig_android_stories_server_coverframe,ig_android_video_captions_universe,ig_android_offline_location_feed,ig_android_direct_inbox_retry_seen_state,ig_android_ontact_invite_universe,ig_android_live_broadcast_blacklist,ig_android_insta_video_reconnect_viewers,ig_android_ad_async_ads_universe,ig_android_search_clear_layout_universe,ig_android_shopping_reporting,ig_android_stories_surface_universe,ig_android_verified_comments_universe,ig_android_preload_media_ahead_in_current_reel,android_instagram_prefetch_suggestions_universe,ig_android_reel_viewer_fetch_missing_reels_universe,ig_android_direct_search_share_sheet_universe,ig_android_business_promote_tooltip,ig_android_direct_blue_tab,ig_android_async_network_tweak_universe,ig_android_elevate_main_thread_priority_universe,ig_android_stories_gallery_nux,ig_android_instavideo_remove_nux_comments,ig_video_copyright_whitelist,ig_react_native_inline_insights_with_relay,ig_android_direct_thread_message_animation,ig_android_draw_rainbow_client_universe,ig_android_direct_link_style,ig_android_live_heart_enhancements_universe,ig_android_rtc_reshare,ig_android_preload_item_count_in_reel_viewer_buffer,ig_android_users_bootstrap_service,ig_android_auto_retry_post_mode,ig_android_shopping,ig_android_main_feed_seen_state_dont_send_info_on_tail_load,ig_fbns_preload_default,ig_android_gesture_dismiss_reel_viewer,ig_android_tool_tip,ig_android_ad_logger_funnel_logging_universe,ig_android_gallery_grid_column_count_universe,ig_android_business_new_ads_payment_universe,ig_android_direct_links,ig_android_audience_control,ig_android_live_encore_consumption_settings_universe,ig_perf_android_holdout,ig_android_cache_contact_import_list,ig_android_links_receivers,ig_android_ad_impression_backtest,ig_android_list_redesign,ig_android_stories_separate_overlay_creation,ig_android_stop_video_recording_fix_universe,ig_android_render_video_segmentation,ig_android_live_encore_reel_chaining_universe,ig_android_sync_on_background_enhanced_10_25,ig_android_immersive_viewer,ig_android_mqtt_skywalker,ig_fbns_push,ig_android_ad_watchmore_overlay_universe,ig_android_react_native_universe,ig_android_profile_tabs_redesign_universe,ig_android_live_consumption_abr,ig_android_story_viewer_social_context,ig_android_hide_post_in_feed,ig_android_video_loopcount_int,ig_android_enable_main_feed_reel_tray_preloading,ig_android_camera_upsell_dialog,ig_android_ad_watchbrowse_universe,ig_android_internal_research_settings,ig_android_search_people_tag_universe,ig_android_react_native_ota,ig_android_enable_concurrent_request,ig_android_react_native_stories_grid_view,ig_android_business_stories_inline_insights,ig_android_log_mediacodec_info,ig_android_direct_expiring_media_loading_errors,ig_video_use_sve_universe,ig_android_cold_start_feed_request,ig_android_enable_zero_rating,ig_android_reverse_audio,ig_android_branded_content_three_line_ui_universe,ig_android_live_encore_production_universe,ig_stories_music_sticker,ig_android_stories_teach_gallery_location,ig_android_http_stack_experiment_2017,ig_android_stories_device_tilt,ig_android_pending_request_search_bar,ig_android_fb_topsearch_sgp_fork_request,ig_android_seen_state_with_view_info,ig_android_animation_perf_reporter_timeout,ig_android_new_block_flow,ig_android_story_tray_title_play_all_v2,ig_android_direct_address_links,ig_android_stories_archive_universe,ig_android_save_collections_cover_photo,ig_android_live_webrtc_livewith_production,ig_android_sign_video_url,ig_android_stories_video_prefetch_kb,ig_android_stories_create_flow_favorites_tooltip,ig_android_live_stop_broadcast_on_404,ig_android_live_viewer_invite_universe,ig_android_promotion_feedback_channel,ig_android_render_iframe_interval,ig_android_accessibility_logging_universe,ig_android_camera_shortcut_universe,ig_android_use_one_cookie_store_per_user_override,ig_profile_holdout_2017_universe,ig_android_stories_server_brushes,ig_android_ad_media_url_logging_universe,ig_android_shopping_tag_nux_text_universe,ig_android_comments_single_reply_universe,ig_android_stories_video_loading_spinner_improvements,ig_android_collections_cache,ig_android_comment_api_spam_universe,ig_android_facebook_twitter_profile_photos,ig_android_shopping_tag_creation_universe,ig_story_camera_reverse_video_experiment,ig_android_direct_bump_selected_recipients,ig_android_ad_cta_haptic_feedback_universe,ig_android_vertical_share_sheet_experiment,ig_android_family_bridge_share,ig_android_search,ig_android_insta_video_consumption_titles,ig_android_stories_gallery_preview_button,ig_android_fb_auth_education,ig_android_camera_universe,ig_android_me_only_universe,ig_android_instavideo_audio_only_mode,ig_android_user_profile_chaining_icon,ig_android_live_video_reactions_consumption_universe,ig_android_stories_hashtag_text,ig_android_post_live_badge_universe,ig_android_swipe_fragment_container,ig_android_search_users_universe,ig_android_live_save_to_camera_roll_universe,ig_creation_growth_holdout,ig_android_sticker_region_tracking,ig_android_unified_inbox,ig_android_live_new_watch_time,ig_android_offline_main_feed_10_11,ig_import_biz_contact_to_page,ig_android_live_encore_consumption_universe,ig_android_experimental_filters,ig_android_search_client_matching_2,ig_android_react_native_inline_insights_v2,ig_android_business_conversion_value_prop_v2,ig_android_redirect_to_low_latency_universe,ig_android_ad_show_new_awr_universe,ig_family_bridges_holdout_universe,ig_android_background_explore_fetch,ig_android_following_follower_social_context,ig_android_video_keep_screen_on,ig_android_ad_leadgen_relay_modern,ig_android_profile_photo_as_media,ig_android_insta_video_consumption_infra,ig_android_ad_watchlead_universe,ig_android_direct_prefetch_direct_story_json,ig_android_shopping_react_native,ig_android_top_live_profile_pics_universe,ig_android_direct_phone_number_links,ig_android_stories_weblink_creation,ig_android_direct_search_new_thread_universe,ig_android_histogram_reporter,ig_android_direct_on_profile_universe,ig_android_network_cancellation,ig_android_background_reel_fetch,ig_android_react_native_insights,ig_android_insta_video_audio_encoder,ig_android_family_bridge_bookmarks,ig_android_data_usage_network_layer,ig_android_universal_instagram_deep_links,ig_android_dash_for_vod_universe,ig_android_modular_tab_discover_people_redesign,ig_android_mas_sticker_upsell_dialog_universe,ig_android_ad_add_per_event_counter_to_logging_event,ig_android_sticky_header_top_chrome_optimization,ig_android_rtl,ig_android_biz_conversion_page_pre_select,ig_android_promote_from_profile_button,ig_android_live_broadcaster_invite_universe,ig_android_share_spinner,ig_android_text_action,ig_android_own_reel_title_universe,ig_promotions_unit_in_insights_landing_page,ig_android_business_settings_header_univ,ig_android_save_longpress_tooltip,ig_android_constrain_image_size_universe,ig_android_business_new_graphql_endpoint_universe,ig_ranking_following,ig_android_stories_profile_camera_entry_point,ig_android_universe_reel_video_production,ig_android_power_metrics,ig_android_sfplt,ig_android_offline_hashtag_feed,ig_android_live_skin_smooth,ig_android_direct_inbox_search,ig_android_stories_posting_offline_ui,ig_android_sidecar_video_upload_universe,ig_android_promotion_manager_entry_point_universe,ig_android_direct_reply_audience_upgrade,ig_android_swipe_navigation_x_angle_universe,ig_android_offline_mode_holdout,ig_android_live_send_user_location,ig_android_direct_fetch_before_push_notif,ig_android_non_square_first,ig_android_insta_video_drawing,ig_android_swipeablefilters_universe,ig_android_live_notification_control_universe,ig_android_analytics_logger_running_background_universe,ig_android_save_all,ig_android_reel_viewer_data_buffer_size,ig_direct_quality_holdout_universe,ig_android_family_bridge_discover,ig_android_react_native_restart_after_error_universe,ig_android_startup_manager,ig_story_tray_peek_content_universe,ig_android_profile,ig_android_high_res_upload_2,ig_android_http_service_same_thread,ig_android_scroll_to_dismiss_keyboard,ig_android_remove_followers_universe,ig_android_skip_video_render,ig_android_story_timestamps,ig_android_live_viewer_comment_prompt_universe,ig_profile_holdout_universe,ig_android_react_native_insights_grid_view,ig_stories_selfie_sticker,ig_android_stories_reply_composer_redesign,ig_android_streamline_page_creation,ig_explore_netego,ig_android_ig4b_connect_fb_button_universe,ig_android_feed_util_rect_optimization,ig_android_rendering_controls,ig_android_os_version_blocking,ig_android_encoder_width_safe_multiple_16,ig_search_new_bootstrap_holdout_universe,ig_android_snippets_profile_nux,ig_android_e2e_optimization_universe,ig_android_comments_logging_universe,ig_shopping_insights,ig_android_save_collections,ig_android_live_see_fewer_videos_like_this_universe,ig_android_show_new_contact_import_dialog,ig_android_live_view_profile_from_comments_universe,ig_fbns_blocked,ig_formats_and_feedbacks_holdout_universe,ig_android_reduce_view_pager_buffer,ig_android_instavideo_periodic_notif,ig_search_user_auto_complete_cache_sync_ttl,ig_android_marauder_update_frequency,ig_android_suggest_password_reset_on_oneclick_login,ig_android_promotion_entry_from_ads_manager_universe,ig_android_live_special_codec_size_list,ig_android_enable_share_to_messenger,ig_android_background_main_feed_fetch,ig_android_live_video_reactions_creation_universe,ig_android_channels_home,ig_android_sidecar_gallery_universe,ig_android_upload_reliability_universe,ig_migrate_mediav2_universe,ig_android_insta_video_broadcaster_infra_perf,ig_android_business_conversion_social_context,android_ig_fbns_kill_switch,ig_android_live_webrtc_livewith_consumption,ig_android_destroy_swipe_fragment,ig_android_react_native_universe_kill_switch,ig_android_stories_book_universe,ig_android_all_videoplayback_persisting_sound,ig_android_draw_eraser_universe,ig_direct_search_new_bootstrap_holdout_universe,ig_android_cache_layer_bytes_threshold,ig_android_search_hash_tag_and_username_universe,ig_android_business_promotion,ig_android_direct_search_recipients_controller_universe,ig_android_ad_show_full_name_universe,ig_android_anrwatchdog,ig_android_qp_kill_switch,ig_android_2fac,ig_direct_bypass_group_size_limit_universe,ig_android_promote_simplified_flow,ig_android_share_to_whatsapp,ig_android_hide_bottom_nav_bar_on_discover_people,ig_fbns_dump_ids,ig_android_hands_free_before_reverse,ig_android_skywalker_live_event_start_end,ig_android_live_join_comment_ui_change,ig_android_direct_search_story_recipients_universe,ig_android_direct_full_size_gallery_upload,ig_android_ad_browser_gesture_control,ig_channel_server_experiments,ig_android_video_cover_frame_from_original_as_fallback,ig_android_ad_watchinstall_universe,ig_android_ad_viewability_logging_universe,ig_android_new_optic,ig_android_direct_visual_replies,ig_android_stories_search_reel_mentions_universe,ig_android_threaded_comments_universe,ig_android_mark_reel_seen_on_Swipe_forward,ig_internal_ui_for_lazy_loaded_modules_experiment,ig_fbns_shared,ig_android_capture_slowmo_mode,ig_android_live_viewers_list_search_bar,ig_android_video_single_surface,ig_android_offline_reel_feed,ig_android_video_download_logging,ig_android_last_edits,ig_android_exoplayer_4142,ig_android_post_live_viewer_count_privacy_universe,ig_android_activity_feed_click_state,ig_android_snippets_haptic_feedback,ig_android_gl_drawing_marks_after_undo_backing,ig_android_mark_seen_state_on_viewed_impression,ig_android_live_backgrounded_reminder_universe,ig_android_live_hide_viewer_nux_universe,ig_android_live_monotonic_pts,ig_android_search_top_search_surface_universe,ig_android_user_detail_endpoint,ig_android_location_media_count_exp_ig,ig_android_comment_tweaks_universe,ig_android_ad_watchmore_entry_point_universe,ig_android_top_live_notification_universe,ig_android_add_to_last_post,ig_save_insights,ig_android_live_enhanced_end_screen_universe,ig_android_ad_add_counter_to_logging_event,ig_android_blue_token_conversion_universe,ig_android_exoplayer_settings,ig_android_progressive_jpeg,ig_android_offline_story_stickers,ig_android_gqls_typing_indicator,ig_android_chaining_button_tooltip,ig_android_video_prefetch_for_connectivity_type,ig_android_use_exo_cache_for_progressive,ig_android_samsung_app_badging,ig_android_ad_holdout_watchandmore_universe,ig_android_offline_commenting,ig_direct_stories_recipient_picker_button,ig_insights_feedback_channel_universe,ig_android_insta_video_abr_resize,ig_android_insta_video_sound_always_on";
    QString m_username;
    QString m_password;
    QString m_userID;
    QString m_debug;
    QString m_username_id;
    QString m_uuid;
    QString m_device_id;
    QString m_token;
    QString m_rank_token;
    QString m_IGDataPath;

    QString m_caption;
    QString m_image_path;

    QString sig_key_version = "4";

    QDir m_data_path;

    bool m_isLoggedIn = false;

    QString generateDeviceId();

signals:
    void profileConnected(QVariant answer);
    void profileConnectedFail();

    void mediaInfoReady(QVariant answer);
    void mediaEdited(QVariant answer);
    void mediaDeleted(QVariant answer);

    void imageConfigureDataReady(QVariant answer);

    void removeSelftagDone(QVariant answer);
    void commentPosted(QVariant answer);
    void commentDeleted(QVariant answer);

    void profilePictureDeleted(QVariant answer);
    void setProfilePrivate(QVariant answer);
    void setProfilePublic(QVariant answer);
    void profileDataReady(QVariant answer);
    void editDataReady(QVariant answer);
    void usernameDataReady(QVariant answer);

    void recentActivityDataReady(QVariant answer);
    void followingRecentDataReady(QVariant answer);

    void userTagsDataReady(QVariant answer);
    void tagFeedDataReady(QVariant answer);
    void exploreDataReady(QVariant answer);

    void userFeedDataReady(QVariant answer);
    void timeLineDataReady(QVariant answer);
    void userTimeLineDataReady(QVariant answer);
    void popularFeedDataReady(QVariant answer);

    void mediaLikersDataReady(QVariant answer);
    void mediaCommentsDataReady(QVariant answer);
    void likeDataReady(QVariant answer);
    void unLikeDataReady(QVariant answer);

    void likeCommentDataReady(QVariant answer);
    void unLikeCommentDataReady(QVariant answer);

    void followDataReady(QVariant answer);
    void unFollowDataReady(QVariant answer);
    void blockDataReady(QVariant answer);
    void unBlockDataReady(QVariant answer);
    void userFriendshipDataReady(QVariant answer);
    void likedMediaDataReady(QVariant answer);

    void userFollowingsDataReady(QVariant answer);
    void userFollowersDataReady(QVariant answer);

    void doLogout(QVariant answer);

    void usernameCheckDataReady(QVariant answer);
    void createAccountDataReady(QVariant answer);

    void error(QString message);

    void searchUsernameDataReady(QVariant answer);

    void getInboxDataReady(QVariant answer);
    void getDirectThreadDataReady(QVariant answer);
    void getPendingInboxDataReady(QVariant answer);
    void getRecentRecipientsDataReady(QVariant answer);

private slots:
    void setUser();
    void doLogin();
    void syncFeatures();
    void profileConnect(QVariant profile);
    void configurePhoto(QVariant answer);
};

#endif // INSTAGRAM_H
