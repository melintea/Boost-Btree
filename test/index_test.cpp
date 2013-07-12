//  btree_unit_test.cpp  ---------------------------------------------------------------//

//  Copyright Beman Dawes 2006, 2010

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See http://www.boost.org/libs/btree for documentation.

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  These tests lightly exercise many portions of the interface. They do not attempt    //
//  to stress the many combinations of control paths possible in large scale use.       //
//  See stl_equivalence_test.cpp for large scale stress testing.                        //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#include <boost/btree/map.hpp>
#include <boost/btree/set.hpp>
#include <boost/btree/support/strbuf.hpp>
#include <boost/btree/support/fixstr.hpp>
#include <boost/detail/lightweight_main.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/cstdint.hpp>
#include <boost/random.hpp>
#include <boost/btree/support/random_string.hpp>

#include <iostream>
#include <iomanip>
#include <utility>
#include <map>
#include <set>
#include <algorithm>

using namespace boost;
namespace fs = boost::filesystem;
using std::cout;
using std::endl;
using std::make_pair;

namespace
{
  bool dump_dot(false);

  struct fat
  {
    int x;
    char unused[28];

    fat(int x_) : x(x_) {}
    fat() : x(-1) {}
    fat(const fat& f) : x(f.x) {}
    fat& operator=(const fat& f) {x = f.x; return *this; }
    fat& operator=(int v) {x = v; return *this; }

    bool operator<(const fat& rhs) const {return x < rhs.x;}
    bool operator==(const fat& rhs) const {return x == rhs.x;}
    bool operator!=(const fat& rhs) const {return x != rhs.x;}
  };

  std::ostream& operator<<(std::ostream& os, const fat& f)
  {
    os << f.x;
    return os;
  }

}  // unnamed namespace

