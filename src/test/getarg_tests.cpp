#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-VIZ");
    BOOST_CHECK(GetBoolArg("-VIZ"));
    BOOST_CHECK(GetBoolArg("-VIZ", false));
    BOOST_CHECK(GetBoolArg("-VIZ", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-VIZo"));
    BOOST_CHECK(!GetBoolArg("-VIZo", false));
    BOOST_CHECK(GetBoolArg("-VIZo", true));

    ResetArgs("-VIZ=0");
    BOOST_CHECK(!GetBoolArg("-VIZ"));
    BOOST_CHECK(!GetBoolArg("-VIZ", false));
    BOOST_CHECK(!GetBoolArg("-VIZ", true));

    ResetArgs("-VIZ=1");
    BOOST_CHECK(GetBoolArg("-VIZ"));
    BOOST_CHECK(GetBoolArg("-VIZ", false));
    BOOST_CHECK(GetBoolArg("-VIZ", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noVIZ");
    BOOST_CHECK(!GetBoolArg("-VIZ"));
    BOOST_CHECK(!GetBoolArg("-VIZ", false));
    BOOST_CHECK(!GetBoolArg("-VIZ", true));

    ResetArgs("-noVIZ=1");
    BOOST_CHECK(!GetBoolArg("-VIZ"));
    BOOST_CHECK(!GetBoolArg("-VIZ", false));
    BOOST_CHECK(!GetBoolArg("-VIZ", true));

    ResetArgs("-VIZ -noVIZ");  // -VIZ should win
    BOOST_CHECK(GetBoolArg("-VIZ"));
    BOOST_CHECK(GetBoolArg("-VIZ", false));
    BOOST_CHECK(GetBoolArg("-VIZ", true));

    ResetArgs("-VIZ=1 -noVIZ=1");  // -VIZ should win
    BOOST_CHECK(GetBoolArg("-VIZ"));
    BOOST_CHECK(GetBoolArg("-VIZ", false));
    BOOST_CHECK(GetBoolArg("-VIZ", true));

    ResetArgs("-VIZ=0 -noVIZ=0");  // -VIZ should win
    BOOST_CHECK(!GetBoolArg("-VIZ"));
    BOOST_CHECK(!GetBoolArg("-VIZ", false));
    BOOST_CHECK(!GetBoolArg("-VIZ", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--VIZ=1");
    BOOST_CHECK(GetBoolArg("-VIZ"));
    BOOST_CHECK(GetBoolArg("-VIZ", false));
    BOOST_CHECK(GetBoolArg("-VIZ", true));

    ResetArgs("--noVIZ=1");
    BOOST_CHECK(!GetBoolArg("-VIZ"));
    BOOST_CHECK(!GetBoolArg("-VIZ", false));
    BOOST_CHECK(!GetBoolArg("-VIZ", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", "eleven"), "eleven");

    ResetArgs("-VIZ -bar");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", "eleven"), "");

    ResetArgs("-VIZ=");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", "eleven"), "");

    ResetArgs("-VIZ=11");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", "eleven"), "11");

    ResetArgs("-VIZ=eleven");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-VIZ", 0), 0);

    ResetArgs("-VIZ -bar");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-VIZ=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-VIZ=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--VIZ");
    BOOST_CHECK_EQUAL(GetBoolArg("-VIZ"), true);

    ResetArgs("--VIZ=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-VIZ", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noVIZ");
    BOOST_CHECK(!GetBoolArg("-VIZ"));
    BOOST_CHECK(!GetBoolArg("-VIZ", true));
    BOOST_CHECK(!GetBoolArg("-VIZ", false));

    ResetArgs("-noVIZ=1");
    BOOST_CHECK(!GetBoolArg("-VIZ"));
    BOOST_CHECK(!GetBoolArg("-VIZ", true));
    BOOST_CHECK(!GetBoolArg("-VIZ", false));

    ResetArgs("-noVIZ=0");
    BOOST_CHECK(GetBoolArg("-VIZ"));
    BOOST_CHECK(GetBoolArg("-VIZ", true));
    BOOST_CHECK(GetBoolArg("-VIZ", false));

    ResetArgs("-VIZ --noVIZ");
    BOOST_CHECK(GetBoolArg("-VIZ"));

    ResetArgs("-noVIZ -VIZ"); // VIZ always wins:
    BOOST_CHECK(GetBoolArg("-VIZ"));
}

BOOST_AUTO_TEST_SUITE_END()
