#ifndef ACCEPTATRON_H
#define ACCEPTATRON_H

#include <cmath>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

/*

=encoding utf8

=head1 NAME

Acceptatron - TAP generator.

=head1 SYNOPSIS

 plan(4);

 ok(foo());

 std::stringstream ss;
 bar(nullptr, ss);
 ok(ss, "passing a nullptr to bar must not invalidate the stream");

 try {
 	quux(3);
	fail("no exception thrown");
 }
 catch (const std::exception& ex)
 {
 	pass("correct exception type");
 }
 catch (...)
 {
 	fail("wrong exception type thrown");
 }

 // std::hash<std::string> is implementation-defined, so there is no
 // guarantee this test will pass on any particular implementation
 std::hash<std::string> h;
 is(9631199822919835226ul, h("foo"));

 done_testing();

=head1 DESCRIPTION

Outputs
L<TAP version 13|https://testanything.org/tap-version-13-specification.html>.
The output can be processed by any TAP consumer, such as
L<C<prove>|http://perldoc.perl.org/prove.html> (when using C<prove>, be sure
to pass an empty string as the interpreter -- e.g., C<prove -e '' ./a.out>).
But an important property of TAP is that it doesn't need to be processed to
be useful.

TAP is meant to be very simple, and all output goes to standard
output.  In the Acceptatron, all output goes through C<std::cout>.  This can
occasionally cause trouble if the code being tested also needs to write to
standard output.  The Acceptatron had to solve this problem in its test suite.
In fact, not only does the Acceptatron need to write to standard output, but
it needs to write TAP to standard output, and some tests check that failed
tests are reported correctly.  We could have solved this by using a framework
that doesn't also need to write TAP to standard output.  Instead, we solved it
by forking a process and redirecting its output so that our tests can determine
if correct TAP is generated.

Not only is the Acceptatron a header-only library, it consists of a single
header: F<acceptatron.h>.

You can generate a PDF of this document by calling
C<<< perldoc -onroff acceptatron.h | groff -man | ps2pdf - F<filename> >>>.
However, by default, F<perldoc> assumes it is formatting Perl documentation,
and will add things such as the Perl version to the output.  The command
C<<< perldoc -onroff -w center:'' -w section:3 -w release:'' acceptatron.h |
groff -man | ps2pdf - F<filename> >>> will create a file without the
Perl-related material.

It is also possible to have F<perldoc> generate TeX output, if you like
manpages formatted by TeX.

=head2 About the name

In I<The Wheel of Time> series, some characters, known as Aes Sedai, are able
to use the One Power much like magic.  During the Age of Legends, some Aes
Sedai freed the Dark One while trying to tap what they thought was a massive
power source.  Lews Therin Telamon -- known as the Dragon -- led the fight
against the Dark One, and eventually reimprisoned him.

However the Dark One was able to leave a taint on the One Power, and any male
Aes Sedai will eventually become insane.  Female Aes Sedai continue the fight
against evil, perform research, serve rulers as advisors (although many rulers
aren't certain whose interests the advice is meant to serve), or hunt male Aes
Sedai.  Female Aes Sedai are formally trained and, to win some trust with
non-Aes Sedai, must swear magically-enforced oaths.

Women in training to become Aes Sedai are periodically tested.  The tests use
I<ter'angreal> that are never named in the stories (characters are summoned to
their tests, and they know a I<ter'angreal> will be used in that test, but it
is never named).  Fans began calling one of these I<ter'angreal> the
Acceptatron in online discussions.  I am unable to come up with a better
name for a testing framework.

=head2 Functions

All functions are in the C<acceptatron> namespace (functions documented as
being in the C<impl> namespace are, in fact, in the C<acceptatron::impl>
namespace).

Unlike C<std>, it is perfectly acceptable to specialize functions in
C<acceptatron> or C<acceptatron::impl>, add overloads, or even add unrelated
functions in your own code.  However, the Acceptatron uses argument dependent
lookup so that this is not necessary.  If you define functions such as
C<tap_format> in your own code, the Acceptatron will call your function instead
of its own.

Likewise, calls to C<equal_to> and C<to_string> use argument dependent lookup.
So it is possible to define either or both functions in your namespace and have
the tests work correctly.  Otherwise, the calls will forward to
C<std::equal_to> and C<std::to_string>.  This also means that if you need to
define either function for your code to work correctly with the standard
library, the Acceptatron will use your code as well; which should generally be
correct.

=over

=cut

*/

