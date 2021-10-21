
#include <assert.h>
#include <string.h>
#include "libgsm.h"

void libgsm_init(libgsm_t* const g, bool is_enc) {
    assert(g != NULL);
    memset((char*)g, 0, sizeof(*g));
    g->nrp = 40;

    int f_fast    = 0; /* use faster fpt algorithm (-F) */
    int f_verbose = 0; /* debugging (-V) */
    int f_ltp_cut = 0; /* LTP cut-off margin (-C) */

    gsm_option(g, GSM_OPT_FAST,    &f_fast);
    gsm_option(g, GSM_OPT_VERBOSE, &f_verbose);
    if (is_enc) {
        gsm_option(g, GSM_OPT_LTP_CUT, &f_ltp_cut);
    }
}
