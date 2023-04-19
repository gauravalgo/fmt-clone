/* formatting library tests
AUTHOR : GAURAV SHARMA
*/

#include <cfloat>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include "format.h"

using std::size_t;
using std::sprintf;

using fmt::Formatter;
using fmt::Format;

TEST(FormatterTest, FormatNoArgs )
{
    Formatter format;
    //format("test

}