namespace acceptatron {

using std::equal_to;
using std::to_string;

namespace impl {
	bool plan_output = false;
	size_t failures = 0;
	size_t tests = 0;
	size_t to_todo = 0;
	std::string todo_msg;
	size_t planned_tests = 0;
	/*

=item B<<< C<templateE<lt>typename RealE<gt> Real impl::parse(const std::string& s)> >>>

Wrapper around C<std::stof>, C<std::stod> and C<std::stold> -- depending on
whether C<Real> is a C<float>, C<double> or C<long double>.

=cut

	*/

	template<typename Real> Real parse(const std::string& s);
	
	/*

=item B<<< C<templateE<lt>typename RealE<gt> std::string impl::dtoa(const Real& d)> >>>

Simple implementation of
L<David Gay's C<dtoa> function|http://www.netlib.org/fp/dtoa.c>.
Outputs C<d> using the shortest string that round trips.  That is, the output
of this function is the shortest string that can be parsed to recover the
original value of C<d>.

The implementation isn't efficient, and may fall back to C<to_string(d)>.
It also makes no effort to honor any localization requirements.

=cut
	*/
	template<typename Real> std::string dtoa(const Real& d);

	/*

=item B<<< C<templateE<lt>typename TE<gt> std::string impl::tap_format(const T& t)> >>>

Generally returns C<t> converted to a C<std::string> surrounded by single
quotes, e.g., C<tap_format(5)> returns C<'5'>, C<tap_format("foo")> returns
C<'foo'>.  Calls C<dtoa> for C<float>, C<double> or C<long double> values.

=cut

	*/
	template<typename T> std::string tap_format(const T& t);

	void do_test_output(bool pred, const std::string& msg,
			const std::string& failure);

