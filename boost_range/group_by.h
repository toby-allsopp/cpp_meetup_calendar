#pragma once

#include <boost/range/adaptor/argument_fwd.hpp>
#include <boost/range/iterator_range.hpp>

template <typename Iterator, typename Predicate>
class group_by_iterator final : public boost::iterator_facade<
    group_by_iterator<Iterator, Predicate>,
    boost::iterator_range<Iterator>,
    boost::forward_traversal_tag,
    boost::iterator_range<Iterator>>
{
public:
    explicit group_by_iterator() : _start{}, _current{}, _end{}
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
class group_by_range final : public boost::iterator_range<
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
class group_by_holder final : public boost::range_detail::holder<Predicate>
{
public:
    explicit group_by_holder(Predicate predicate) : boost::range_detail::holder<Predicate>(predicate)
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
