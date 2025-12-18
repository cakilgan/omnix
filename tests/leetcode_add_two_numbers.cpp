
#include "omnix/time/oxtime.h"
#include <omnix/test/oxtest.h>

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}

    bool operator==(const ListNode& other) const {
        const ListNode* a = this;
        const ListNode* b = &other;

        while (a && b) {
            if (a->val != b->val)
                return false;
            a = a->next;
            b = b->next;
        }
        return a == nullptr && b == nullptr;
    }
};

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode* result = new ListNode();
    ListNode* head = result;
    auto v1 = 0;
    auto v2 = 0;
    auto sum = 0;
    auto check = 0;
    
    while(l1||l2){

        v1 = l1 ? (l1->val) : 0;
        v2 = l2 ? (l2->val) : 0;
        sum = v1+v2+result->val;
        check = sum-10;

        if(check>=0){
            result->next = new ListNode(1);
            result->val = check;
        }else{
            result->val = sum;
            if(((l1 && l1->next ) || (l2 && l2->next))){
                result->next = new ListNode(0);
            }
        }

        result = result->next;

        if(l1)
            l1 = l1->next;
        if(l2)
            l2 = l2->next;
    }

    return head;
}



OXTEST(LeetCode,AddTwoNumbers){
    ListNode* Node1 = 
    new ListNode(
        9,
        new ListNode(9)
    );
    ListNode* Node2 = 
    new ListNode(
        1
    );

    auto start = ox::now();
    auto sum = addTwoNumbers(Node1, Node2);
    auto end = ox::now();

    printf("dist:%llu\n",ox::nanoseconds(end-start).value());

    auto answer = ListNode(0,new ListNode(0,new ListNode(1)));
    

    IS_EQUALS(*sum,answer);

    return ox::test_result::success;
}