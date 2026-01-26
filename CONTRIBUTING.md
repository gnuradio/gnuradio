# Contributing to GNU Radio

Welcome! You are reading about how to contribute code to GNU Radio. First of
all, we are very happy that you're about to contribute, and welcome your
submissions! We hope many more will come.

In this document, we will explain the main things to consider when submitting
pull requests against GNU Radio. Reading this first will help a lot with
streamlining the process of getting your code merged.

There is also a [wiki-based version of this file][wikicontrib], which contains
more detail.  This file is more of a checklist.

## New to GNU Radio?

If you're new, it is a good idea to first get acquainted with GNU Radio from a
user perspective.  https://tutorials.gnuradio.org/ is a good place to start.

## What about non-code contributions?

Those are at least as important as code contributions: Emails to the mailing
list, answers on Stack Overflow, Wiki page edits, examples... We very much
appreciate those. However, this document is specifically about contributing
code.

## I want to contribute code, but can't build GNU Radio!

We love to help beginners used to development with GNU Radio! The right order
of things for most cases is:

1. **Learn to use GNU Radio**<br> [Install GNU Radio from
   packaging][installation] and [work through the tutorials][tutorials].
   Asking questions in this step is a great way to improve our packaging and
   documentation – it might be much more valuable to the project than a code
   contribution!
2. **Learn to build GNU Radio**<br> Check the 
   [platform-specific guide][platform-specific] on how to do that!<br>
   (It's not that hard, it's harder to install all the build-dependencies.)
3. **Make a code change, compile and test it.**
4. **Contribute your code change to us!**<br> This is done through the
   "pull request" mechanism on [our github project][github-project].

As always, we strive to be as helpful as possible: If you have a problem during
any of these steps, reach out to us via [chat][chat]!

## Own your contributions!

We get a lot of pull requests. That is great! Many people are doing their best
to help GNU Radio become better. However, we expect that you *own your
contribution*. We mean that in two ways:

1. The boring, copyright / code ownership way. See "DCO Signed?" below.
2. The human "be responsible for what you're doing!" way.

With the advent of LLM-aided code generation, we are getting an increasing
number of PRs where, upon asking multiple times and reading a lot of code that
seems to not quite make sense, we learn the code was never tried by the
submitter. That is frustrating for both parties – it eats time and goodwill on
the maintainer's side, and in the end, getting one's PR questioned
fundamentally is harsh on the beginner's motivation, too.

You need to *at least* be able to understand your code. It doesn't matter how
quickly you can generate high-quality code with your tool, our maintainers have
to read it; so we expect you have actually tried it before you submit it.
Reading code that just doesn't solve the actual problem although that's claimed
in the PR description is by now taking up a significant amount of the time we
have for maintaining GNU Radio. You must hence be able to **understand** what
the code does that you're committing, and **have personally tested it does what
it should**. Yes, that, in most cases, means that you need to build GNU Radio
yourself.

We want your help, even if you can't yet build GNU Radio. Please, however,
understand that we don't need help copying bug reports into an LLM tool. We are
perfectly capable of that ourselves; what we need help with is the part where
one needs to actually understand a problem and **design and verify** a
solution.

### DCO Signed?

Any code contributions going into GNU Radio will become part of a GPL-licensed,
open source repository. It is therefore imperative that code submissions belong
to the authors, and that submitters have the authority to merge that code into
the public GNU Radio codebase.

For that purpose, we use the [Developer's Certificate of Origin](DCO.txt). It
is the same document used by other projects. Signing the DCO states that there
are no legal reasons to not merge your code.

To sign the DCO, suffix your git commits with a "Signed-off-by" line. When
using the command line, you can use `git commit -s` to automatically add this
line. If there were multiple authors of the code, or other types of
stakeholders, make sure that all are listed, each with a separate Signed-off-by
line.

## Coding Guidelines

We have codified our coding guidelines in [GREP1][grep1]. Please read them, and
stick to them. For C++ code, use clang-format. For Python, PEP8 is your friend
(but again, check the actual coding guidelines).

## Python or C++?

Python is nice, and generally simpler. However, all performance-critical code
should be written in C++. The same goes for all blocks, since there are use
cases for C++-only GNU Radio.

## Git commit messages are very important

We follow standard git commit message guidelines, similar to many other open
source projects. See the [coding guidelines][grep1] for more details. In a
nutshell:
- Keep the lines below 72 characters
- Subject line has the component prepended (e.g., `runtime:`)
- Avoid empty git commit messages
- The git commit message explains the change, the code only explains the current
  state

## Include Unit Tests

If you have an obvious test, that might speed up the time it takes to convince
reviewers that your code is correct.

## Code is "Write once, read many times"; it's a liability!

You only have to write your code once – but multiple people will have to read and
understand it, be able to test whether it does what it should, and take care of
it for many years.

You can make the life of present and future project contributors better by
writing code that is clear, adding unit tests that are clear in what they test,
and where that's not possible, demonstrate you've tried the code on your own
machine.

## GitHub-specific Actions

GNU Radio is currently hosted at GitHub, where we've added some mechanics that
help with getting code merged. If you intend to develop code more than once,
request being accepted into the [gr-devs][gr-devs] group. Members of this group
can change labels and approve pull requests or request changes.

## The Buddy Principle: Submit One, Review One

When you've submitted a pull request, please take the time to review another
one. This helps make sure that there are always a number of reviews at least
equal to the number of pull requests, which means the maintainers don't get
overwhelmed when a lot is being contributed.

## Standard command line options

When writing programs that are executable from the command line,
please follow existing examples regarding their command line arguments, and
reuse them.

[chat]: https://wiki.gnuradio.org/index.php?title=Chat
[installation]: https://wiki.gnuradio.org/index.php?title=InstallingGR#Quick_Start
[tutorials]: https://tutorials.gnuradio.org
[platform-specific]: https://wiki.gnuradio.org/index.php?title=InstallingGR#Platform-specific_guides
[github-project]: https://github.com/gnuradio/gnuradio
[grep1]: https://github.com/gnuradio/greps/blob/master/grep-0001-coding-guidelines.md
[wikicontrib]: https://wiki.gnuradio.org/index.php/Development
[gr-devs]: https://github.com/orgs/gnuradio/teams/gr-devs
