#include "omnix/time/oxtime.h"
#include <algorithm>
#include <omnix/test/oxtest.h>
#include <vector>


double findMedianSortedArrays(const std::vector<int>& nums1, const std::vector<int>& nums2) {
    std::vector<int> arr;
    arr.reserve(nums1.size()+nums2.size());

    for (auto& num : nums1) {
        arr.emplace_back(num);
    }
    for (auto& num : nums2) {
        arr.emplace_back(num);
    }
    
    std::sort(arr.begin(),arr.end());


    //printf("arrsize:%llu\n",arr.size());
    auto mid = arr.size()/2;
    //printf("mid:%llu\n",arr.size()/2);


    //for (auto& num : arr) {
        //printf("NUM:%i\n",num);
    //}

    if(arr.size()%2==0){
        //printf("mid 2: %i,%i\n",arr[mid-1],arr[mid]);
        return static_cast<double>(arr[mid-1]+arr[mid])/2;
    }else{
        //printf("mid single: %i\n",arr[mid]);
        return arr[mid];
    }
}

OXTEST(LeetCode,MedianOfTwoSortedArrays){
    IS_EQUALS(findMedianSortedArrays({1,3}, {2}),2);
    IS_EQUALS(findMedianSortedArrays({1,2}, {3,4}),2.50000);
    return ox::test_result::success;
}