#pragma once

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace greg = boost::gregorian;
using date = greg::date;
using day = greg::date_duration;

// Note boost::iota<> is completely different!
class day_iterator : public boost::iterator_facade<
    day_iterator,
    date,
    boost::forward_traversal_tag,
    date>
{
public:
    day_iterator() : _value()
    {
    }

    explicit day_iterator(date value) : _value(value)
    {
    }

private:
    friend class boost::iterator_core_access;

    void increment() { _value = _value + greg::days(1); }

    bool equal(day_iterator const& other) const { return this->_value == other._value; }

    date dereference() const { return _value; }

    date _value;
};

//TODO: show it with counting_iterator<date>