	std::string append_location(std::string msg, const char* filename,
			int line);
}
using impl::tap_format;
using impl::do_test_output;

/*

=item B<C<void plan(size_t count)>>

Set the planned number of tests.

=cut

*/
void plan(size_t count);

/*

=item B<C<void done_testing(size_t count=0)>>

If C<count> is not 0, outputs a plan line giving how many tests should have
been run.  Exits.  Exit codes are:

=over

=item * 0: all tests succeeded, all expected tests were run

=item * 255: not all expected tests were run

=item * I<n>, 0 < I<n> < 255: all expected tests were run, but some failed
(exit code is C<std::min(254, number_of_failures)>).

=back

=cut

*/
void done_testing(size_t count=0);

/*

=item B<C<void skip_all(const std::string& reason)>>

Report that all tests will be skipped and exit with a successful status.
Should only be called if no tests have been executed.

=cut

*/
void skip_all(const std::string& reason);

/*

=item B<C<void diag(const std::string& msg)>>

Output C<msg> as a TAP comment.

=cut

*/
void diag(const std::string& msg);

/*

=item B<C<void note(const std::string& msg)>>

Output C<msg> as a TAP comment.

=cut

*/
void note(const std::string& msg);

/*

=item B<C<void ok(bool pred, const std::string& msg="")>>

Simplest test function.  Able to report success or failure, but few additional
details.

=cut

*/
void ok(bool pred, const std::string& msg="");

/*

=item B<<< C<templateE<lt>typename T, typename Pred=equal_toE<lt>TE<gt>E<gt> void is(const T& actual, const T& expected, const std::string& msg="", Pred pred=equal_toE<lt>TE<gt>())> >>>

Reports success if C<pred(actual, expected)> returns C<true>, and failure
otherwise.  By default, C<pred> is defined as C<actual == expected>.

=cut

*/
template<typename T, typename Pred=equal_to<T>> void is(const T& actual,
		const T& expected, const std::string& msg="",
		Pred pred=equal_to<T>());

/*

=item B<<< C<templateE<lt>typename T, typename Pred=equal_toE<lt>TE<gt>E<gt> void isnt(const T& actual, const T& expected, const std::string& msg="", Pred pred=equal_toE<lt>TE<gt>())> >>>

Reports success if C<pred(actual, expected)> returns C<false>, and failure
otherwise.

By default, C<pred> is defined as C<actual == expected>, so the test succeeds
if C<actual == expected> is C<false>.  This should be equivalent to
C<actual != expected>, but nothing in C++ requires that relationship.  If you
want to test C<actual != expected> explicitly, use C<cmp_ok>, e.g.,
C<<< cmp_ok(2, 3, "not a real test", std::not_equal_to<int>()) >>>.

=cut

*/
template<typename T, typename Pred=equal_to<T>> void isnt(const T& actual,
		const T& expected, const std::string& msg="",
		Pred pred=equal_to<T>());

/*

=item B<<< C<templateE<lt>typename T, typename PredE<gt> void like(const T& t, Pred pred, const std::string& msg="")> >>>

Reports success if C<pred(t)> returns C<true>, and failure otherwise.

=cut

*/
template<typename T, typename Pred> void like(const T& t, Pred pred,
		const std::string& msg="");

/*

=item B<<< C<templateE<lt>typename T, typename PredE<gt> void unlike(const T& t, Pred pred, const std::string& msg="")> >>>

Reports success if C<pred(t)> returns C<false>, and failure otherwise.

=cut

*/
template<typename T, typename Pred> void unlike(const T& t, Pred pred,
		const std::string& msg="");

/*

=item B<<< C<templateE<lt>typename T, typename PredE<gt> void cmp_ok(const T& actual, const T& expected, const std::string& msg="", Pred pred=equal_toE<lt>TE<gt>)> >>>

Reports success if C<pred(actual, expected)> returns C<false>, and failure
otherwise.  By default, C<pred> is defined as C<actual == expected>.

=cut

*/
template<typename T, typename Pred=equal_to<T>> void cmp_ok(const T& actual,
		const T& expected, const std::string& msg="",
		Pred pred=equal_to<T>());

/*

=item B<<< C<void todo(size_t count, const std::string& msg="", bool pred=true)> >>>

If C<pred> is C<true>, the next C<count> tests are for code that isn't
expected to be work correctly yet.  Unlike Perl's F<Test::More>, the
Acceptatron expects to be able to link to any functions mentioned in those
tests, so this may not be as helpful as it is in Perl.

=cut

*/
void todo(size_t count, const std::string& msg="", bool pred=true);

/*

=item B<<< C<void end_todo()> >>>

Regardless of any previous calls to C<todo>, specifies that the next tests
are regular tests.  Calls to C<todo> B<do not> need to be paired with calls
to C<end_todo>.  C<end_todo> simply allows you to overestimate the C<count>
passed to C<todo>.

=cut

*/
void end_todo();

/*

=item B<<< C<void pass(const std::string& msg)> >>>

Report a successful test.

=cut

*/
void pass(const std::string& msg);

/*

=item B<<< C<void fail(const std::string& msg)> >>>

Report a failed test.

=cut

*/
void fail(const std::string& msg);

/*

=item B<<< C<void BAIL_OUT(const std::string& msg="")> >>>

Immediately stop all test processing.  Exit with a status code of 255.

=cut

*/
void BAIL_OUT(const std::string& msg="");

inline void plan(size_t count)
{
	impl::planned_tests = count;
	std::cout << "TAP version 13\n1.." << count << '\n';
	impl::plan_output = true;
}

inline void done_testing(size_t count)
{
	if (count) {
		impl::planned_tests = count;
	}
	if (!impl::plan_output) {
		std::cout << "1.." << (count ? count : impl::tests) << '\n';
	}
	if (impl::planned_tests != impl::tests) {
		std::exit(255);
	}
	std::exit(std::min<int>(254, impl::failures));
}

inline void skip_all(const std::string& reason)
{
	std::cout << "1..0" << " # Skipped: " << reason << '\n';
	impl::plan_output = true;
	std::exit(0);
}

inline void diag(const std::string& msg)
{
	std::cout << "# " << msg << '\n';
}

inline void note(const std::string& msg)
{
	return diag(msg);
}

inline void ok(bool pred, const std::string& msg)
{
	do_test_output(pred, msg, "");
}

template<typename T, typename Pred> void is(const T& actual, const T& expected,
		const std::string& msg, Pred pred)
{
	std::ostringstream failure_os;
	failure_os << "# Failed test\n# got: " << tap_format(actual)
			<< "\n# expected: " << tap_format(expected);
	do_test_output(pred(actual, expected), msg, failure_os.str());
}

template<typename T, typename Pred> void isnt(const T& actual,
		const T& expected, const std::string& msg, Pred pred)
{
	std::ostringstream failure_os;
	failure_os << "# Failed test\n# got: " << tap_format(actual)
			<< "\n# expected: anything else";
	do_test_output(!pred(actual, expected), msg, failure_os.str());
}

template<typename T, typename Pred> void like(const T& t, Pred pred,
		const std::string& msg)
{
	std::ostringstream failure_os;
	failure_os << "# Failed test\n"
		<< "# " << tap_format(t) << " does not match pred";
	do_test_output(pred(t), msg, failure_os.str());
}

template<typename T, typename Pred> void unlike(const T& t, Pred pred,
		const std::string& msg)
{
	std::ostringstream failure_os;
	failure_os << "# Failed test\n"
		<< "# " << tap_format(t) << " matches pred";
	do_test_output(!pred(t), msg, failure_os.str());
}

template<typename T, typename Pred> void cmp_ok(const T& actual,
		const T& expected, const std::string& msg, Pred pred)
{
	return is(actual, expected, msg, pred);
}

inline void todo(size_t count, const std::string& msg, bool pred)
{
	if (pred) {
		impl::to_todo = count;
		impl::todo_msg = msg;
	}
}

inline void end_todo()
{
	impl::to_todo = 0;
	impl::todo_msg = "";
}

inline void pass(const std::string& msg)
{
	do_test_output(true, msg, "");
}

inline void fail(const std::string& msg)
{
	do_test_output(false, msg, "");
}

inline void BAIL_OUT(const std::string& msg)
{
	std::cout << "Bail out!";
	if (!msg.empty()) {
		std::cout << ' ' << msg << '\n';
	}
	std::exit(255);
}

namespace impl {
	template<> float parse(const std::string& s)
	{
		return std::stof(s);
	}
	