namespace {

//-------------------------------------- instantiate -----------------------------------//

template <class BT>
void instantiate_test(BT& bt)
{ 
  BOOST_TEST(!bt.is_open());
  BOOST_TEST(bt.size() == 0U);
  BOOST_TEST(bt.empty());
}

void instantiate()
{
  cout << "  instantiate..." << endl;

  // map
  {
    btree::btree_map<fat, int> x;
    instantiate_test(x);
  }
  {
    btree::btree_map<fat, int, btree::big_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_map<fat, int, btree::little_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_map<fat, int, btree::native_endian_traits> x;
    instantiate_test(x);
  }

  // multimap
  {
    btree::btree_multimap<fat, int> x;
    instantiate_test(x);
  }
  {
    btree::btree_multimap<fat, int, btree::big_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_multimap<fat, int, btree::little_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_multimap<fat, int, btree::native_endian_traits> x;
    instantiate_test(x);
  }

  // set
  {
    btree::btree_set<int> x;
    instantiate_test(x);
  }
  {
    btree::btree_set<int, btree::big_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_set<int, btree::little_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_set<int, btree::native_endian_traits> x;
    instantiate_test(x);
  }

  // multiset
  {
    btree::btree_multiset<int> x;
    instantiate_test(x);
  }
  {
    btree::btree_multiset<int, btree::big_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_multiset<int, btree::little_endian_traits> x;
    instantiate_test(x);
  }
  {
    btree::btree_multiset<int, btree::native_endian_traits> x;
    instantiate_test(x);
  }

  cout << "    instantiate complete" << endl;
}

//-------------------------------- types_test ------------------------------------------//

void types_test()
{
  cout << "  types_test..." << endl;

  //  test std::set and std::map to add insight into types

  typedef std::set<int>                  std_set;
  typedef btree::btree_set<int>         bt_set;

  BOOST_TEST((boost::is_same<std_set::key_type, int>::value));
  BOOST_TEST((boost::is_same< bt_set::key_type, int>::value));

  BOOST_TEST((boost::is_same<std_set::value_type, int>::value));
  BOOST_TEST((boost::is_same< bt_set::value_type, int>::value));

  // reference is "lvalue of T" where T is value_type
  BOOST_TEST((boost::is_same<std_set::reference, int&>::value));
  BOOST_TEST((boost::is_same< bt_set::reference, int&>::value));

  // const_reference is "const lvalue of T" where T is value_type
  BOOST_TEST((boost::is_same<std_set::const_reference, int const&>::value));
  BOOST_TEST((boost::is_same< bt_set::const_reference, int const&>::value));

  BOOST_TEST((boost::is_same<std_set::iterator::reference, int const&>::value));
  BOOST_TEST((boost::is_same< bt_set::iterator::reference, int const&>::value));
  
  BOOST_TEST((boost::is_same<std_set::const_iterator::reference, int const&>::value));
  BOOST_TEST((boost::is_same< bt_set::const_iterator::reference, int const&>::value));

  typedef std::map<int, long>            std_map;
  typedef btree::btree_map<int, long>   bt_map;

  BOOST_TEST((boost::is_same<std_map::key_type, int>::value));
  BOOST_TEST((boost::is_same< bt_map::key_type, int>::value));
  
  BOOST_TEST((boost::is_same<std_map::mapped_type, long>::value));
  BOOST_TEST((boost::is_same< bt_map::mapped_type, long>::value));

  BOOST_TEST((boost::is_same<std_map::value_type, std::pair<const int, long> >::value));
  BOOST_TEST((boost::is_same< bt_map::value_type, btree::map_value<int, long> >::value));

  // reference is "lvalue of T" where T is value_type
  BOOST_TEST((boost::is_same<std_map::reference,
    std::pair<const int, long>& >::value));
  BOOST_TEST((boost::is_same< bt_map::reference,
    btree::map_value<int, long>& >::value));

  // const_reference is "const lvalue of T" where T is value_type
  BOOST_TEST((boost::is_same<std_map::const_reference,
    const std::pair<const int, long>& >::value));
  BOOST_TEST((boost::is_same< bt_map::const_reference,
    const btree::map_value<int, long>& >::value));

  BOOST_TEST((boost::is_same<std_map::iterator::reference,
    std::pair<const int, long>& >::value));
  BOOST_TEST((boost::is_same< bt_map::iterator::reference,
    const btree::map_value<int, long>& >::value));

  BOOST_TEST((boost::is_same<std_map::const_iterator::reference,
    const std::pair<const int, long>& >::value));
  BOOST_TEST((boost::is_same< bt_map::const_iterator::reference,
    const btree::map_value<int, long>& >::value));

  cout << "    types_test complete" << endl;
}

//-------------------------------------- construct_new ---------------------------------//

template <class BT>
void construct_new_test(BT& bt, const fs::path& p)
{ 
  BOOST_TEST(bt.is_open());
  BOOST_TEST_EQ(bt.size(), 0U);
  BOOST_TEST(bt.empty());
  BOOST_TEST(!bt.read_only());
  BOOST_TEST(bt.node_size() == btree::default_node_size);  // the default
  BOOST_TEST(bt.max_cache_size() ==  btree::default_max_cache_nodes);
  bt.max_cache_size(-1);
  BOOST_TEST(bt.max_cache_size() == static_cast<std::size_t>(-1));
  bt.max_cache_megabytes(100);
  BOOST_TEST(bt.max_cache_size() == 25600);
 
  BOOST_TEST(bt.file_path() == p);

  BOOST_TEST(bt.begin() == bt.end());

  typename BT::key_type k;
  BOOST_TEST(bt.lower_bound(k) == bt.end());
  BOOST_TEST(bt.upper_bound(k) == bt.end());
  BOOST_TEST(bt.find(k) == bt.end());

  bt.close();
  BOOST_TEST(!bt.is_open());
}

void construct_new()
{
  cout << "  construct_new..." << endl;
  {
    fs::path p("btree_map.btree");
    btree::btree_map<fat, int> x(p, btree::flags::truncate);
    construct_new_test(x, p);
  }

  cout << "    construct_new complete" << endl;
}

//-----------------------------------  single_insert  ----------------------------------//

void  single_insert()
{
  cout << "  single_insert..." << endl;

  {
    fs::path p("btree_map.btree");
    fs::remove(p);

    typedef btree::btree_map<int, int> btree_type;

    btree_type x(p, btree::flags::read_write, -1, 128);

    std::pair<btree_type::const_iterator, bool> result
      = x.emplace(123, 456);
    //x.dump_dot(std::cout);

    BOOST_TEST_EQ(x.size(), 1U);
    BOOST_TEST(result.second);
    BOOST_TEST_EQ(result.first->key(), 123);
    BOOST_TEST_EQ(result.first->mapped_value(), 456);
  }
 
  cout << "    single_insert complete" << endl;
}

//------------------------------------ open_existing -----------------------------------//

void open_existing()
{
  cout << "  open_existing..." << endl;
  fs::path p("btree_map.btree");

  {
    fs::remove(p);
    btree::btree_map<int, int> bt(p, btree::flags::truncate, -1, 128);

    int key = 5;
    int mapped = 0x55;
    bt.emplace(key, mapped);
    if (dump_dot)
      bt.dump_dot(std::cout);
    key = 4; mapped = 0x44;
    bt.emplace(key, mapped);
    if (dump_dot)
      bt.dump_dot(std::cout);
    key = 6; mapped = 0x66;
    bt.emplace(key, mapped);
    if (dump_dot)
      bt.dump_dot(std::cout);
  }

  {
    cout << "      try to open with wrong signature" << endl;
    bool signature_ok = false;
    try {btree::btree_map<int, int> bt2(p,btree::flags::read_only,0);}
    catch (...) { signature_ok = true; }
    BOOST_TEST(signature_ok);
  }
  {
    cout << "      try to open with wrong uniquenesss" << endl;
    bool uniqueness_ok = false;
    try {btree::btree_multimap<int, int> bt2(p);}
    catch (...) { uniqueness_ok = true; }
    BOOST_TEST(uniqueness_ok);
  }
  {
    cout << "      try to open with set/map conflict" << endl;
    bool set_vs_map_ok = false;
    try {btree::btree_set<int> bt2(p);}
    catch (...) { set_vs_map_ok = true; }
    BOOST_TEST(set_vs_map_ok);
  }
  {
    cout << "      try to open with key size conflict" << endl;
    bool key_size_ok = false;
    try {btree::btree_map<char, int> bt2(p);}
    catch (...) { key_size_ok = true; }
    BOOST_TEST(key_size_ok);
  }
  {
    cout << "      try to open with mapped_size conflict" << endl;
    bool mapped_size_ok = false;
    try {btree::btree_map<int, char> bt2(p);}
    catch (...) { mapped_size_ok = true; }
    BOOST_TEST(mapped_size_ok);
  }

  cout << "      verify header contents" << endl;
  btree::btree_map<int, int> bt2(p);
  BOOST_TEST(bt2.is_open());
  BOOST_TEST(!bt2.empty());
  BOOST_TEST_EQ(bt2.size(), 3U);
  BOOST_TEST_EQ(bt2.node_size(), 128U);
  BOOST_TEST_EQ(bt2.header().element_count(), 3U);
  BOOST_TEST_EQ(bt2.header().node_size(), 128U);
  if (dump_dot)
    bt2.dump_dot(std::cout);

  // TODO: test each header value

  cout << "    open_existing complete" << endl;
}

//--------------------------------- small_variable_set ---------------------------------//

void small_variable_set()
{
  cout << "  small_variable_set..." << endl;

  fs::path p("btree_set.btree");
  fs::remove(p);
  typedef btree::btree_set<btree::strbuf> btree_type;
  btree_type bt(p, btree::flags::truncate, -1, 128);
  std::pair<btree_type::iterator, bool> result;

  btree::strbuf stuff;

  stuff = "now";
  BOOST_TEST_EQ(boost::btree::dynamic_size(stuff), 5U);
  result = bt.insert(stuff);
  BOOST_TEST_EQ(bt.size(), 1U);
  BOOST_TEST(result.second);
  BOOST_TEST(std::strcmp(result.first->c_str(), "now") == 0 );
  //bt.dump_dot(std::cout);
  stuff = "is";
  bt.insert(stuff);
  //bt.dump_dot(std::cout);
  stuff = "the";
  bt.emplace(stuff);  // try the emplace() overload
  stuff = "time";
  bt.insert(stuff);
  stuff = "when";
  bt.insert(stuff);
  stuff = "all";
  bt.insert(stuff);
  stuff = "good";
  bt.insert(stuff);
  stuff = "...";
  bt.insert(stuff);

  BOOST_TEST(bt.is_open());
  BOOST_TEST(!bt.empty());
  BOOST_TEST_EQ(bt.size(), 8U);
  BOOST_TEST_EQ(bt.node_size(), 128U);
  BOOST_TEST_EQ(bt.header().element_count(), 8U);
  BOOST_TEST_EQ(bt.header().node_size(), 128U);

  if (dump_dot)
    bt.dump_dot(std::cout);

  cout << "    small_variable_set complete" << endl;
}

//--------------------------------- small_variable_map ---------------------------------//

void small_variable_map()
{
  cout << "  small_variable_map..." << endl;

  fs::path p("btree_map.btree");
  fs::remove(p);
  btree::btree_map<btree::strbuf, btree::strbuf> bt(p, btree::flags::truncate, -1, 128);

  btree::strbuf key;
  btree::strbuf value;

  key = "now";
  value = "won";
  bt.emplace(key, value);
  key = "is";
  value = "si";
  bt.emplace(key, value);
  key = "the";
  value = "eht";
  bt.emplace(key, value);
  key = "time";
  value = "emit";
  bt.emplace(key, value);
  key = "when";
  value = "nehw";
  bt.emplace(key, value);
  key = "all";
  value = "lla";
  bt.emplace(key, value);
  key = "good";
  value = "doog";
  bt.emplace(key, value);
  key = "...";
  value = "...";
  bt.emplace(key, value);

  BOOST_TEST(bt.is_open());
  BOOST_TEST(!bt.empty());
  BOOST_TEST_EQ(bt.size(), 8U);
  BOOST_TEST_EQ(bt.node_size(), 128U);
  BOOST_TEST_EQ(bt.header().element_count(), 8U);
  BOOST_TEST_EQ(bt.header().node_size(), 128U);

  if (dump_dot)
    bt.dump_dot(std::cout);

  cout << "    small_variable_map complete" << endl;
}

////------------------------------- btree_less ---------------------------------------------//
//
//void btree_less()
//{
//  cout << "  btree_less..." << endl;
//
//  const char* list[] = {"bb", "b", "a", "aa"};
//
//  for (int i = 0; i < 4; ++i) 
//    cout << list[i] << '\n';
//  cout << endl;
//
//  std::sort(&list[0], &list[4], btree::less<const char*>());
//
//  for (int i = 0; i < 4; ++i) 
//    cout << list[i] << '\n';
//
//  int i1=4;
//  int i2=3;
//  int i3=1;
//  int i4=2;
//  const int* list_int[] = {&i1, &i2, &i3, &i4};
//
//  for (int i = 0; i < 4; ++i) 
//    cout << *list_int[i] << '\n';
//  cout << endl;
//
//  std::sort(&list_int[0], &list_int[4], btree::less<const int*>());
//
//  for (int i = 0; i < 4; ++i) 
//    cout << *list_int[i] << '\n';
//  cout << endl;
//
//  cout << "    btree_less complete" << endl;
//}
//
////------------------------------- compare_function_objects -----------------------------//
//
//template <class BT>
//void compare_function_objects_test(BT& bt)
//{
//}
//
//void compare_function_objects()
//{
//  cout << "  compare_function_objects..." << endl;
//  {
//    btree::btree_set<int> bt;
//
//    int i2 = 2;
//    int i1 = 1;
//     
//    BOOST_TEST(bt.key_comp()(&i1, &i2));
//    BOOST_TEST(!bt.key_comp()(&i1, &i1));
//    BOOST_TEST(!bt.key_comp()(&i2, &i1));
//    BOOST_TEST(bt.value_comp()(&i1, &i2));
//    BOOST_TEST(!bt.value_comp()(&i1, &i1));
//    BOOST_TEST(!bt.value_comp()(&i2, &i1));
//  }
//
//  {
//    btree::btree_map<int, long> bt;
//
//    int i2 = 2;
//    int i1 = 1;
//
//    BOOST_TEST(bt.key_comp()(&i1, &i2));
//    BOOST_TEST(!bt.key_comp()(&i1, &i1));
//    BOOST_TEST(!bt.key_comp()(&i2, &i1));
//
//    struct my_pair : public btree::map_value<const int, const long>
//    {
//      int first;
//      int second;
//    };
//
//    my_pair pr2, pr1;
//    pr1.first = 1;
//    pr2.first = 2;
//
//    BOOST_TEST(bt.value_comp()(pr1, pr2));
////    BOOST_TEST(!bt.value_comp()(pr1, pr1));
////    BOOST_TEST(!bt.value_comp()(pr2, pr1));
//  }
//
//  //{
//  //  btree::btree_multimap x;
//  //}
//  //{
//  //  btree::btree_multiset x;
//  //}
//
//  cout << "    compare_function_objects complete" << endl;
//}

////------------------------------------ alignment ---------------------------------------//
//
//void alignment()
//{
//  cout << "  alignment..." << endl;
//  cout << "    alignment complete" << endl;
//}

//------------------------------------- insert -----------------------------------------//

template <class BTree>
void insert_tests(BTree& bt)
{
  cout << "    testing \"" << bt.file_path().string() << "\" ..." << endl;
  //cout << '\n' << bt.manager() << '\n';

  BOOST_TEST(bt.size() == 0U);
  BOOST_TEST(bt.empty());
  BOOST_TEST(!bt.read_only());
  BOOST_TEST(bt.node_size() == 128);
  BOOST_TEST(bt.begin() == bt.end());

  typename BTree::const_iterator empty_iterator, begin, end, cur;
  begin = bt.begin();
  end = bt.end();
  BOOST_TEST(begin == end);
  BOOST_TEST(bt.find(fat(0)) == bt.end());

  typename BTree::key_type key(0x0C);
  typename BTree::mapped_type mapped_value(0xCCCCCCCC);

  std::pair<typename BTree::const_iterator, bool> result;

  result = bt.emplace(key, mapped_value);
  BOOST_TEST(result.second);
  BOOST_TEST(result.first->key() == key);
  BOOST_TEST_EQ(result.first->mapped_value(), mapped_value);
  BOOST_TEST(bt.size() == 1U);
  BOOST_TEST(!bt.empty());
  BOOST_TEST(bt.begin() != bt.end());
  cur = bt.find(key);
  BOOST_TEST(cur != bt.end());
  BOOST_TEST(cur->key() == key);
  BOOST_TEST(cur->mapped_value() == mapped_value);
  BOOST_TEST(bt.find(fat(0)) == bt.end());
  BOOST_TEST(bt.find(fat(1000)) == bt.end());

  key = 0x0A;
  mapped_value = 0xAAAAAAAA;
  result = bt.emplace(key, mapped_value);
  BOOST_TEST(result.second);
  BOOST_TEST_EQ(result.first->key().x, key.x);
  BOOST_TEST_EQ(result.first->mapped_value(), mapped_value);
  BOOST_TEST_EQ(bt.find(0x0A)->key().x, 0x0A);
  BOOST_TEST_EQ(bt.find(0x0C)->key().x, 0x0C);

  key = 0x0E;
  mapped_value = 0xEEEEEEEE;
  result = bt.emplace(key, mapped_value);
  BOOST_TEST(result.second);
  BOOST_TEST_EQ(result.first->key().x, key.x);
  BOOST_TEST_EQ(result.first->mapped_value(), mapped_value);
  BOOST_TEST_EQ(bt.find(0x0E)->key().x, 0x0E);
  BOOST_TEST_EQ(bt.find(0x0A)->key().x, 0x0A);
  BOOST_TEST_EQ(bt.find(0x0C)->key().x, 0x0C);

  key = 0x0B;
  mapped_value = 0xBBBBBBBB;
  result = bt.emplace(key, mapped_value);
  BOOST_TEST(result.second);
  BOOST_TEST_EQ(result.first->key().x, key.x);
  BOOST_TEST_EQ(result.first->mapped_value(), mapped_value);
  BOOST_TEST_EQ(bt.find(0x0B)->key().x, 0x0B);
  BOOST_TEST_EQ(bt.find(0x0E)->key().x, 0x0E);
  BOOST_TEST_EQ(bt.find(0x0A)->key().x, 0x0A);
  BOOST_TEST_EQ(bt.find(0x0C)->key().x, 0x0C);

  key = 0x0D;
  mapped_value = 0xDDDDDDDD;
  result = bt.emplace(key, mapped_value);
  BOOST_TEST(result.second);
  BOOST_TEST_EQ(result.first->key().x, key.x);
  BOOST_TEST_EQ(result.first->mapped_value(), mapped_value);
  BOOST_TEST_EQ(bt.find(0x0D)->key().x, 0x0D);
  BOOST_TEST_EQ(bt.find(0x0B)->key().x, 0x0B);
  BOOST_TEST_EQ(bt.find(0x0E)->key().x, 0x0E);
  BOOST_TEST_EQ(bt.find(0x0A)->key().x, 0x0A);
  BOOST_TEST_EQ(bt.find(0x0C)->key().x, 0x0C);
  bt.flush();

  BOOST_TEST_EQ(bt.size(), 5U);

  BOOST_TEST_EQ(bt.find(0x0A)->key().x, 0x0A);
  BOOST_TEST_EQ(bt.find(0x0B)->key().x, 0x0B);
  BOOST_TEST_EQ(bt.find(0x0C)->key().x, 0x0C);
  BOOST_TEST_EQ(bt.find(0x0D)->key().x, 0x0D);
  BOOST_TEST_EQ(bt.find(0x0E)->key().x, 0x0E);

  result.first = cur = begin = end = empty_iterator;

  //bt.manager().dump_buffers(cout);
  //bt.manager().dump_available_buffers(cout);

  //cout << '\n' << bt.manager() << '\n';
  //cout << "\nroot is node " << bt.header().root_node_id() << ", size() " << bt.size() << '\n'; 
//  bt.dump_dot(std::cout);

  cur = bt.begin();
  //cout << "after begin():\n";
  //bt.manager().dump_buffers(cout);
  //bt.manager().dump_available_buffers(cout);

  BOOST_TEST_EQ(cur->key().x, 0x0A);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));
  ++cur;
  //cout << "after ++:\n";
  //bt.manager().dump_buffers(cout);
  //bt.manager().dump_available_buffers(cout);
  BOOST_TEST_EQ(cur->key().x, 0x0B);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));
  ++cur;
  BOOST_TEST_EQ(cur->key().x, 0x0C);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));
  ++cur;
  BOOST_TEST_EQ(cur->key().x, 0x0D);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));
  ++cur;
  BOOST_TEST_EQ(cur->key().x, 0x0E);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));
  ++cur;
  BOOST_TEST(cur == bt.end());
  //cout << "at end:\n";
  //bt.manager().dump_buffers(cout);
  //bt.manager().dump_available_buffers(cout);
  --cur;
  BOOST_TEST_EQ(cur->key().x, 0x0E);
  --cur;
  BOOST_TEST_EQ(cur->key().x, 0x0D);
  --cur;
  BOOST_TEST_EQ(cur->key().x, 0x0C);
  --cur;
  BOOST_TEST_EQ(cur->key().x, 0x0B);
  --cur;
  BOOST_TEST_EQ(cur->key().x, 0x0A);
  BOOST_TEST(cur == bt.begin());

  BOOST_TEST_EQ(bt.last()->key().x, 0x0E);

  // erase tests

  cout << "    erase all elements" << endl;
  cur = bt.find(0x0C);
  BOOST_TEST(cur != bt.end());
  //cout << "    erase " << cur->key() << endl;
  cur = bt.erase(cur);

  BOOST_TEST_EQ(cur->key().x, 0x0D);
  BOOST_TEST_EQ(bt.size(), 4U);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));

  cur = bt.find(0x0B);
  BOOST_TEST(cur != bt.end());
  //cout << "    erase " << cur->key() << endl;
  cur = bt.erase(cur);

  BOOST_TEST_EQ(cur->key().x, 0x0D);
  BOOST_TEST_EQ(bt.size(), 3U);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));

  cur = bt.find(0x0E);
  BOOST_TEST(cur != bt.end());

  //cout << "root is node " << bt.header().root_node_id() << '\n'; 
  //bt.dump_dot(std::cout);

  //cout << "    erase " << cur->key() << endl;
  cur = bt.erase(cur);

  BOOST_TEST(cur == bt.end());
  BOOST_TEST_EQ(bt.size(), 2U);
  BOOST_TEST_EQ(bt.header().root_node_id(), 2U);
  BOOST_TEST_EQ(bt.header().root_level(), 1U);

  cur = bt.find(0x0A);
  BOOST_TEST(cur != bt.end());
  //cout << "    erase " << cur->key() << endl;
  cur = bt.erase(cur);

  BOOST_TEST(cur != bt.end());
  BOOST_TEST_EQ(cur->key().x, 0x0D);
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));
  BOOST_TEST(bt.begin() == cur);
  BOOST_TEST_EQ(bt.size(), 1U);
  BOOST_TEST_EQ(bt.header().root_node_id(), 4U);
  BOOST_TEST_EQ(bt.header().root_level(), 0U);

  cur = bt.find(0x0D);

  //cout << "root is node " << bt.header().root_node_id() << '\n'; 
  //bt.dump_dot(std::cout);

  BOOST_TEST(cur != bt.end());
  BOOST_TEST(bt.inspect_leaf_to_root(cout, cur));

  //cout << "    erase " << cur->key() << endl;
  cur = bt.erase(cur);

  BOOST_TEST(cur == bt.end());
  BOOST_TEST(bt.begin() == bt.end());
  BOOST_TEST_EQ(bt.size(), 0U);
  BOOST_TEST_EQ(bt.header().root_node_id(), 4U);
  BOOST_TEST_EQ(bt.header().root_level(), 0U);
  
  //cout << "root is node " << bt.header().root_node_id() << '\n'; 
  //bt.dump_dot(std::cout);

  cout << "    add enough elements to force branch node splits" << endl;
  for (int i = 1; i <= 21; ++i )
  {
// std::cout << "\n inserting " << i << std::endl;
    key = i;
    mapped_value = i * 100;
    bt.emplace(key, mapped_value);
//std::cout << "root is node " << bt.header().root_node_id() << '\n'; 
  //bt.dump_dot(std::cout);
  }
  BOOST_TEST_EQ(bt.size(), 21U);
  
  //cout << "root is node " << bt.header().root_node_id() << '\n'; 
  //bt.dump_dot(std::cout);

  cout << "    erase every other element" << endl;
  for (int i = 1; i <= 21; i += 2 )
  {
    BOOST_TEST_EQ(bt.erase(i), 1U);
    BOOST_TEST_EQ(bt.erase(i), 0U);
  }
  BOOST_TEST_EQ(bt.size(), 10U);

  //cout << "root is node " << bt.header().root_node_id() << '\n'; 
  //bt.dump_dot(std::cout);

  cout << "    erase remaining elements and attempt to erase nonexistant elements" << endl;
  for (int i = 1; i <= 31; ++i )  // many of these won't exist
  {
    //cout << "\n  erase " << i << endl;
    typename BTree::size_type count_result = bt.count(i);
    typename BTree::size_type erase_result = bt.erase(i);
    BOOST_TEST_EQ(count_result, erase_result);
    //cout << "     erase count " << ct << ", size() " << bt.size() << endl;
    //cout << "root is node " << bt.header().root_node_id() << '\n'; 
    //bt.dump_dot(std::cout);
  }
  BOOST_TEST_EQ(bt.size(), 0U);

  bt.flush();
