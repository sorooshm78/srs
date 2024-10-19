#include <pjsip-ua/sip_siprec.h>
#include <pjsip/print_util.h>
#include <pjsip/sip_endpoint.h>
#include <pjsip/sip_event.h>
#include <pjsip/sip_transaction.h>
#include <pj/log.h>
#include <pj/math.h>
#include <pj/os.h>
#include <pj/pool.h>


#define THIS_FILE               "sip_siprec.c"


static const pj_str_t STR_SIPREC         = {"siprec", 6};


/*
 * Initialize Session Timers support in PJSIP. 
 */
PJ_DEF(pj_status_t) pjsip_siprec_init_module(pjsip_endpoint *endpt)
{
    pj_status_t status;

    PJ_ASSERT_RETURN(endpt, PJ_EINVAL);

    if (is_initialized)
        return PJ_SUCCESS;

    /* Register Session-Expires header parser */
    status = pjsip_register_hdr_parser( STR_SE.ptr, STR_SHORT_SE.ptr, 
                                        &parse_hdr_se);
    if (status != PJ_SUCCESS)
        return status;

    /* Register Min-SE header parser */
    status = pjsip_register_hdr_parser( STR_MIN_SE.ptr, NULL, 
                                        &parse_hdr_min_se);
    if (status != PJ_SUCCESS)
        return status;

    /* Register 'timer' capability to endpoint */
    status = pjsip_endpt_add_capability(endpt, NULL, PJSIP_H_SUPPORTED,
                                        NULL, 1, &STR_TIMER);
    if (status != PJ_SUCCESS)
        return status;

    /* Register deinit module to be executed when PJLIB shutdown */
    if (pjsip_endpt_atexit(endpt, &pjsip_timer_deinit_module) != PJ_SUCCESS) {
        /* Failure to register this function may cause this module won't 
         * work properly when the stack is restarted (without quitting 
         * application).
         */
        pj_assert(!"Failed to register Session Timer deinit.");
        PJ_LOG(1, (THIS_FILE, "Failed to register Session Timer deinit."));
    }

    is_initialized = PJ_TRUE;

    return PJ_SUCCESS;
}
