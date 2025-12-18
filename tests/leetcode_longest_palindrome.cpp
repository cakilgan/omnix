#include "omnix/time/oxtime.h"
#include <algorithm>
#include <omnix/test/oxtest.h>
#include <vector>


// abccba

std::string longestPalindrome(const std::string& s) {
    
}
OXTEST(LeetCode,LongestPalindrome){
    IS_EQUALS(longestPalindrome("hoppalacumbala"), "abba");
    return ox::test_result::success;
}