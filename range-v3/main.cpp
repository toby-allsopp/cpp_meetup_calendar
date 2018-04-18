// Translation of example into range-v3
//

#include <boost/date_time/gregorian/gregorian.hpp>
#include <range/v3/all.hpp>

namespace greg = boost::gregorian;
using date = greg::date;
using day = greg::date_duration;

using namespace ranges;

#if 1
auto dates_in_year(int year)
{
  // Andrew rolled a custom iterator for this. We can combine some predefined views instead.
  const auto start = date{date::year_type(year), greg::Jan, 1};
  const auto end = date{date::year_type(year + 1), greg::Jan, 1};
  return view::iota(0)
      | view::transform([start](auto i) { return start + greg::days(i); })
      | view::take_while([end](auto date) { return date < end; });
}

#elif 0
// Here's a neat way to use a coroutine generator.
//
// NOTE this doesn't work because the group_by view adaptor requires a
// ForwardRange but coroutine generators can only be InputRanges.
#include <range/v3/experimental/utility/generator.hpp>

experimental::generator<date> dates_in_year(int year)
{
  for (date d = date{date::year_type(year), greg::Jan, 1};
       d < date{date::year_type(year + 1), greg::Jan, 1};
       d += greg::days(1)) {
    co_yield d;
  }
}

#elif 0
// Here's what Eric Niebler did in his example:

namespace boost
{
    namespace gregorian
    {
        date &operator++(date &d)
        {
            return d = d + day(1);
        }
        date operator++(date &d, int)
        {
            return ++d - day(1);
        }
    }
}
namespace ranges
{
    template<>
    struct difference_type<date>
    {
        using type = date::duration_type::duration_rep::int_type;
    };
}
CONCEPT_ASSERT(Incrementable<date>());

auto
dates_in_year(unsigned short year)
{
    return view::iota(date{year, greg::Jan, 1}, date{year+1, greg::Jan, 1});
}
#endif

#include <range/v3/experimental/utility/generator.hpp>

template <typename Rng, typename Pred>
experimental::generator<date> filter(Rng rng, Pred pred)
{
    for (auto x : rng)
        if (pred(x))
            co_yield x;
}

auto by_month()
{
  return view::group_by([](date const& a, date const& b)
    {
        return a.month() == b.month();
    });
}

auto by_week()
{
    return view::group_by([](date const& a, date const& b)
    {
        return a.week_number() == b.week_number();
    });
}

auto month_by_week()
{
    return view::transform([](auto month)
    {
        return month | by_week();
    });
}

int main()
{
    // const *does* compile...
    auto const year = dates_in_year(2018);

    auto const months = year | by_month() | month_by_week();
    for (auto const& month : months) // cannot be non-const reference!!!
    {
        std::cout << "===== " << month.front().front().month() << std::endl;
        for (auto const& week : month)
        {
            for (auto const& day : week | view::transform(&date::day))
                std::cout << day << " ";
            std::cout << std::endl;
        }
    }

    //// This one does not compile!!!
    //auto rng = year;
    //bool const switchOnlyMondays = true;
    //if (switchOnlyMondays)
    //    rng = rng | view::filter([](date d) { return d.day_of_week() == 0; });
    //for (auto const& day : rng)
    //    std::cout << day << std::endl;
    //// Which is obvious:
    ////boost::iterator_range<day_iterator> rng = year;
    ////boost::filtered_range<is_sunday, boost::iterator_range<day_iterator>> flt = 
    ////    rng | boost::adaptors::filtered(is_sunday{});

    any_view<date, category::input> rng{year};
    bool const switchOnlyMondays = true;
    if (switchOnlyMondays)
        //rng = rng | view::filter(is_sunday{});
    // This one does *not* crash
        rng = filter(rng, [](date const& d) { return d.day_of_week() == 0; });
    // Having "auto const& day" here does *not* crash!!!
    for (auto const& day : rng)
        std::cout << day << std::endl;

    return 0;
}
