/**
 * Definition for singly-linked list.
 */

struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

/*
    int carry = 0;

    while (l1 || l2) {
      const int sum = (l1 ? l1->val : 0) + (l2 ? l2->val : 0) + carry;

      const int mod = sum % 10;

      if (mod != sum) {
        carry = mod + 1;
      }

      iterator->val = mod;

      if (l1)
        l1 = l1->next;

      if (l2)
        l2 = l2->next;

      if (l1 || l2) {
        iterator->next = new ListNode;
        iterator = iterator->next;
      }
    }
*/

class Solution {
private:
  const int listNodeValue(const ListNode *l) { return l ? l->val : 0; }

public:
  ListNode *addTwoNumbers(ListNode *l1, ListNode *l2) {

    ListNode *l3 = new ListNode;
    ListNode *iterator = l3;

    int carry = 0;

    while (l1 || l2 || carry) {
      const int sum = this->listNodeValue(l1) + this->listNodeValue(l2) + carry;

      carry = sum / 10;

      iterator->val = sum % 10;

      if (l1)
        l1 = l1->next;

      if (l2)
        l2 = l2->next;

      if (!l1 && !l2 && !carry) {
        break;
      }

      iterator->next = new ListNode;
      iterator = iterator->next;
    }

    return l3;
  }
};
