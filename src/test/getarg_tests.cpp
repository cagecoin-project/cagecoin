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
    ResetArgs("-CGC");
    BOOST_CHECK(GetBoolArg("-CGC"));
    BOOST_CHECK(GetBoolArg("-CGC", false));
    BOOST_CHECK(GetBoolArg("-CGC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CGCo"));
    BOOST_CHECK(!GetBoolArg("-CGCo", false));
    BOOST_CHECK(GetBoolArg("-CGCo", true));

    ResetArgs("-CGC=0");
    BOOST_CHECK(!GetBoolArg("-CGC"));
    BOOST_CHECK(!GetBoolArg("-CGC", false));
    BOOST_CHECK(!GetBoolArg("-CGC", true));

    ResetArgs("-CGC=1");
    BOOST_CHECK(GetBoolArg("-CGC"));
    BOOST_CHECK(GetBoolArg("-CGC", false));
    BOOST_CHECK(GetBoolArg("-CGC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCGC");
    BOOST_CHECK(!GetBoolArg("-CGC"));
    BOOST_CHECK(!GetBoolArg("-CGC", false));
    BOOST_CHECK(!GetBoolArg("-CGC", true));

    ResetArgs("-noCGC=1");
    BOOST_CHECK(!GetBoolArg("-CGC"));
    BOOST_CHECK(!GetBoolArg("-CGC", false));
    BOOST_CHECK(!GetBoolArg("-CGC", true));

    ResetArgs("-CGC -noCGC");  // -CGC should win
    BOOST_CHECK(GetBoolArg("-CGC"));
    BOOST_CHECK(GetBoolArg("-CGC", false));
    BOOST_CHECK(GetBoolArg("-CGC", true));

    ResetArgs("-CGC=1 -noCGC=1");  // -CGC should win
    BOOST_CHECK(GetBoolArg("-CGC"));
    BOOST_CHECK(GetBoolArg("-CGC", false));
    BOOST_CHECK(GetBoolArg("-CGC", true));

    ResetArgs("-CGC=0 -noCGC=0");  // -CGC should win
    BOOST_CHECK(!GetBoolArg("-CGC"));
    BOOST_CHECK(!GetBoolArg("-CGC", false));
    BOOST_CHECK(!GetBoolArg("-CGC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CGC=1");
    BOOST_CHECK(GetBoolArg("-CGC"));
    BOOST_CHECK(GetBoolArg("-CGC", false));
    BOOST_CHECK(GetBoolArg("-CGC", true));

    ResetArgs("--noCGC=1");
    BOOST_CHECK(!GetBoolArg("-CGC"));
    BOOST_CHECK(!GetBoolArg("-CGC", false));
    BOOST_CHECK(!GetBoolArg("-CGC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CGC", "eleven"), "eleven");

    ResetArgs("-CGC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CGC", "eleven"), "");

    ResetArgs("-CGC=");
    BOOST_CHECK_EQUAL(GetArg("-CGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CGC", "eleven"), "");

    ResetArgs("-CGC=11");
    BOOST_CHECK_EQUAL(GetArg("-CGC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CGC", "eleven"), "11");

    ResetArgs("-CGC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CGC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CGC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CGC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CGC", 0), 0);

    ResetArgs("-CGC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CGC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CGC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CGC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CGC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CGC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CGC");
    BOOST_CHECK_EQUAL(GetBoolArg("-CGC"), true);

    ResetArgs("--CGC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CGC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCGC");
    BOOST_CHECK(!GetBoolArg("-CGC"));
    BOOST_CHECK(!GetBoolArg("-CGC", true));
    BOOST_CHECK(!GetBoolArg("-CGC", false));

    ResetArgs("-noCGC=1");
    BOOST_CHECK(!GetBoolArg("-CGC"));
    BOOST_CHECK(!GetBoolArg("-CGC", true));
    BOOST_CHECK(!GetBoolArg("-CGC", false));

    ResetArgs("-noCGC=0");
    BOOST_CHECK(GetBoolArg("-CGC"));
    BOOST_CHECK(GetBoolArg("-CGC", true));
    BOOST_CHECK(GetBoolArg("-CGC", false));

    ResetArgs("-CGC --noCGC");
    BOOST_CHECK(GetBoolArg("-CGC"));

    ResetArgs("-noCGC -CGC"); // CGC always wins:
    BOOST_CHECK(GetBoolArg("-CGC"));
}

BOOST_AUTO_TEST_SUITE_END()