	template<> double parse(const std::string& s)
	{
		return std::stod(s);
	}
	
	template<> long double parse(const std::string& s)
	{
		return std::stold(s);
	}
	
	template<typename Real> std::string dtoa(const Real& d)
	{
		if (d != d) {
			return std::to_string(std::numeric_limits<Real>::quiet_NaN());
		}
		const size_t MAX_LEN = (d < 0 ? 1 : 0)
			+ std::log10(std::numeric_limits<Real>::max()) + 1
			+ std::numeric_limits<Real>::digits10;
		size_t digits = std::log10(std::abs(d)) + 1;
		// sign
		if (d < 0) {
			++digits;
		}
		// if there's a fractional part (i.e., space for decimal point)
		Real whole;
		if (std::modf(d, &whole)) {
			++digits;
		}
		for (; digits < MAX_LEN; ++digits) {
			std::stringstream s;
			s << std::setprecision(digits) << d;
			auto result = s.str();
			if (parse<Real>(result) == d) {
				return result;
			}
		}
		// if all else fails, fall back to default behavior
		return to_string(d);
	}

	template<typename T> std::string tap_format(const T& t)
	{
		std::ostringstream os;
		os << "'" << t << "'";
		return os.str();
	}

	template<> std::string tap_format(const float& t)
	{
		std::ostringstream os;
		os << "'" << dtoa(t) << "'";
		return os.str();
	}

	template<> std::string tap_format(const double& t)
	{
		std::ostringstream os;
		os << "'" << dtoa(t) << "'";
		return os.str();
	}

	template<> std::string tap_format(const long double& t)
	{
		std::ostringstream os;
		os << "'" << dtoa(t) << "'";
		return os.str();
	}

	void do_test_output(bool pred, const std::string& msg,
			const std::string& failure)
	{
		++impl::tests;
		std::ostringstream os;
		os << impl::tests;
		if (!msg.empty()) {
			os << " - " << msg;
		}
		if (impl::to_todo) {
			--impl::to_todo;
			os << " #TODO";
			if (!impl::todo_msg.empty()) {
				os << ' ' << impl::todo_msg;
			}
		}
		if (pred) {
			std::cout << "ok " << os.str() << '\n';
			return;
		}
		++impl::failures;
		std::cout << "not ok " << os.str();
 		if (!failure.empty()) {
			std::cout << '\n' << failure;
		}
		std::cout << '\n';
	}

	inline std::string append_location(std::string msg,
			const char* filename, int line)
	{
		return msg.append(" ")
			.append(filename)
			.append(":")
			.append(std::to_string(line));
	}
}
}
/*


=item B<<< C<APPEND_LOC(msg)> >>>

Macro to append the values of C<__FILE__> and C<__LINE__> to a copy of C<msg>.

=cut

*/
#define APPEND_LOC(msg) \
	acceptatron::impl::append_location(msg, __FILE__, __LINE__)

/*

=back

=head1 BUGS

The Acceptatron is not currently thread safe.

=head1 ACKNOWLEDGEMENTS

Loosely based on the Perl modules F<Test::Simple> and F<Test::More>.

=head1 SEE ALSO

There are similar C++ testing frameworks.  Some are listed at
L<testanything.org|http://testanything.org/producers.html#cplusplus>.

It is also possible to have a Perl program fork a C++ program -- and use
F<Test::Simple>, F<Test::More>, or another test module -- to inspect the
output.  The Acceptatron's own test suite uses this approach.

=head1 AUTHOR

L<Max Lybbert|mailto:mlybbert@users.sourceforge.net>.

=head1 COPYRIGHT

The Acceptatron is not copyrighted.  It is public domain software. I have no
interest in enforcing any terms or conditions related to the library.

I am interested in receiving bug reports or contributions (with the
understanding that any the library will continue to be released as public
domain software).

=cut

*/

#endif // ACCEPTATRON_H

