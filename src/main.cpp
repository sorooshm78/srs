#include <pjsua-lib/pjsua.h>
#include <pjsip.h>
#include <iostream>

// Define port number for SIP server
#define SIP_PORT 5060

// Log levels for debugging
#define LOG_LEVEL 4

// Function to handle incoming SIP requests
static pj_bool_t on_rx_request(pjsip_rx_data *rdata)
{
    std::cout << "Received SIP message" << std::endl;

    // Parse the SIP request headers
    pjsip_msg *msg = rdata->msg_info.msg;
    if (msg->type == PJSIP_REQUEST_MSG) {
        std::cout << "Incoming SIP Request: " << msg->line.req.method.name.ptr << std::endl;
    }

    // Iterate and print all headers
    pjsip_hdr *header = msg->hdr.next;
    while (header != &msg->hdr) {
        std::cout << "Header: " << header->name.ptr << std::endl;
        header = header->next;
    }

    return PJ_TRUE;
}

// Register the PJSIP module to handle incoming requests
static pjsip_module mod_msg_handler =
{
    nullptr, nullptr,                  // prev, next
    { "mod-msg-handler", 15 },          // Name
    -1,                                 // ID
    PJSIP_MOD_PRIORITY_TRANSPORT_LAYER, // Priority
    nullptr,                            // load()
    nullptr,                            // start()
    nullptr,                            // stop()
    nullptr,                            // unload()
    &on_rx_request,                     // on_rx_request()
    nullptr,                            // on_rx_response()
    nullptr,                            // on_tx_request()
    nullptr,                            // on_tx_response()
    nullptr,                            // on_tsx_state()
};

// Initialization of PJSIP
void init_pjsip()
{
    pjsua_config cfg;
    pjsua_logging_config log_cfg;

    // Initialize PJSUA (PJSIP's high level API)
    pjsua_create();

    // Configure PJSUA
    pjsua_config_default(&cfg);

    pjsua_logging_config_default(&log_cfg);
    log_cfg.console_level = LOG_LEVEL;

    // Initialize PJSUA with configuration
    pjsua_init(&cfg, &log_cfg, nullptr);

    // Add UDP transport on the specified port (5060 by default)
    pjsua_transport_config transport_cfg;
    pjsua_transport_config_default(&transport_cfg);
    transport_cfg.port = SIP_PORT;
    pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, nullptr);

    // Start PJSUA
    pjsua_start();

    // Register custom module to intercept incoming SIP messages
    pjsip_endpt_register_module(pjsua_get_pjsip_endpt(), &mod_msg_handler);
}

int main()
{
    try {
        // Initialize PJSIP library
        init_pjsip();

        std::cout << "SIP server is listening on port " << SIP_PORT << std::endl;

        // Keep the server running indefinitely
        while (true) {
            // Perform necessary SIP operations
            pjsua_handle_events(100);
        }

        // Clean up PJSIP before exit
        pjsua_destroy();
    } catch (const std::exception &e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
