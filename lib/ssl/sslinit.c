/*
 * NSS utility functions
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "prtypes.h"
#include "prinit.h"
#include "seccomon.h"
#include "secerr.h"
#include "ssl.h"
#include "sslimpl.h"

static int ssl_inited = 0;

SECStatus
ssl_Init(void)
{
// TODO(DJ) is this always called before anything else?
// Consider doing init work here instad of EnsureInit stuff.
    if (!ssl_inited) {
	if (ssl_InitializePRErrorTable() != SECSuccess) {
	    PORT_SetError(SEC_ERROR_NO_MEMORY);
	    return (SECFailure);
	}
	ssl_inited = 1;
    }
    return SECSuccess;
}