//  cout << '\n' << bt << '\n';

  cout << "    testing \"" << bt.file_path().string() << "\" complete" << endl;
}

void insert()
{
  cout << "    insert..." << endl;

  //  these tests use a value type that is large relative to the node size, thus stressing
  //  the code by causing a lot of node splits 

  {
    fs::path map_path("btree_map.btree");
    btree::btree_map<fat, int> map(map_path, btree::flags::truncate, -1, 128);
    map.max_cache_size(0);  // maximum stress
    insert_tests(map);
  }

  cout << "      insert complete" << endl;
}

//---------------------------------- find_and_bounds -----------------------------------//

typedef btree::btree_set<int> fb_set_type;
typedef btree::btree_multiset<int> fb_multiset_type;
typedef btree::btree_map<fat, int> fb_map_type;
typedef btree::btree_multimap<fat, int> fb_multimap_type;

void do_fb_insert(fb_set_type& bt, int i)
{
  bt.insert(i);
}
void do_fb_insert(fb_multiset_type& bt, int i)
{
  bt.insert(i);
}
void do_fb_insert(fb_map_type& bt, int i)
{
  fat k(i);
  bt.emplace(k, i*100);
}
void do_fb_insert(fb_multimap_type& bt, int i)
{
  fat k(i);
  bt.emplace(k, i*100);
}

