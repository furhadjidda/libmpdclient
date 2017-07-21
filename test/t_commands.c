#include "capture.h"
#include <mpd/connection.h>
#include <mpd/response.h>
#include <mpd/queue.h>
#include <mpd/database.h>
#include <mpd/search.h>

#include <check.h>

#include <stdlib.h>
#include <time.h>

static void
abort_command(struct test_capture *capture,
	      struct mpd_connection *connection)
{
	test_capture_send(capture, "ACK [5@0] {} cancel\n");
	mpd_response_finish(connection);
	ck_assert(mpd_connection_clear_error(connection));
}

START_TEST(test_queue_commands)
{
	struct test_capture capture;
	struct mpd_connection *c = test_capture_init(&capture);

	ck_assert(mpd_send_list_queue_meta(c));
	ck_assert_str_eq(test_capture_receive(&capture), "playlistinfo\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_list_queue_range_meta(c, 0, 1));
	ck_assert_str_eq(test_capture_receive(&capture), "playlistinfo \"0:1\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_list_queue_range_meta(c, 42, (unsigned)-1));
	ck_assert_str_eq(test_capture_receive(&capture), "playlistinfo \"42:\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_get_queue_song_pos(c, 42));
	ck_assert_str_eq(test_capture_receive(&capture), "playlistinfo \"42\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_queue_changes_meta(c, 42));
	ck_assert_str_eq(test_capture_receive(&capture), "plchanges \"42\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_queue_changes_meta_range(c, 42, 6, 7));
	ck_assert_str_eq(test_capture_receive(&capture), "plchanges \"42\" \"6:7\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_queue_changes_brief(c, 42));
	ck_assert_str_eq(test_capture_receive(&capture), "plchangesposid \"42\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_queue_changes_meta_range(c, 42, 6, (unsigned)-1));
	ck_assert_str_eq(test_capture_receive(&capture), "plchanges \"42\" \"6:\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_add_tag_id(c, 42, MPD_TAG_COMMENT, "foo"));
	ck_assert_str_eq(test_capture_receive(&capture), "addtagid \"42\" \"Comment\" \"foo\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_clear_tag_id(c, 42, MPD_TAG_COMMENT));
	ck_assert_str_eq(test_capture_receive(&capture), "cleartagid \"42\" \"Comment\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_send_clear_all_tags_id(c, 42));
	ck_assert_str_eq(test_capture_receive(&capture), "cleartagid \"42\"\n");
	abort_command(&capture, c);

	mpd_connection_free(c);
	test_capture_deinit(&capture);
}
END_TEST

START_TEST(test_database_commands)
{
	struct test_capture capture;
	struct mpd_connection *c = test_capture_init(&capture);

	ck_assert(mpd_send_list_files(c, "nfs://foo/bar"));
	ck_assert_str_eq(test_capture_receive(&capture), "listfiles \"nfs://foo/bar\"\n");
	abort_command(&capture, c);

	mpd_connection_free(c);
	test_capture_deinit(&capture);
}
END_TEST

START_TEST(test_search)
{
	struct test_capture capture;
	struct mpd_connection *c = test_capture_init(&capture);

	/* start a search, but cancel it */
	ck_assert(mpd_search_queue_songs(c, false));
	ck_assert(mpd_search_add_uri_constraint(c, MPD_OPERATOR_DEFAULT,
						"foo"));
	mpd_search_cancel(c);

	/* start a new search */
	ck_assert(mpd_search_db_songs(c, true));
	ck_assert(mpd_search_add_base_constraint(c, MPD_OPERATOR_DEFAULT,
						 "foo"));
	ck_assert(mpd_search_add_tag_constraint(c, MPD_OPERATOR_DEFAULT,
						MPD_TAG_ARTIST, "Queen"));
	ck_assert(mpd_search_add_any_tag_constraint(c, MPD_OPERATOR_DEFAULT,
						    "Foo"));
	ck_assert(mpd_search_add_sort_tag(c, MPD_TAG_DATE, false));
	ck_assert(mpd_search_add_window(c, 7, 9));
	ck_assert(mpd_search_commit(c));

	ck_assert_str_eq(test_capture_receive(&capture), "find base \"foo\" Artist \"Queen\" any \"Foo\" sort Date window 7:9\n");
	abort_command(&capture, c);

	/* another search */
	ck_assert(mpd_search_db_songs(c, false));
	ck_assert(mpd_search_add_base_constraint(c, MPD_OPERATOR_DEFAULT,
						 "foo"));
	ck_assert(mpd_search_add_sort_tag(c, MPD_TAG_DATE, false));
	ck_assert(mpd_search_add_window(c, 7, 9));
	ck_assert(mpd_search_commit(c));

	ck_assert_str_eq(test_capture_receive(&capture), "search base \"foo\" sort Date window 7:9\n");
	abort_command(&capture, c);

	mpd_connection_free(c);
	test_capture_deinit(&capture);
}
END_TEST

START_TEST(test_list)
{
	struct test_capture capture;
	struct mpd_connection *c = test_capture_init(&capture);

	ck_assert(mpd_search_db_tags(c, MPD_TAG_ARTIST));
	ck_assert(mpd_search_commit(c));
	ck_assert_str_eq(test_capture_receive(&capture), "list Artist\n");
	abort_command(&capture, c);

	ck_assert(mpd_search_db_tags(c, MPD_TAG_ALBUM));
	ck_assert(mpd_search_add_group_tag(c, MPD_TAG_ARTIST));
	ck_assert(mpd_search_commit(c));
	ck_assert_str_eq(test_capture_receive(&capture), "list Album group Artist\n");
	abort_command(&capture, c);

	mpd_connection_free(c);
	test_capture_deinit(&capture);
}
END_TEST

START_TEST(test_count)
{
	struct test_capture capture;
	struct mpd_connection *c = test_capture_init(&capture);

	ck_assert(mpd_count_db_songs(c));
	ck_assert(mpd_search_add_tag_constraint(c, MPD_OPERATOR_DEFAULT,
						MPD_TAG_ARTIST, "Queen"));
	ck_assert(mpd_search_commit(c));
	ck_assert_str_eq(test_capture_receive(&capture), "count Artist \"Queen\"\n");
	abort_command(&capture, c);

	ck_assert(mpd_count_db_songs(c));
	ck_assert(mpd_search_add_tag_constraint(c, MPD_OPERATOR_DEFAULT,
						MPD_TAG_ARTIST, "Queen"));
	ck_assert(mpd_search_add_group_tag(c, MPD_TAG_ALBUM));
	ck_assert(mpd_search_commit(c));
	ck_assert_str_eq(test_capture_receive(&capture), "count Artist \"Queen\" group Album\n");
	abort_command(&capture, c);

	mpd_connection_free(c);
	test_capture_deinit(&capture);
}
END_TEST

static Suite *
create_suite(void)
{
	Suite *s = suite_create("commands");

	TCase *tc_queue = tcase_create("queue");
	tcase_add_test(tc_queue, test_queue_commands);
	suite_add_tcase(s, tc_queue);

	TCase *tc_database = tcase_create("database");
	tcase_add_test(tc_database, test_database_commands);
	suite_add_tcase(s, tc_database);

	TCase *tc_search = tcase_create("search");
	tcase_add_test(tc_search, test_search);
	tcase_add_test(tc_search, test_list);
	tcase_add_test(tc_search, test_count);
	suite_add_tcase(s, tc_search);

	return s;
}

int
main(void)
{
	Suite *s = create_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	int number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
