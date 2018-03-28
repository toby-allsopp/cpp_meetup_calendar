// boost_range.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/adaptor/argument_fwd.hpp>
#include <boost/range/adaptor/transformed.hpp>

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

auto dates_in_year(int year)
{
    return boost::iterator_range<day_iterator>(
        day_iterator{date{date::year_type(year), greg::Jan, 1}},
        day_iterator{date{date::year_type(year + 1), greg::Jan, 1}});
}

template <typename Iterator, typename Predicate>
class group_by_iterator : public boost::iterator_facade<
        group_by_iterator<Iterator, Predicate>,
        boost::iterator_range<Iterator>,
        boost::forward_traversal_tag,
        boost::iterator_range<Iterator>>
{
public:
    group_by_iterator() : _start{}, _current{}, _end{}
    {
    }

    explicit group_by_iterator(Predicate predicate, Iterator start, Iterator end) :
        _predicate(predicate),
        _start(start),
        _current(start),
        _end(end)
    {
        if (_start != _end)
            skip_until_predicate_becomes_false();
    }

private:
    friend class boost::iterator_core_access;

    void skip_until_predicate_becomes_false()
    {
        ++_current;
        while (_current != _end && _predicate(*_start, *_current))
            ++_current;
    }

    void increment()
    {
        _start = _current;
        if (_current != _end)
            skip_until_predicate_becomes_false();
    }

    bool equal(group_by_iterator const& other) const
    {
        // both must be compared!!!
        return this->_start == other._start &&
            this->_current == other._current;
    }

    boost::iterator_range<Iterator> dereference() const
    {
        return boost::iterator_range<Iterator>{_start, _current};
    }

    Predicate _predicate;
    Iterator _start;
    Iterator _current;
    Iterator _end;
};

template <typename Range, typename Predicate>
class group_by_range : public boost::iterator_range<
        group_by_iterator<typename boost::range_iterator<Range>::type, Predicate>>
{
    typedef typename boost::range_value<Range>::type value_type;
    typedef typename boost::range_iterator<Range>::type iterator_base;
    typedef group_by_iterator<typename boost::range_iterator<Range>::type, Predicate> grouped_by_iterator;
    typedef boost::iterator_range<grouped_by_iterator> base_t;

public:
    explicit group_by_range(Range& rng, Predicate predicate) : base_t(
        grouped_by_iterator(predicate, boost::begin(rng), boost::end(rng)),
        grouped_by_iterator(predicate, boost::end(rng), boost::end(rng)))
    {
    }
};

template <typename Predicate>
class group_by_holder : public boost::range_detail::holder<Predicate>
{
public:
    group_by_holder(Predicate predicate) : boost::range_detail::holder<Predicate>(predicate)
    {
    }

    void operator=(const group_by_holder&) = delete;
};

static boost::range_detail::forwarder<group_by_holder>
grouped_by = boost::range_detail::forwarder<group_by_holder>();

template <typename SinglePassRange, typename Predicate>
inline group_by_range<SinglePassRange, Predicate>
operator|(SinglePassRange& rng, group_by_holder<Predicate> const& predicate)
{
    return group_by_range<SinglePassRange, Predicate>(rng, predicate.val);
}

template <typename SinglePassRange, typename Predicate>
inline group_by_range<SinglePassRange, Predicate>
operator|(SinglePassRange const& rng, group_by_holder<Predicate> const& predicate)
{
    return group_by_range<SinglePassRange, Predicate>(rng, predicate.val);
}

auto by_month()
{
    return grouped_by([](date const& a, date const& b)
    {
        return a.month() == b.month();
    });
}

auto by_week()
{
    return grouped_by([](date const& a, date const& b)
    {
        return a.week_number() == b.week_number();
    });
}

auto month_by_week()
{
    return boost::adaptors::transformed([](auto month)
    {
        return month | by_week();
    });
}

int main()
{
    auto year = dates_in_year(2018);
    auto months = year | by_month() | month_by_week();
    for (auto const& month : months) // cannot be non-const reference!!!
    {
        std::cout << "===== " << month.front().front().month() << std::endl;
        for (auto const& week : month)
        {
            for (auto const& day : week | boost::adaptors::transformed([](date d) { return d.day(); }))
                std::cout << day << " ";
            std::cout << std::endl;
        }
    }
    return 0;
}