template <class BTree>
void find_and_bounds_tests(BTree& bt)
{
  cout << "    testing \"" << bt.file_path().string() << "\" ..." << endl;

  for (int i = 17; i > 0; i -= 2)
  {
    do_fb_insert(bt, i);
    //std::cout << "   size is " << bt.size() << std::endl;
    //bt.dump_dot(std::cout);
  }

  BOOST_TEST_EQ(bt.size(), 9U);

  if (!(bt.header().flags() & btree::flags::unique))
  {
    //cout << "root is node " << bt.header().root_node_id() << '\n'; 
    //bt.dump_dot(std::cout);
    //cout << "insert 3" << endl;
    do_fb_insert(bt, 3);
    //cout << "root is node " << bt.header().root_node_id() << '\n'; 
    //bt.dump_dot(std::cout);
    //cout << "insert 7" << endl;
    do_fb_insert(bt, 7);
    //cout << "root is node " << bt.header().root_node_id() << '\n'; 
    //bt.dump_dot(std::cout);
    //cout << "insert 7" << endl;
    do_fb_insert(bt, 7);
    for (int i = 0; i < 10; ++i)
    {
      //cout << "root is node " << bt.header().root_node_id() << '\n'; 
      //bt.dump_dot(std::cout);

      //cout << "insert 15" << endl;
      do_fb_insert(bt, 15);
    }
    //cout << "root is node " << bt.header().root_node_id() << '\n'; 
    //bt.dump_dot(std::cout);


    BOOST_TEST_EQ(bt.size(), 22U);
  }

  //cout << "root is node " << bt.header().root_node_id() << '\n'; 
  if (dump_dot)
    bt.dump_dot(std::cout);

  //             i =   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18
  const int  lwr[] = { 1, 1, 3, 3, 5, 5, 7, 7, 9, 9, 11, 11, 13, 13, 15, 15, 17, 17, -1};
  const int  upr[] = { 1, 3, 3, 5, 5, 7, 7, 9, 9,11, 11, 13, 13, 15, 15, 17, 17, -1, -1};
  const int  fnd[] = {-1, 1,-1, 3,-1, 5,-1, 7,-1, 9, -1, 11, -1, 13, -1, 15, -1, 17, -1};
  const unsigned
             cnt[] = { 0, 1, 0, 2, 0, 1, 0, 3, 0, 1,  0,  1,  0,  1,  0, 11,  0,  1,  0};

  for (int i = 0; i <= 18; ++i)
  {
    //std::cout << "lower_bound " << i << " is " <<
    //  (bt.lower_bound(i) == bt.end() ? 99999 : bt.key(*bt.lower_bound(i))) << std::endl;


    BOOST_TEST((bt.lower_bound(i) != bt.end() && bt.key(*bt.lower_bound(i)) == lwr[i])
      || (bt.lower_bound(i) == bt.end() && lwr[i] == -1));

    BOOST_TEST((bt.upper_bound(i) != bt.end() && bt.key(*bt.upper_bound(i)) == upr[i])
      || (bt.upper_bound(i) == bt.end() && upr[i] == -1));

    BOOST_TEST((bt.find(i) != bt.end() && bt.key(*bt.find(i)) == fnd[i])
      || (bt.find(i) == bt.end() && fnd[i] == -1));

    BOOST_TEST(bt.lower_bound(i) == bt.end()
      || bt.inspect_leaf_to_root(cout, bt.lower_bound(i)));

    BOOST_TEST(bt.upper_bound(i) == bt.end()
      || bt.inspect_leaf_to_root(cout, bt.upper_bound(i)));

    BOOST_TEST(bt.find(i) == bt.end()
      || bt.inspect_leaf_to_root(cout, bt.find(i)));

    if (bt.header().flags() & btree::flags::unique)
      BOOST_TEST(bt.count(i) == (cnt[i] ? 1 : 0));
    else
      BOOST_TEST(bt.count(i) == cnt[i]);

//    cout << "      i = " << i << ", bt.count(i) = " << bt.count(i) <<endl;
  }

  cout << "    testing \"" << bt.file_path().string() << "\" complete" << endl;
}

