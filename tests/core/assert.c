#ifdef GIT_ASSERT_HARD
# undef GIT_ASSERT_HARD
#endif

#define GIT_ASSERT_HARD 0

#include "clar_libgit2.h"

static const char *hello_world = "hello, world";
static const char *fail = "FAIL";

static int dummy_fn(const char *myarg)
{
	GIT_ASSERT_ARG(myarg);
	GIT_ASSERT_ARG(myarg != hello_world);
	return 0;
}

static void fn_returns_void(const char *myarg)
{
	GIT_ASSERT_ARG_EXT(myarg, return);
	GIT_ASSERT_ARG_EXT(myarg != hello_world, return);
}

static const char *fn_returns_string(const char *myarg)
{
	GIT_ASSERT_ARG_EXT(myarg, return fail);
	GIT_ASSERT_ARG_EXT(myarg != hello_world, return fail);

	return myarg;
}

static int bad_math(void)
{
	GIT_ASSERT(1 + 1 == 3);
	return 42;
}

static void bad_returns_void(void)
{
	GIT_ASSERT_EXT(1 + 1 == 3, return);
}

static const char *bad_returns_string(void)
{
	GIT_ASSERT_EXT(1 + 1 == 3, return NULL);
	return hello_world;
}

void test_core_assert__argument(void)
{
	cl_git_fail(dummy_fn(NULL));
	cl_assert(git_error_last());
	cl_assert_equal_i(GIT_ERROR_INVALID, git_error_last()->klass);
	cl_assert_equal_s("invalid argument: 'myarg'", git_error_last()->message);

	cl_git_fail(dummy_fn(hello_world));
	cl_assert(git_error_last());
	cl_assert_equal_i(GIT_ERROR_INVALID, git_error_last()->klass);
	cl_assert_equal_s("invalid argument: 'myarg != hello_world'", git_error_last()->message);

	cl_git_pass(dummy_fn("foo"));
}

void test_core_assert__argument_with_non_int_return_type(void)
{
	const char *foo = "foo";

	cl_assert_equal_p(fail, fn_returns_string(NULL));
	cl_assert_equal_i(GIT_ERROR_INVALID, git_error_last()->klass);
	cl_assert_equal_s("invalid argument: 'myarg'", git_error_last()->message);

	cl_assert_equal_p(fail, fn_returns_string(hello_world));
	cl_assert_equal_i(GIT_ERROR_INVALID, git_error_last()->klass);
	cl_assert_equal_s("invalid argument: 'myarg != hello_world'", git_error_last()->message);

	cl_assert_equal_p(foo, fn_returns_string(foo));
}

void test_core_assert__argument_with_void_return_type(void)
{
	const char *foo = "foo";

	git_error_clear();
	fn_returns_void(NULL);
	cl_assert_equal_i(GIT_ERROR_INVALID, git_error_last()->klass);
	cl_assert_equal_s("invalid argument: 'myarg'", git_error_last()->message);

	git_error_clear();
	fn_returns_string(hello_world);
	cl_assert_equal_i(GIT_ERROR_INVALID, git_error_last()->klass);
	cl_assert_equal_s("invalid argument: 'myarg != hello_world'", git_error_last()->message);

	git_error_clear();
	cl_assert_equal_p(foo, fn_returns_string(foo));
	cl_assert_equal_p(NULL, git_error_last());
}

void test_core_assert__internal(void)
{
	cl_git_fail(bad_math());
	cl_assert(git_error_last());
	cl_assert_equal_i(GIT_ERROR_INTERNAL, git_error_last()->klass);
	cl_assert_equal_s("unrecoverable internal error: '1 + 1 == 3'", git_error_last()->message);

	cl_assert_equal_p(NULL, bad_returns_string());
	cl_assert(git_error_last());
	cl_assert_equal_i(GIT_ERROR_INTERNAL, git_error_last()->klass);
	cl_assert_equal_s("unrecoverable internal error: '1 + 1 == 3'", git_error_last()->message);

	git_error_clear();
	bad_returns_void();
	cl_assert(git_error_last());
	cl_assert_equal_i(GIT_ERROR_INTERNAL, git_error_last()->klass);
	cl_assert_equal_s("unrecoverable internal error: '1 + 1 == 3'", git_error_last()->message);
}
