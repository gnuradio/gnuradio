/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "local_sighandler.h"
#include <cstring>
#include <stdexcept>

namespace gr {

local_sighandler::local_sighandler(int signum, void (*new_handler)(int))
    : d_signum(signum)
{
#ifdef HAVE_SIGACTION
    struct sigaction new_action;
    memset(&new_action, 0, sizeof(new_action));

    new_action.sa_handler = new_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    gr::configure_default_loggers(d_logger, d_debug_logger, "local_sighandler");
    if (sigaction(d_signum, &new_action, &d_old_action) < 0) {
        d_logger->error("sigaction (install new): {:s}", strerror(errno));
        throw std::runtime_error("sigaction");
    }
#endif
}

local_sighandler::~local_sighandler() noexcept(false)
{
#ifdef HAVE_SIGACTION
    if (sigaction(d_signum, &d_old_action, 0) < 0) {
        d_logger->error("sigaction (restore old): {:s}", strerror(errno));
        throw std::runtime_error("sigaction");
    }
#endif
}

void local_sighandler::throw_signal(int signum) { throw signal(signum); }

/*
 * Semi-hideous way to may a signal number into a signal name
 */
#define SIGNAME(x) \
    case x:        \
        return #x

std::string signal::name() const
{
    switch (signum()) {
#ifdef SIGHUP
        SIGNAME(SIGHUP);
#endif
#ifdef SIGINT
        SIGNAME(SIGINT);
#endif
#ifdef SIGQUIT
        SIGNAME(SIGQUIT);
#endif
#ifdef SIGILL
        SIGNAME(SIGILL);
#endif
#ifdef SIGTRAP
        SIGNAME(SIGTRAP);
#endif
#ifdef SIGABRT
        SIGNAME(SIGABRT);
#endif
#ifdef SIGBUS
        SIGNAME(SIGBUS);
#endif
#ifdef SIGFPE
        SIGNAME(SIGFPE);
#endif
#ifdef SIGKILL
        SIGNAME(SIGKILL);
#endif
#ifdef SIGUSR1
        SIGNAME(SIGUSR1);
#endif
#ifdef SIGSEGV
        SIGNAME(SIGSEGV);
#endif
#ifdef SIGUSR2
        SIGNAME(SIGUSR2);
#endif
#ifdef SIGPIPE
        SIGNAME(SIGPIPE);
#endif
#ifdef SIGALRM
        SIGNAME(SIGALRM);
#endif
#ifdef SIGTERM
        SIGNAME(SIGTERM);
#endif
#ifdef SIGSTKFLT
        SIGNAME(SIGSTKFLT);
#endif
#ifdef SIGCHLD
        SIGNAME(SIGCHLD);
#endif
#ifdef SIGCONT
        SIGNAME(SIGCONT);
#endif
#ifdef SIGSTOP
        SIGNAME(SIGSTOP);
#endif
#ifdef SIGTSTP
        SIGNAME(SIGTSTP);
#endif
#ifdef SIGTTIN
        SIGNAME(SIGTTIN);
#endif
#ifdef SIGTTOU
        SIGNAME(SIGTTOU);
#endif
#ifdef SIGURG
        SIGNAME(SIGURG);
#endif
#ifdef SIGXCPU
        SIGNAME(SIGXCPU);
#endif
#ifdef SIGXFSZ
        SIGNAME(SIGXFSZ);
#endif
#ifdef SIGVTALRM
        SIGNAME(SIGVTALRM);
#endif
#ifdef SIGPROF
        SIGNAME(SIGPROF);
#endif
#ifdef SIGWINCH
        SIGNAME(SIGWINCH);
#endif
#ifdef SIGIO
        SIGNAME(SIGIO);
#endif
#ifdef SIGPWR
        SIGNAME(SIGPWR);
#endif
#ifdef SIGSYS
        SIGNAME(SIGSYS);
#endif
    default:
#if defined(SIGRTMIN) && defined(SIGRTMAX)
        if (signum() >= SIGRTMIN && signum() <= SIGRTMAX) {
            return "SIGRTMIN + " + std::to_string(signum());
        }
#endif
        return "SIGNAL " + std::to_string(signum());
    }
}

} /* namespace gr */