void find_and_bounds()
{
  cout << "  find_and_bounds..." << endl;

  {
    fb_set_type set("find_and_bounds_set.btr", btree::flags::truncate, -1, 128);
    BOOST_TEST(set.header().flags() & btree::flags::unique);
    BOOST_TEST(set.header().flags() & btree::flags::key_only);
    set.max_cache_size(0);  // maximum stress
    find_and_bounds_tests(set);
  }

  {
    fb_multiset_type multiset("find_and_bounds_multiset.btr",
      btree::flags::truncate, -1, 128);
    BOOST_TEST(!(multiset.header().flags() & btree::flags::unique));
    BOOST_TEST(multiset.header().flags() & btree::flags::key_only);
    multiset.max_cache_size(0);  // maximum stress
    find_and_bounds_tests(multiset);
  }

  //  these tests use a value type that is large relative to the node size, thus stressing
  //  the code by causing a lot of node splits 

  {
    fb_map_type map("find_and_bounds_map.btr",
      btree::flags::truncate, -1, 128);
    BOOST_TEST(map.header().flags() & btree::flags::unique);
    BOOST_TEST(!(map.header().flags() & btree::flags::key_only));
    map.max_cache_size(0);  // maximum stress
    find_and_bounds_tests(map);
  }

  {
    fb_multimap_type multimap("find_and_bounds_multimap.btr",
      btree::flags::truncate, -1, 128);
    BOOST_TEST(!(multimap.header().flags() & btree::flags::unique));
    BOOST_TEST(!(multimap.header().flags() & btree::flags::key_only));
    multimap.max_cache_size(0);  // maximum stress
    find_and_bounds_tests(multimap);
  }

  cout << "    find_and_bounds complete" << endl;
}

