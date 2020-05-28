#include <application.h>
#define SWITCHES_RELAY_ID 0
#define WHEELS_RELAY_ID 1


// LED instance
bc_led_t led;

// Button instance
bc_button_t switches;
bc_button_t wheels;

bc_module_relay_t switchesRelay;
bc_module_relay_t wheelsRelay;

bool switchesReset = false;
bool wheelsReset = false;

void reset_relay(uint64_t *id, const char *topic, void *value, void *param);

static const bc_radio_sub_t subs[] = {
    // state/set
    {"safety/switches/-/reset", BC_RADIO_SUB_PT_BOOL, reset_relay, (void *) SWITCHES_RELAY_ID},
    {"safety/wheels/-/reset", BC_RADIO_SUB_PT_BOOL, reset_relay, (void *) WHEELS_RELAY_ID}
};


void reset_relay(uint64_t *id, const char *topic, void *value, void *param)
{
    uint32_t relay = (uint32_t) param;

    if(relay == SWITCHES_RELAY_ID)
    {
        switchesReset = true;
        bc_module_relay_set_state(&switchesRelay, false);
        bc_radio_pub_state(BC_RADIO_PUB_STATE_RELAY_MODULE_0, false);

    }
    else if(relay == WHEELS_RELAY_ID)
    {
        wheelsReset = true;
        bc_module_relay_set_state(&wheelsRelay, false);
        bc_radio_pub_state(BC_RADIO_PUB_STATE_RELAY_MODULE_1, false);

    }
}

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{


    if(self->_channel.gpio == 0)
    {
        bc_log_debug("sepnuto tlacitko 1");
        if (event == BC_BUTTON_EVENT_PRESS && !(bc_module_relay_get_state(&switchesRelay)))
        {
            switchesReset = false;
            bc_module_relay_set_state(&switchesRelay, true);
            bc_radio_pub_state(BC_RADIO_PUB_STATE_RELAY_MODULE_0, true);
        }
        else if(event == BC_BUTTON_EVENT_RELEASE && switchesReset)
        {
            bc_module_relay_set_state(&switchesRelay, false);
            bc_radio_pub_state(BC_RADIO_PUB_STATE_RELAY_MODULE_0, false);

        }
    }
    else if(self->_channel.gpio == 1)
    {
        if (event == BC_BUTTON_EVENT_PRESS && !(bc_module_relay_get_state(&wheelsRelay)))
        {
            wheelsReset = false;
            bc_module_relay_set_state(&wheelsRelay, true);
            bc_radio_pub_state(BC_RADIO_PUB_STATE_RELAY_MODULE_1, true);
        }
        else if(event == BC_BUTTON_EVENT_RELEASE && wheelsReset)
        {
            bc_module_relay_set_state(&wheelsRelay, false);
            bc_radio_pub_state(BC_RADIO_PUB_STATE_RELAY_MODULE_1, false);


        }
    }

    
}

void application_init(void)
{

    bc_led_init(&led, BC_GPIO_LED, false, 0);

    bc_led_blink(&led, 2);

    bc_log_init(BC_LOG_LEVEL_DUMP, BC_LOG_TIMESTAMP_ABS);
    

    bc_log_debug("start");

    bc_button_init(&switches, BC_GPIO_P0, BC_GPIO_PULL_DOWN, false);
    bc_button_init(&wheels, BC_GPIO_P1, BC_GPIO_PULL_DOWN, false);


    bc_button_set_event_handler(&switches, button_event_handler, NULL);
    bc_button_set_event_handler(&wheels, button_event_handler, NULL);


    bc_radio_init(BC_RADIO_MODE_NODE_LISTENING);
    bc_radio_set_subs((bc_radio_sub_t *) subs, sizeof(subs)/sizeof(bc_radio_sub_t));
    
    bc_module_relay_init(&switchesRelay, 0x3B);
    bc_module_relay_set_state(&switchesRelay, false);

    bc_module_relay_init(&wheelsRelay, 0x3F);
    bc_module_relay_set_state(&wheelsRelay, false);

    bc_radio_pairing_request("first-room-safety", VERSION);
}