//---------------------------------- insert_non_unique -----------------------------------//

template <class BTree>
void insert_non_unique_tests(BTree& bt)
{
  BOOST_TEST(!(bt.header().flags() & btree::flags::unique));
  
  typename BTree::const_iterator result;

  const int n = 12;
  cout << "  testing with " << n << " equal elements ..." << endl;

  fat k;

  for (int i = 1; i <= n; ++i)
  {
    result = bt.emplace(k = 3, i);
    BOOST_TEST_EQ(bt.size(), static_cast<unsigned>(i));
    BOOST_TEST_EQ(result->key().x, 3);
    BOOST_TEST_EQ(result->mapped_value(), i);

    //cout << "root is node " << bt.header().root_node_id() << '\n'; 
    //bt.dump_dot(std::cout);
   
    int j = 0;
    for (typename BTree::const_iterator_range range = bt.equal_range(3);
         range.first != range.second; ++range.first)
    {
      //cout << range.first->key().x << ", " << range.first->mapped_value() << endl;
      ++j;
      BOOST_TEST_EQ(range.first->key().x, 3);
      BOOST_TEST_EQ(range.first->mapped_value(), j);
    }
    BOOST_TEST_EQ(j, i);
  }

  cout << "  testing \"" << bt.file_path().string() << "\" complete" << endl;
}

void insert_non_unique()
{
  cout << "  insert_non_unique..." << endl;

  //  these tests use a value type that is large relative to the node size, thus stressing
  //  the code by causing a lot of node splits 

  {
    fs::path map_path("non_unique.btr");
    btree::btree_multimap<fat, int> multimap(map_path,
      btree::flags::truncate, -1, 128);
    multimap.max_cache_size(0);  // maximum stress
    insert_non_unique_tests(multimap);
  }

  cout << "    insert_non_unique complete" << endl;
}

//--------------------------------  update_test  --------------------------------------//

void update_test()
{
  cout << "  update_test..." << endl;

  typedef btree::btree_map<fat, int> bt_type;
  bt_type bt("update.btr", btree::flags::truncate, -1, 128);

  boost::mt19937 rng;
  boost::uniform_int<> million(1,1000000);
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
           random_value(rng, million);

  for (int n = 1; n < 1000; ++n)
  {
    bt.emplace(fat(random_value()), n);
  }

  for (bt_type::iterator itr = bt.begin(); itr != bt.end(); ++itr)
  {
//    cout << "    " << itr->key() << "," << itr->mapped_value() << '\n';
    int n = itr->mapped_value();
    bt_type::iterator new_itr =  bt.update(itr, -n);
    BOOST_TEST(itr == new_itr);
    BOOST_TEST(new_itr->mapped_value() == -n);
  }

  for (bt_type::iterator itr = bt.begin(); itr != bt.end(); ++itr)
  {
//    cout << "    " << itr->key() << "," << itr->mapped_value() << '\n';
    BOOST_TEST(itr->mapped_value() <= 0);
  }

  cout << "     update_test complete" << endl;
}

////-------------------------------------- erase -----------------------------------------//
//
//void erase()
//{
//  cout << "  erase..." << endl;
//  cout << "    erase complete" << endl;
//}
//
////------------------------------------ iteration ---------------------------------------//
//
//void iteration()
//{
//  cout << "  iteration..." << endl;
//  cout << "    iteration complete" << endl;
//}
//
////-------------------------------------  multi -----------------------------------------//
//
//void  multi()
//{
//  cout << "   multi..." << endl;
//  cout << "     multi complete" << endl;
//}
//
////--------------------------- parent_pointer_to_split_node -----------------------------//
//
//void parent_pointer_to_split_node()
//{
//  cout << "  parent_pointer_to_split_node..." << endl;
//  cout << "    parent_pointer_to_split_node complete" << endl;
//}
//
////----------------------------- parent_pointer_lifetime --------------------------------//
//
//void parent_pointer_lifetime()
//{
//  cout << "  parent_pointer_lifetime..." << endl;
//  cout << "    parent_pointer_lifetime complete" << endl;
//}

//-------------------------------- pack_optimization -----------------------------------//

void pack_optimization()
{
  cout << "  pack_optimization..." << endl;

  typedef btree::btree_multiset<fat> set_type;
  typedef set_type::value_type value_type;
  const int node_sz = 128;
  const unsigned n_levels = 5;  // enough to exercise branch packing

  set_type np("not_packed.btr", btree::flags::truncate, -1, node_sz);

  for (int i=2034875; np.header().levels() < n_levels;
    i = (i*1234567891) + 11) // avoid ordered values
  {
    set_type::iterator itr = np.insert(fat(i));
  }

  //if (dump_dot)
  //  np.dump_dot(std::cerr);
  
  set_type p("packed.btr", btree::flags::truncate, -1, node_sz);
  for (set_type::const_iterator it = np.begin(); it != np.end(); ++it)
  {
    set_type::const_iterator it2 = p.insert(*it);
    BOOST_TEST(p.inspect_leaf_to_root(cout, it2));
  }

  //if (dump_dot)
  //  p.dump_dot(std::cerr);

  BOOST_TEST_EQ(np.size(), p.size());
  BOOST_TEST(p.header().node_count() < np.header().node_count());
  BOOST_TEST(p.header().leaf_node_count() < np.header().leaf_node_count());
  BOOST_TEST(p.header().branch_node_count() < np.header().branch_node_count());

  cout << "      " << np.header().branch_node_count() << " branch nodes before pack, "
       << p.header().branch_node_count() << " branch nodes after pack\n"
       << "      " << np.header().leaf_node_count() << " leaf nodes before pack, "
       << p.header().leaf_node_count() << " leaf nodes after pack\n"
       << "      " << np.header().levels() << " levels before pack, "
       << p.header().levels() << " levels after pack\n";

  cout << "    pack_optimization complete" << endl;
}

////-------------------------------------  fixstr ----------------------------------------//
//
//void  fixstr()
//{
//  cout << "  fixstr..." << endl;
//
//  typedef boost::detail::fixstr<15>       str_t;
//  typedef btree::btree_map<str_t, str_t>  map_t;
//
//  map_t bt("fixstr.btr", btree::flags::truncate);
//
//  bt.insert(make_pair(str_t("Tyler"), str_t("jet black")));
//  bt.insert(make_pair(str_t("Harry"), str_t("black & white")));
//
//  map_t::const_iterator it = bt.begin();
//  BOOST_TEST(it->first == "Harry");
//  BOOST_TEST(it->second == "black & white");
//  ++it;
//  BOOST_TEST(it->first == "Tyler");
//  BOOST_TEST(it->second == "jet black");
//  ++it;
//  BOOST_TEST(it == bt.end());
//
//  cout << "     fixstr complete" << endl;
//}

//-----------------------------  reopen_btree_object_test  -----------------------------//

//  the use case reproduced here failed in another program

void  reopen_btree_object_test()
{
  cout << "  reopen_btree_object_test..." << endl;

  const int n = 1;
  cout << "    inserting " << n << " btree elements..." << endl;

  std::string path("reopen.btree");
  std::string path2("reopen.btree.2");
  typedef btree::btree_map<long, long> map_type;
  map_type bt(path, btree::flags::truncate, -1, 128);
  rand48  rng;
  uniform_int<long> n_dist(0, n);
  variate_generator<rand48&, uniform_int<long> > key(rng, n_dist);
  const int seed = 1;
  rng.seed(seed);

  for (long i = 1; i <= n; ++i)
  {
    bt.emplace(key(), i);
  }

  cout << "    copying " << bt.size() << " elements..." << endl;

  bt.close();
  fs::remove(path2);
  fs::rename(path, path2);
  map_type bt2(path2);
  bt.open(path, btree::flags::truncate, -1, 128);
  for (map_type::iterator it = bt2.begin(); it != bt2.end(); ++it)
    bt.emplace(it->key(), it->mapped_value());
  BOOST_TEST_EQ(bt.size(), bt2.size());
  bt2.close();
  bt.close();
  cout << "  " << path2 << " file_size: " << fs::file_size(path2) << '\n';
  cout << "  " << path << "  file_size: " << fs::file_size(path) << '\n';
  BOOST_TEST_EQ(fs::file_size(path2), fs::file_size(path));

  cout << "     reopen_btree_object_test complete" << endl;
}


//--------------------------------  cache_size_test  -----------------------------------//

void  cache_size_test()
{
  cout << "  cache_size_test..." << endl;

  typedef fat value_type;
  const int node_sz = 128;
  const unsigned n_levels = 5;  // enough to exercise both shallow and deep trees
  const std::size_t cache_max = 24;

  btree::btree_multiset<fat> bt("cache_size_test.btr", btree::flags::truncate, -1, node_sz);
  BOOST_TEST(bt.manager().buffers_in_memory() == 1);  // the root is cached
  BOOST_TEST(bt.manager().buffers_available() == 0);  // the root buffer is not available
  
  bt.max_cache_size(cache_max);
  
  cout << "    inserting..." << endl;
  for (int i=2034875;
    bt.header().levels() < n_levels;
    i = (i*1234567891) + 11) // avoid ordered values
  {
    { 
//      cout << "     inserting " << bt.size() + 1 << ", key " << i << endl;
      btree::btree_multiset<fat>::iterator itr = bt.insert(fat(i));
      BOOST_TEST(bt.inspect_leaf_to_root(cout, itr));
      // There is one iterator in existance, so one buffer per level should be in memory
      // in addition to any available buffers
      BOOST_TEST(itr.use_count() == 1 || (bt.header().levels() == 1 && itr.use_count() == 2));
      BOOST_TEST(bt.manager().buffers_in_use() == bt.header().levels());

      if (bt.manager().buffers_in_use()
        != bt.header().levels())
      {
        cout << "     after insert():"
          << ", use count=" << itr.use_count()
          << ", levels=" << bt.header().levels()
          << ", cache size=" << bt.manager().buffers_in_memory()
          << ", cache active=" << bt.manager().buffers_in_memory()
                                    - bt.manager().buffers_available()
          << ", cache avail=" << bt.manager().buffers_available()
          << ", branch pages =" << bt.header().branch_node_count()
          << ", leaf pages =" << bt.header().leaf_node_count()
          << ", size=" << bt.size()
          << endl;
        bt.manager().dump_buffers(cout);
        bt.manager().dump_available_buffers(cout);
        bt.dump_dot(std::cout);
      }
    }
    // there are now no iterators in existance, so only the root buffer should be held
    BOOST_TEST(bt.manager().buffers_in_use() == 1);
    if (bt.manager().buffers_in_use() != 1)
    {
      cout << "       after insert block exit:" << bt.header().levels()
        << ", cache size=" << bt.manager().buffers_in_memory()
        << ", cache available=" << bt.manager().buffers_available()
        << ", branch pages =" << bt.header().branch_node_count()
        << ", leaf pages =" << bt.header().leaf_node_count()
        << ", size=" << bt.size()
        << endl;

      bt.manager().dump_buffers(cout);
      bt.manager().dump_available_buffers(cout);
      bt.dump_dot(std::cout);
    }
  }

  cout << "    erasing..." << endl;
  for (btree::btree_multiset<fat>::iterator itr = bt.begin();  itr != bt.end();)
  {
    BOOST_TEST(bt.inspect_leaf_to_root(cout, itr));
    BOOST_TEST(itr == bt.begin());
    //cout << "      erasing " << *itr << endl;
    itr = bt.erase(itr);
  }

  bool ok=true;
  BOOST_ASSERT(ok = (bt.manager().buffers_in_memory() <= cache_max + 1));
  if (!ok)
  {
    cout << bt;
    cout << bt.manager();
    cout << endl;
    bt.dump_dot(std::cerr);
  }

  cout << "    cache_size_test complete" << endl;
}

//-------------------------------------  _test  ----------------------------------------//

void  _test()
{
  cout << "  _test..." << endl;

  cout << "     _test complete" << endl;
}

}  // unnamed namespace

//------------------------------------ cpp_main ----------------------------------------//

int cpp_main(int argc, char* argv[])
{
  std::string command_args;

  for (int a = 0; a < argc; ++a)
  {
    command_args += argv[a];
    if (a != argc-1)
      command_args += ' ';
  }

  cout << command_args << '\n';;

  for (; argc > 1; ++argv, --argc) 
  {
    if ( std::strcmp( argv[1]+1, "b" )==0 )
      dump_dot = true;
    else
    {
      cout << "Error - unknown option: " << argv[1] << "\n\n";
      argc = -1;
      break;
    }
  }

  if (argc < 1) 
  {
    cout << "Usage: btree_unit_test [Options]\n"
//      " The argument n specifies the number of test cases to run\n"
      " Options:\n"
//      "   path     Specifies the test file path; default test.btree\n"
      "   -d       Dump tree using Graphviz dot format; default is no dump\n"
      ;
    return 1;
  }

  instantiate();
  types_test();
  //btree_less();
  //compare_function_objects();
  construct_new();
  single_insert();
  open_existing();
  small_variable_set();
  small_variable_map();
  //alignment();
  insert();
  insert_non_unique();
  find_and_bounds();
  //erase();
  update_test();
  //iteration();
  //multi();
  //parent_pointer_to_split_node();
  //parent_pointer_lifetime();
  pack_optimization();
  reopen_btree_object_test();
  //fixstr();
  cache_size_test();
  

  //{
  //  cout << "btree_multimap tests..." << endl;
  //  fs::path multimap_path("btree_multimap.btree");
  //  btree::btree_multimap<int,long> multimap(multimap_path, btree::flags::truncate);
  //}

  //{
  //  cout << "btree_set tests..." << endl;
  //  fs::path set_path("btree_set.btree");
  //  btree::btree_set<int,long> set(set_path, btree::flags::truncate);
  //}

  //{
  //  cout << "btree_multiset tests..." << endl;
  //  fs::path multiset_path("btree_multiset.btree");
  //  btree::btree_multiset<int,long> multiset(multiset_path, btree::flags::truncate);
  //}

  cout << "all tests complete" << endl;

  return boost::report_errors();
}

